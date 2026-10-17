"""Brute-force the COMPILER FLAGS, the axis nobody has searched.

`pad/ccscore.py` searches the mwccarm build and `pad/pragmasweep.py` the pragma state; the command
line has always been assumed to be the project's. It is only an assumption -- the ROM's translation
unit had whatever flags its makefile had, and -proc / -opt / -char change scheduling and register
allocation, not just instruction selection.

Reports the per-case classified score and the reloc-masked byte distance, so a flag that reorders
registers without changing sizes is visible.

Usage: MWCC=2.0/sp2p2 python flagsweep.py <ABS src>
"""
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff
import casescore

BASE = list(casediff.FLAGS)

VARIANTS = [
    ("(base)", []),
    ("-proc arm7tdmi", ["-proc", "arm7tdmi"]),
    ("-proc arm9tdmi", ["-proc", "arm9tdmi"]),
    ("-proc arm946e", ["-proc", "arm946e"]),
    ("-proc arm966e", ["-proc", "arm966e"]),
    ("-proc arm968e", ["-proc", "arm968e"]),
    ("-proc arm1020e", ["-proc", "arm1020e"]),
    ("-O4", ["-O4"]),
    ("-O3", ["-O3"]),
    ("-O2,p", ["-O2,p"]),
    ("-O2,s", ["-O2,s"]),
    ("-Op", ["-Op"]),
    ("-opt speed", ["-opt", "speed"]),
    ("-opt space", ["-opt", "space"]),
    ("-opt level=2", ["-opt", "level=2"]),
    ("-opt noglobal", ["-opt", "noglobal"]),
    ("-opt nopeephole", ["-opt", "nopeephole"]),
    ("-opt noschedule", ["-opt", "noschedule"]),
    ("-opt schedule", ["-opt", "schedule"]),
    ("-char unsigned", ["-char", "unsigned"]),
    ("-enum min", ["-enum", "min"]),
    ("-sym off", ["-sym", "off"]),
    ("-inline auto", ["-inline", "auto"]),
    ("-inline off", ["-inline", "off"]),
    ("-interworking off", ["-nointerworking"]),
    ("-fp none", ["-fp", "none"]),
    ("-str reuse", ["-str", "reuse"]),
    ("-align 8", ["-align", "8"]),
    ("-msgstyle gcc", ["-msgstyle", "gcc"]),
]


def masked_bytes(text):
    rom = casediff.rom_text()
    n = 0
    for i in range(0, min(len(rom), len(text)), 4):
        a, b = rom[i:i + 4], text[i:i + 4]
        if a == b:
            continue
        if a[3] & 0x0F == 0x0B or b[3] & 0x0F == 0x0B:
            continue
        n += sum(1 for x, y in zip(a, b) if x != y)
    return n


src = sys.argv[1]
obj = os.path.splitext(src)[0] + "_fs.o"
cc = casediff.CC
best = None
for name, extra in VARIANTS:
    flags = BASE + extra
    r = subprocess.run(casediff.buildcfg.tool_command(cc) + flags + ["-c", src, "-o", obj], capture_output=True, text=True,
                       cwd=casediff.REPO)
    if r.returncode:
        print("%-20s COMPILE FAIL" % name)
        continue
    from elftools.elf.elffile import ELFFile
    with open(obj, "rb") as fh:
        secs = [s.data() for s in ELFFile(fh).iter_sections() if s.name.startswith(".text")]
    text = max(secs, key=len)
    nb = masked_bytes(text)
    print("%-20s bytes=%-6d len=0x%x" % (name, nb, len(text)))
    if best is None or nb < best[0]:
        best = (nb, name)
print("BEST", best[1], best[0])
