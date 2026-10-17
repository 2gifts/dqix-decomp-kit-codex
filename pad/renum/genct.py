import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__)))))
import kitpaths as _kp
import os
import sys
import threading

import frida

sys.path.insert(0, (_kp.KIT + "/frida"))
import schedforce as sf

JS = r"""
const LAST = ptr('0x63a224');
Interceptor.attach(ptr('0x4ffe20'), {
  onEnter(a) { const b = LAST.readPointer(); this.b = b; this.c = b.add(0x28).readS16(); this.s = a[0]; this.raw = a[0].toInt32(); },
  onLeave() { const b = LAST.readPointer(); send([this.c, b.equals(this.b) ? 0 : 1, this.raw]); }
});
"""


def trace(src):
    ev = []
    done = threading.Event()
    dev = frida.get_local_device()
    pid, ses = sf.buildcfg.frida_spawn(dev, sf.compile_argv(src, src[:-4] + ".gc.o"), sf.REPO)
    sc = ses.create_script(sf.buildcfg.frida_script(JS))
    sc.on("message", lambda m, d: ev.append(m["payload"]) if m["type"] == "send" else print(m))
    sc.load()
    ses.on("detached", lambda *a: done.set())
    dev.resume(pid)
    done.wait(300)
    return ev


for src in sys.argv[1:]:
    src = os.path.abspath(src).replace(os.sep, "/")
    ev = trace(src)
    line = []
    for c, split, kind in ev:
        line.append("%d:%d%s" % (kind, c, "|" if split else ""))
    print(os.path.basename(src), " ".join(line))
