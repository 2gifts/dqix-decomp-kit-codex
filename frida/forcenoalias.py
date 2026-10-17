import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import os
import struct
import sys
import threading

import capstone
import frida
from elftools.elf.elffile import ELFFile

SP = _kp.SP
KIT = _kp.KIT
REPO = _kp.REPO
sys.path.insert(0, KIT)
import buildcfg  # noqa: E402

JS = r"""
const VREGS = ptr('0x63a364');
const MODE = '%s';
let inStoreStore = 0;
let forced = 0;
Interceptor.attach(ptr('0x4ff2b0'), {
  onEnter(a) { this.cur = a[0].add(0xc).readPointer(); inStoreStore = 1; },
  onLeave() { inStoreStore = 0; }
});
Interceptor.attach(ptr('0x559dc0'), {
  onEnter(a) { this.a = a[0]; this.b = a[1]; },
  onLeave(r) {
    if (!inStoreStore) return;
    const final = VREGS.readU32() === 0;
    const aop = this.a.add(0x28).readU16(), bop = this.b.add(0x28).readU16();
    if (aop !== 0xee || bop !== 0xee) return;
    const abase = this.a.add(0x40).readS16(), bbase = this.b.add(0x40).readS16();
    const nosp = abase !== 13 && bbase !== 13;
    let drop = false;
    if (MODE === 'pass2') drop = final;
    else if (MODE === 'pass1') drop = !final;
    else if (MODE === 'both') drop = true;
    else if (MODE === 'both_nosp') drop = nosp;
    else if (MODE === 'p2_nosp') drop = final && nosp;
    else if (MODE === 'p1nosp_p2all') drop = final || nosp;
    if (drop) { r.replace(ptr(0)); forced++; }
  }
});
"""


def run(src, mode, out_obj):
    cc = os.path.normpath(buildcfg.cc_path(None))
    argv = buildcfg.tool_command(cc) + list(buildcfg.FLAGS) + ["-c", src, "-o", out_obj]
    done = threading.Event()
    device = frida.get_local_device()
    pid, session = buildcfg.frida_spawn(device, argv, REPO)
    script = session.create_script(buildcfg.frida_script(JS % mode))
    script.on("message", lambda m, d: print(m))
    script.load()
    session.on("detached", lambda *a: done.set())
    device.resume(pid)
    done.wait(300)


if __name__ == "__main__":
    src, mode = sys.argv[1], sys.argv[2]
    obj = os.path.splitext(src)[0] + ".fna_%s.o" % mode
    run(src, mode, obj)
    with open(obj, "rb") as fh:
        e = ELFFile(fh)
        data = b"".join(s.data() for s in e.iter_sections() if s.name.startswith(".text") and s.data_size)
    md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM)
    L = ["%03x %s %s" % (i.address, i.mnemonic, i.op_str) for i in md.disasm(data, 0)]
    for n, s in enumerate(L):
        if "str" in s and ("0xeac]" in s) and n > 3:
            print(" ; ".join(x[4:] for x in L[n - 5:n + 2]))
