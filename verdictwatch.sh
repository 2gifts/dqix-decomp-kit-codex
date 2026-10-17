#!/bin/bash
# Emit one line per worker verdict, across EVERY module. Usage: bash verdictwatch.sh [seconds]
#
# Tailing named slot logs does not work: the log is per MODULE (`pull_<mod>_s<slot>.log`) and the
# dispatcher picks a different module on almost every claim, so a tail of pull_main_s1.log is blind
# to every verdict on ov003, ov006, ov024... and a new module's log does not exist when the tail
# starts, so a glob does not fix it either. This re-globs each pass and remembers the byte offset it
# has already reported per file, so nothing is missed and nothing is repeated.
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
ONCE=0
[ "$1" = "--once" ] && { ONCE=1; shift; }
EVERY="${1:-60}"
STATE="$SP/wlog/.verdict_offsets"
LOG="$SP/wlog/verdictwatch.log"

if [ -f "$SP/verdictwatch.pid" ] && kill -0 "$(cat "$SP/verdictwatch.pid" 2>/dev/null)" 2>/dev/null; then
  exit 0
fi
echo $$ > "$SP/verdictwatch.pid"
trap 'rm -f "$SP/verdictwatch.pid"' EXIT
mkdir -p "$SP/wlog"
touch "$STATE"

declare -A OFF
while :; do
  OFF=()
  while read -r b off; do
    [ -n "$b" ] && OFF[$b]=$off
  done < "$STATE"
  changed=0
  sizes=$(stat -c '%s %n' "$SP"/wlog/pull_*_s*.log 2>/dev/null)
  while read -r now f; do
    [ -f "$f" ] || continue
    b=${f##*/}
    off=${OFF[$b]}
    case "$off" in ''|*[!0-9]*) OFF[$b]=$now; changed=1; continue ;; esac
    if [ "$now" -gt "$off" ]; then
      tail -c "+$((off + 1))" "$f" \
        | grep -E '^[0-9]{2}:[0-9]{2} s[0-9]+ (MATCH|miss|presweep|postsweep)' \
        | sed "s|^|${b%.log} |"
    fi
    [ "$now" -ne "$off" ] && { OFF[$b]=$now; changed=1; }
  done <<< "$sizes"
  if [ "$changed" -eq 1 ]; then
    for b in "${!OFF[@]}"; do printf '%s %s\n' "$b" "${OFF[$b]}"; done > "$STATE.tmp"
    mv -f "$STATE.tmp" "$STATE"
  fi
  [ "$ONCE" -eq 1 ] && exit 0
  sleep "$EVERY"
done
