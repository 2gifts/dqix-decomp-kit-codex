"""Score a candidate under EVERY mwccarm build, with the per-case classified score.

The earlier compiler matrix compared raw byte distance on a source whose frame was still wrong, so
every 2.0 build looked identical and the question was written off. Byte distance is the wrong
objective (it counts relocations and pools) and a wrong-length source hides build differences
anyway. This runs pad/casescore.py under each build instead, which is the number that tracks real
work — and some functions in this project only match under a non-default build, so the build is a
variable to search, not a constant.

Usage: python ccscore.py <file.cpp> [--only 2.0]
"""
import glob
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff
import casescore

src = sys.argv[1]
only = None
for i, a in enumerate(sys.argv):
    if a == "--only":
        only = sys.argv[i + 1]

builds = sorted(glob.glob(f"{casediff.buildcfg.COMPILER_ROOT}/*/*/mwccarm.exe"))
default = casediff.CC
rows = []
for cc in builds:
    name = "/".join(cc.replace("\\", "/").split("/")[-3:-1])
    if only and not name.startswith(only):
        continue
    casediff.CC = cc
    try:
        n, detail = casescore.score(src)
    except Exception as exc:                      # a build that cannot parse the source at all
        n, detail = 10 ** 6, "error: %s" % str(exc)[:60]
    rows.append((n, name, detail))
    print("%-16s %-8d %s" % (name, n, detail), flush=True)
casediff.CC = default
rows.sort()
print("\nBEST %s -> %d  %s" % (rows[0][1], rows[0][0], rows[0][2]))
