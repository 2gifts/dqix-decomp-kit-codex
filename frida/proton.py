#!/usr/bin/env python3
"""Run a command with mwccarm under Steam's Proton wine, so the Frida tools work on Linux.

    python frida/proton.py python frida/colorforce.py <src> ov017 <addr> <size>
    python frida/proton.py python regress.py

The decomp runs mwccarm through wibo, which is statically linked: Frida cannot inject into it.
Proton's 32-bit wine is dynamically linked and can be. Only commands run through this use Proton;
everything else keeps wibo.

    DQIX_PROTON         Proton's files/ directory (default: Steam's "Proton 9.0 (Beta)")
    DQIX_PROTON_PREFIX  the Wine prefix (default: ~/.local/share/dqix-proton/pfx), built on first use
"""
import os
import shutil
import sys

HOME = os.path.expanduser("~")
PROTON = os.environ.get("DQIX_PROTON") or f"{HOME}/.local/share/Steam/steamapps/common/Proton 9.0 (Beta)/files"
PFX = os.environ.get("DQIX_PROTON_PREFIX") or f"{HOME}/.local/share/dqix-proton/pfx"


def make_prefix():
    src = os.path.join(PROTON, "share", "default_pfx")
    tmp = PFX + ".tmp"
    shutil.rmtree(tmp, ignore_errors=True)
    shutil.copytree(src, tmp, symlinks=True)
    for root, dirs, files in os.walk(os.path.join(tmp, "drive_c")):
        for name in dirs + files:
            p = os.path.join(root, name)
            if os.path.islink(p) and not os.path.isabs(os.readlink(p)):
                target = os.path.normpath(os.path.join(src, os.path.relpath(root, tmp), os.readlink(p)))
                os.remove(p)
                os.symlink(target, p)
    os.symlink("../drive_c", os.path.join(tmp, "dosdevices", "c:"))
    os.symlink("/", os.path.join(tmp, "dosdevices", "z:"))
    with open(os.path.join(tmp, ".update-timestamp"), "w") as fh:
        fh.write("disable")
    os.makedirs(os.path.dirname(PFX), exist_ok=True)
    os.rename(tmp, PFX)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    wine = os.path.join(PROTON, "bin", "wine")
    if not os.path.isfile(wine):
        sys.exit(f"no Proton wine at {wine}; set DQIX_PROTON to a Proton install's files/ directory")
    if not os.path.isdir(PFX):
        make_prefix()
    link = os.path.join(os.path.dirname(PFX), "files")
    if os.path.realpath(link) != os.path.realpath(PROTON):
        tmp = f"{link}.{os.getpid()}"
        os.symlink(PROTON, tmp)
        os.replace(tmp, link)
    wine = os.path.join(link, "bin", "wine")
    env = dict(os.environ, DQIX_WINE=wine, WINEPREFIX=PFX, WINEDEBUG="-all", WINEDLLOVERRIDES="mscoree=;mshtml=",
               WINELOADERNOEXEC="1")
    env.pop("DISPLAY", None)
    env.pop("WAYLAND_DISPLAY", None)
    os.execvpe(sys.argv[1], sys.argv[1:], env)
