import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import os as _bcos, sys as _bcsys
_bcsys.path.insert(0, _bcos.path.dirname(_bcos.path.dirname(_bcos.path.abspath(__file__))))
import buildcfg
"""Compile one scratch .cpp with the project compiler and disassemble its .text.

Iterating a construct against the 1500-line c04 costs a minute a try; a probe costs a second.

Usage: python probe_cc.py <src.cpp> [--bytes]
"""
import os
import subprocess
import sys

import capstone
from elftools.elf.elffile import ELFFile

REPO = _kp.REPO
CC = buildcfg.cc_path(os.environ.get("MWCC"))
FLAGS = list(buildcfg.FLAGS)

src = sys.argv[1]
obj = os.path.splitext(src)[0] + ".o"
FLAGS = FLAGS + os.environ.get("PROBE_FLAGS", "").split()
r = subprocess.run(buildcfg.tool_command(os.environ.get("PROBE_CC", CC)) + FLAGS + ["-c", src, "-o", obj],
                   capture_output=True, text=True, cwd=REPO)
if r.returncode:
    print(r.stdout[-3000:] + r.stderr[-3000:])
    sys.exit(1)
want = [a for a in sys.argv[2:] if not a.startswith("--")]
with open(obj, "rb") as fh:
    elf = ELFFile(fh)
    md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM)
    syms = {}
    for sec in elf.iter_sections():
        if sec.header["sh_type"] != "SHT_SYMTAB":
            continue
        for s in sec.iter_symbols():
            # mwcc emits ARM/Thumb mapping symbols ($a, $t) into the same section, and they sort
            # first -- a setdefault on them labels every probe function "$a".
            if s["st_info"]["type"] == "STT_FUNC" and not s.name.startswith("$"):
                syms.setdefault(s["st_shndx"], s.name)
    for i, sec in enumerate(elf.iter_sections()):
        if not sec.name.startswith(".text") or not sec.header.sh_size:
            continue
        name = syms.get(i, sec.name)
        data = sec.data()
        print("%s: 0x%x (%d) bytes" % (name, len(data), len(data)))
        if "--bytes" in sys.argv and (not want or any(w in name for w in want)):
            for ins in md.disasm(data, 0):
                print("  +0x%04x  %s %s" % (ins.address, ins.mnemonic, ins.op_str))
