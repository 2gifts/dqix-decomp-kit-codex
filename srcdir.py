#!/usr/bin/env python3
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
# Where new work for a module goes. The convention is src/Combat/Main and src/Combat/Overlay_N,
# but the update-compiler base moved overlay 33 to src/Filesystem/Overlay_33 and spread ARM9 main
# over eleven directories, so the module's own delinks.txt is the authority whenever the
# conventional directory is not one of the ones it names. Usage: python srcdir.py <OV|main>
import collections
import os
import re
import sys

import buildcfg

REPO = _kp.REPO
SRC_RE = re.compile(r"(?m)^\s*(src/[^:\s]+\.(?:cpp|c))\s*:")


def config_dir(mod):
    return buildcfg.config_dir(mod)


def for_module(mod):
    default = "src/Combat/Main" if mod == "main" else f"src/Combat/Overlay_{int(mod)}"
    path = os.path.join(REPO, config_dir(mod), "delinks.txt")
    try:
        text = open(path, encoding="utf-8").read()
    except IOError:
        return default
    dirs = collections.Counter(os.path.dirname(p) for p in SRC_RE.findall(text))
    if not dirs or default in dirs:
        return default
    return dirs.most_common(1)[0][0]


if __name__ == "__main__":
    if sys.argv[1] == "--lib":
        print(buildcfg.lib_dir(sys.argv[2], sys.argv[3]) if len(sys.argv) > 3 else " ".join(buildcfg.lib_dirs(sys.argv[2])))
    else:
        print(for_module(sys.argv[1]))
