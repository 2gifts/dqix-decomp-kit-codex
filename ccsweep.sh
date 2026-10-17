#!/bin/bash
# Try every installed mwccarm build (optionally with extra flags) against one function.
# Library code in the ROM was built by whatever toolchain shipped that library, so a function
# whose SHAPE is right but whose size is stubbornly wrong is usually a different compiler build,
# not a different source. Usage: ccsweep.sh <OV|main> <addr> <file.cpp> ["extra flags"]
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
REPO="$(python "$KIT/kitpaths.py" repo)"
OV="$1"; ADDR="$2"; SRC="$3"; EXTRA="$4"
cd "$REPO" || exit 2

COMPILER_ROOT="$(python "$KIT/buildcfg.py" --compiler-root)" || exit 2
for d in "$COMPILER_ROOT"/*/*/mwccarm.exe; do
  [ -f "$d" ] || continue
  ver=${d#"$COMPILER_ROOT"/}
  ver=${ver%/mwccarm.exe}
  out=$(MWCC="$ver" WDIFF_FLAGS="$EXTRA" python "$KIT/wdiff.py" "$OV" "$ADDR" "$SRC" </dev/null 2>&1 | head -1)
  printf '%-14s %s\n' "$ver" "$out"
done
echo "CCSWEEP DONE"
