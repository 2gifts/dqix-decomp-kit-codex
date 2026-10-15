#!/bin/bash
# THE kill for this project. Use nothing else.
#
# Why this exists. Git-Bash `kill -9` on a worker only kills the POSIX-side `timeout`/`claude` wrapper.
# The real spender is a WINDOWS process (`claude.exe -p "DQIX decomp worker…"`), which survives, gets
# reparented to PID 1, and keeps burning tokens with no driver left to collect its output. Measured
# 08-12: three orphaned claude.exe workers ran on after their driver was killed, outside the governor,
# and every earlier "killed the fleet" in this session left some behind. limit_guard's kill_workers has
# the same hole -- it scans /proc cmdlines, which never sees a detached claude.exe.
#
# Order matters: supervisor FIRST (else it relaunches run_all mid-kill), then drivers, then workers.
# Workers are killed on BOTH sides: POSIX wrappers and the Windows claude.exe children.
#
# The session's own interactive claude.exe is identified by NOT carrying the worker prompt, so it is
# never a candidate -- do not "simplify" this to killing all claude.exe.
#
# Usage:
#   bash killfleet.sh              kill workers only (leave supervisor/drivers running)
#   bash killfleet.sh --all        kill supervisor, drivers and workers
#   bash killfleet.sh --orphans    kill only workers whose parent is gone
#   bash killfleet.sh --dry        report what would be killed, kill nothing
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
MODE="${1:-workers}"
DRY=0
[ "$MODE" = "--dry" ] && { DRY=1; MODE="report"; }
WIN=0; PSALL=-e
case "$OSTYPE" in msys*|cygwin*) WIN=1; PSALL= ;; esac

say() { [ "$DRY" -eq 1 ] && echo "  would kill: $*" || echo "  killed: $*"; }

posix_pids() {   # $1 = cmdline substring
  for p in $(ps $PSALL 2>/dev/null | awk 'NR>1{print $1}'); do
    c=$(tr '\0' ' ' 2>/dev/null < "/proc/$p/cmdline") || continue
    case "$c" in *"$1"*) echo "$p";; esac
  done
}

kill_posix() {   # $1 = substring, $2 = label
  local n=0 p
  for p in $(posix_pids "$1"); do
    [ "$DRY" -eq 1 ] || kill -9 "$p" 2>/dev/null
    n=$((n+1))
  done
  [ "$n" -gt 0 ] && say "$n $2 (posix)"
  return 0
}

# Windows side. Match on the worker PROMPT text, never on the image name: the operator's own
# interactive claude.exe has no such prompt and must survive.
kill_windows_workers() {
  local only_orphans="$1"
  if [ "$WIN" -eq 0 ]; then
    [ "$only_orphans" = "1" ] || return 0
    local n=0 p q
    for p in $(posix_pids "DQIX decomp worker"); do
      q=$p
      while q=$(ps -o ppid= -p "$q" 2>/dev/null | tr -d ' ') && [ "${q:-0}" -gt 1 ]; do
        case "$(tr '\0' ' ' 2>/dev/null < "/proc/$q/cmdline")" in *pull_worker.sh*) continue 2 ;; esac
      done
      [ "$DRY" -eq 1 ] || kill -9 "$p" 2>/dev/null
      n=$((n+1))
    done
    say "$n orphaned workers (posix)"
    return 0
  fi
  # NO -Filter. Written as "Name=''claude.exe''" inside a bash SINGLE-quoted string,
  # the doubled quotes close and reopen the bash quote instead of escaping anything,
  # so PowerShell received the invalid filter Name=claude.exe and matched NOTHING.
  # This function -- the one that actually kills -- was therefore a no-op on the
  # Windows side for its whole life, while the verify line at the bottom (correctly
  # escaped, double-quoted) reported "0 alive" and looked like confirmation.
  # Measured 08-17: `--dry` said "would kill 0" with 3 workers plainly running.
  # Match in Where-Object from a DOUBLE-quoted string with $_ escaped.
  local ps1="Get-CimInstance Win32_Process | Where-Object { \$_.Name -eq 'claude.exe' -and \$_.CommandLine -like '*DQIX decomp worker*' }"
  if [ "$only_orphans" = "1" ]; then
    # orphan = its parent process no longer exists
    ps1="$ps1 | Where-Object { -not (Get-Process -Id \$_.ParentProcessId -ErrorAction SilentlyContinue) }"
  fi
  if [ "$DRY" -eq 1 ]; then
    local n
    n=$(powershell -NoProfile -Command "@($ps1).Count" 2>/dev/null | tr -d '\r')
    say "${n:-0} claude.exe workers (windows)"
  else
    local n
    n=$(powershell -NoProfile -Command "\$x=@($ps1); \$x | ForEach-Object { Stop-Process -Id \$_.ProcessId -Force -ErrorAction SilentlyContinue }; \$x.Count" 2>/dev/null | tail -1 | tr -d '\r')
    say "${n:-0} claude.exe workers (windows)"
  fi
}

echo "killfleet: mode=$MODE"
case "$MODE" in
  --all)
    kill_posix "$KIT/supervise.sh" "supervisor"      # first, or it relaunches the driver mid-kill
    rm -f "$SP/supervise.pid"
    # THE DRIVER IS pull_all.sh. This named only run_all/run_overlay/run_main, all deleted on
    # 2026-08-20, so `killfleet.sh --all` -- the escalation the stop procedure falls back to when
    # something survives -- killed the supervisor, reported success, and left the actual fleet
    # spending. The old names stay in the list because a stale checkout can still have them.
    kill_posix "$KIT/pull_all.sh" "pull_all driver"
    kill_posix "$KIT/pull_worker.sh" "pull workers"
    rm -f "$SP/run_all.lock" "$SP/pull_all.pid"
    kill_posix "DQIX decomp worker" "worker wrappers"
    kill_windows_workers 0
    ;;
  --orphans)
    kill_windows_workers 1
    ;;
  workers|report)
    kill_posix "DQIX decomp worker" "worker wrappers"
    kill_windows_workers 0
    ;;
  *)
    echo "usage: killfleet.sh [--all|--orphans|--dry]"; exit 2;;
esac

sleep 3
if [ "$WIN" -eq 0 ]; then left=$(posix_pids "DQIX decomp worker" | wc -l); else
left=$(powershell -NoProfile -Command "@(Get-CimInstance Win32_Process -Filter \"Name='claude.exe'\" | Where-Object { \$_.CommandLine -like '*DQIX decomp worker*' }).Count" 2>/dev/null | tr -d '\r')
fi
echo "  verify: ${left:-?} worker claude.exe still alive (want 0)"
[ "${left:-1}" = "0" ] || echo "  WARNING: workers survived — investigate before assuming the fleet is down"
