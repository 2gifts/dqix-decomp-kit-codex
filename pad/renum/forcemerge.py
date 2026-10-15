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
const BLOCKS = ptr('0x63a828');
const MAP = %s;
const AT = ptr('%s');
const NTH = %d;
let calls = 0;
Interceptor.attach(AT, { onEnter() {
  calls++;
  if (calls !== NTH) return;
  let n = 0;
  let b = BLOCKS.readPointer();
  while (!b.isNull()) {
    let pc = b.add(0x14).readPointer();
    while (!pc.isNull()) {
      const k = pc.add(0x2c).readS16();
      for (let i = 0; i < k; i++) {
        const o = pc.add(0x2e + i * 14);
        if (o.readU8() === 0) {
          const r = o.add(4).readS16();
          if (MAP[r] !== undefined) { o.add(4).writeS16(MAP[r]); n++; }
        }
      }
      pc = pc.readPointer();
    }
    b = b.readPointer();
  }
  send({rewrote: n});
} });
"""


def run(src, mapping, obj, at="0x50b790", nth=1):
    argv = sf.compile_argv(src, obj)
    done = threading.Event()
    msgs = []
    device = frida.get_local_device()
    pid, session = sf.buildcfg.frida_spawn(device, argv, sf.REPO)
    script = session.create_script(sf.buildcfg.frida_script(JS % (mapping, at, nth)))
    script.on("message", lambda m, d: msgs.append(m.get("payload", m)))
    script.load()
    session.on("detached", lambda *a: done.set())
    device.resume(pid)
    done.wait(300)
    return msgs


def main():
    src = os.path.abspath(sys.argv[1]).replace("\\", "/")
    mod, addr, size = sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4], 16)
    pool_from = int(sys.argv[5], 16)
    mapping = "{%s}" % ",".join("%d:%d" % tuple(map(int, p.split(":"))) for p in sys.argv[6].split(","))
    at = os.environ.get("FM_AT", "0x50b790")
    nth = int(os.environ.get("FM_NTH", "1"))
    obj = os.path.splitext(src)[0] + ".fm.o"
    print(run(src, mapping, obj, at, nth))
    fn = sf.Func(obj, addr=addr)
    R = sf.rom(mod, addr, size)
    real = sf.score(fn, R, size, pool_from)
    print("size 0x%x diff words %s" % (fn.size, None if real is None else ["0x%x" % x for x in real]))


if __name__ == "__main__":
    main()
