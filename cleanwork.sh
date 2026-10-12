#!/bin/bash
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
REPO="$(python "$KIT/kitpaths.py" repo)"
QUEUE="${1:-$SP/wlog/clean_queue.tsv}"
LOG="$SP/wlog/clean.log"
WORK="$SP/handwork/clean"
MANIFEST="$SP/clean/manifest.tsv"

_knob() { local v; v=$(tr -dc "$2" < "$1" 2>/dev/null); echo "${v:-$3}"; }
vcount() {
  python -c "import re,sys;H=re.compile(r'0x0?(?:4[0-9a-fA-F]{6}|2[37]f[0-9a-fA-F]{4})\b|^\s*#');print(sum(len(re.findall(r'\bvolatile\b',l)) for l in open(sys.argv[1],encoding='utf-8',errors='ignore') if not H.search(l)))" "$1"
}
pcount() { python "$KIT/plausible.py" "$1" 2>/dev/null | grep -vc " VOLATILE-"; }
finished() { grep -qE " (CLEAN|KEEP|BLOCKED|SKIP) $1 " "$LOG" 2>/dev/null; }
land() {
  [ -s "$MANIFEST" ] || return 0
  LANDCLEAN_TRIES=180 bash "$KIT/landclean.sh" 2>&1 | sed "s/^/$(date '+%H:%M') landclean: /" >> "$LOG"
}

mkdir -p "$WORK" "$SP/clean"
git -C "$REPO" fetch -q origin decomp-matching
while IFS=$'\t' read -r -u 3 MOD ADDR REL NOTE; do
  { [ -e "$SP/STOP_CLEAN" ] || [ -e "$SP/FLEET_STOPPED" ]; } && break
  [ -n "$ADDR" ] || continue
  finished "$ADDR" && continue
  F="$WORK/$MOD/$(basename "$REL")"
  mkdir -p "$(dirname "$F")"
  BASE=$(git -C "$REPO" rev-parse "origin/decomp-matching:$REL" 2>/dev/null)
  if [ -z "$BASE" ]; then
    echo "$(date '+%H:%M') SKIP $ADDR not on decomp-matching: $REL" >> "$LOG"
    continue
  fi
  git -C "$REPO" show "origin/decomp-matching:$REL" > "$F"
  before=$(vcount "$F")
  pbefore=$(pcount "$F")
  if [ "$before" -eq 0 ] 2>/dev/null; then
    echo "$(date '+%H:%M') SKIP $ADDR no volatile left outside hardware" >> "$LOG"
    continue
  fi
  DOC=$(python "$KIT/recipe_select.py" "$MOD" "$ADDR" --lean 2>/dev/null | tr -d '\r' | tr '\\' '/' | head -1)
  MODEL=$(_knob "$SP/CLEAN_MODEL" 'a-z0-9.-' opus)
  CAP=$(_knob "$SP/CLEAN_CAP" '0-9.' 4)
  OUT="$SP/wlog/clean_${MOD}_${ADDR}.json"
  echo "$(date '+%H:%M') claim $ADDR $MOD $(basename "$REL") volatile=$before [$MODEL] cap \$$CAP" >> "$LOG"
  cd "$WORK" || exit 1
  WGATE_SESSION="clean_$(date +%s)" timeout -k 30 "${HANG_GUARD:-7200}" claude -p "DQIX cleanup worker, module $MOD.
$F is a byte-exact source of $ADDR that already landed on decomp-matching. Make it read like the
source the original developers wrote, and keep it byte-exact.
It uses volatile $before time(s) on data that is not a hardware register or memory shared with the
ARM7. There, volatile almost always stands in for a construct nobody found yet: a field the source
really reads twice, an inline accessor, a different field type, a struct copy, a different
declaration or statement order. Remove each such volatile and find that construct.
Keep a volatile only on a hardware register, on memory shared with the ARM7, or on a variable an
interrupt handler or another thread writes while this code waits on it, and say which.
Never introduce another construct the developers could not have written: no parameter the callers
never pass, no self-assignment, no zero-negation padding, no dead address-taking local, no codegen
#pragma. python $KIT/plausible.py $F must not report anything it did not report before.
Codegen levers: grep -n '^##' $KIT/worker_src/core.md, then read only the sections you need with
grep -n -A60 '<heading>' $KIT/worker_src/core.md.
${DOC:+Module recipes: $DOC.}
Target listing: python $KIT/wlist.py $MOD $ADDR
Gate after every edit: python $KIT/wgate.py $MOD $ADDR $F
Edit only $F. When the gate prints STOP, stop.
If a construct replaced a volatile, append ONE tab-separated line to $SP/wlog/levers.tsv:
${ADDR}<TAB><size><TAB><bytes off without the volatile><TAB><the construct that replaced it>
Reply with exactly one line:
  CLEAN $ADDR
  KEEP $ADDR <which data, and why it is really volatile>
  BLOCKED $ADDR <CLASS> <metric>" \
    --model "$MODEL" --permission-mode dontAsk --safe-mode --autocompact auto \
    --disallowedTools "WebFetch" "WebSearch" "NotebookEdit" "Task" "Agent" "TodoWrite" "Artifact" "Monitor" "Skill" \
    --max-budget-usd "$CAP" --output-format json \
    --allowedTools "Bash(python:*)" "Bash(grep:*)" "Write" "Read" "Edit" "Grep" "Glob" \
    --add-dir "$SP" \
    > "$OUT" 2>/dev/null < /dev/null
  cd "$KIT" || exit 1
  cost=$(python -c "import json,sys;print('%.4f' % json.load(open(sys.argv[1]))['total_cost_usd'])" "$OUT" 2>/dev/null || echo "$CAP")
  said=$(python -c "import json,sys;print((json.load(open(sys.argv[1])).get('result') or '').strip().splitlines()[-1])" "$OUT" 2>/dev/null)
  after=$(vcount "$F")
  if python "$KIT/wgate.py" "$MOD" "$ADDR" "$F" 2>&1 | tail -1 | grep -q "^MATCH" && [ "$after" -lt "$before" ] \
     && [ "$(pcount "$F")" -le "$pbefore" ]; then
    mkdir -p "$SP/clean/$(dirname "$REL")"
    cp "$F" "$SP/clean/$REL"
    printf '%s\t%s\t%s\n' "$REL" "$BASE" "volatile $before -> $after" >> "$MANIFEST"
    v=CLEAN
  elif [ "${said%% *}" = "KEEP" ]; then
    v=KEEP
  else
    v=BLOCKED
  fi
  echo "$(date '+%H:%M') $v $ADDR \$$cost volatile $before->$after | ${said:0:200}" >> "$LOG"
  [ "$(wc -l < "$MANIFEST" 2>/dev/null || echo 0)" -ge "$(_knob "$SP/CLEAN_BATCH" '0-9' 8)" ] && land
done 3< "$QUEUE"
land
echo "$(date '+%H:%M') cleanwork done" >> "$LOG"
