#!/bin/bash
# Prepare a Claude Code cloud session (Ubuntu) to match DQIX functions with this kit.
# Needs DQIX_ROM_URL in the cloud environment's variables: a Google Drive share link or a direct
# download URL for your own USA ROM dump. The ROM never goes into any repository.
# Re-running is safe: finished steps are skipped.
set -euo pipefail

KIT="$(cd "$(dirname "$0")/.." && pwd)"
ROOT="$(dirname "$KIT")"
REPO="$ROOT/dqix-decomp"
STATE="$ROOT/dqix-kit-state"
VENV=/opt/dqix-venv
ROM_SHA1=c7c3014c237900c8281289b8bc76a781969b6278

: "${DQIX_ROM_URL:?set DQIX_ROM_URL in the cloud environment variables}"

if [ ! -x "$VENV/bin/python" ]; then
  python3 -m venv "$VENV"
fi
PY="$VENV/bin/python"
"$PY" -m pip install -q --upgrade pip
"$PY" -m pip install -q ninja capstone pyelftools

if [ ! -d "$REPO/.git" ]; then
  git clone -q --branch decomp-matching --single-branch https://github.com/ZevyaDev/dqix-decomp.git "$REPO"
fi
"$PY" -m pip install -q -r "$REPO/tools/requirements.txt"

ROM="$REPO/extract/baserom_dqix_usa.nds"
if [ ! -f "$ROM" ] || [ "$(sha1sum "$ROM" | cut -d' ' -f1)" != "$ROM_SHA1" ]; then
  mkdir -p "$REPO/extract"
  url="$DQIX_ROM_URL"
  id=$(printf '%s' "$url" | sed -n 's#.*/d/\([A-Za-z0-9_-]*\).*#\1#p; s#.*[?&]id=\([A-Za-z0-9_-]*\).*#\1#p' | head -1)
  if [ -n "$id" ]; then
    url="https://drive.usercontent.google.com/download?id=$id&export=download&confirm=t"
  fi
  curl -fsSL -o "$ROM" "$url"
  got=$(sha1sum "$ROM" | cut -d' ' -f1)
  if [ "$got" != "$ROM_SHA1" ]; then
    echo "ROM sha1 is $got, expected $ROM_SHA1 (wrong file, or the link returned a web page)" >&2
    rm -f "$ROM"
    exit 1
  fi
fi

export PATH="$VENV/bin:$PATH"
cd "$REPO"
"$PY" tools/configure.py usa
ninja wibo dsd objdiff-cli tools/mwccarm/2.0/sp2p2/mwccarm.exe extract

mkdir -p "$STATE"
cd "$KIT"
DQIX_NO_FRESHNESS=1 "$PY" kit_init.py
echo "BOOTSTRAP OK: kit $KIT, decomp $REPO, state $STATE, python $PY"
