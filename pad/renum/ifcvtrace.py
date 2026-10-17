import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__)))))
import kitpaths as _kp
import os
import sys
import threading

import frida

sys.path.insert(0, (_kp.KIT + "/frida"))
import schedforce as sf  # noqa: E402

JS = r"""
function ops(b) {
  const out = [];
  let pc = b.add(0x14).readPointer(), g = 0;
  while (!pc.isNull() && g++ < 200) { out.push(pc.add(0x28).readU16().toString(16) + '/' + pc.add(0x2a).readU8()); pc = pc.readPointer(); }
  return out;
}
let fn = 0;
Interceptor.attach(ptr('0x5031e0'), { onEnter() { fn++; send({ev: 'cond', fn: fn}); } });
Interceptor.attach(ptr('0x5025f0'), {
  onEnter(a) { this.b = a[0]; this.f = a[1].toInt32() & 0xff; },
  onLeave(r) { send({ev: 'arm', fn: fn, blk: this.b.add(0x1c).readS32(), n: this.b.add(0x28).readS16(), flag: this.f,
                     ok: r.toInt32() & 0xff, ret: this.returnAddress.toString(), ops: ops(this.b)}); }
});
Interceptor.attach(ptr('0x502400'), { onLeave(r) { send({ev: 'limit', v: r.toInt32(), ret: this.returnAddress.toString()}); } });
"""


def run(src, flags):
    argv = sf.compile_argv(src, os.path.splitext(src)[0] + ".ifc.o")
    if flags:
        prefix = len(sf.buildcfg.tool_command(sf.buildcfg.cc_path(None)))
        argv = argv[:prefix] + flags + [a for a in argv[prefix:] if not a.startswith("-O")]
    ev = []
    done = threading.Event()
    dev = frida.get_local_device()
    pid, ses = sf.buildcfg.frida_spawn(dev, argv, sf.REPO)
    sc = ses.create_script(sf.buildcfg.frida_script(JS))
    sc.on("message", lambda m, d: ev.append(m["payload"]) if m["type"] == "send" else print(m))
    sc.load()
    ses.on("detached", lambda *a: done.set())
    dev.resume(pid)
    done.wait(300)
    return ev


if __name__ == "__main__":
    src = os.path.abspath(sys.argv[1]).replace("\\", "/")
    for e in run(src, sys.argv[2].split() if len(sys.argv) > 2 else None):
        print(e)
