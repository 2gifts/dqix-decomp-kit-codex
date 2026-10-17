#!/bin/bash
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
source "$KIT/wavelock.sh"
if ! wave_lock_acquire "${REGIONSYNC_TRIES:-900}"; then
  echo "REFUSED: $LOCK is held by another integration"
  exit 3
fi
trap wave_lock_release EXIT
export DQIX_MAIN_REPO="${DQIX_MAIN_REPO:-$(python "$KIT/kitpaths.py" repo)}"
REPO="$(python "$KIT/integ_tree.py" sync)" || { echo "FATAL: no integration tree"; exit 2; }
cd "$REPO" || exit 2
H0=$(git rev-parse HEAD)
python "$KIT/regionsync.py"
RC=$?
if [ "$(git rev-parse HEAD)" != "$H0" ]; then
  python "$KIT/integ_tree.py" publish
  case $? in
    0|2) ;;
    *) echo "REFUSED: integ_tree.py publish failed; the ports stay committed in $REPO"; RC=1 ;;
  esac
fi
exit $RC
