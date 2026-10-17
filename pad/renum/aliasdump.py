import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__)))))
import kitpaths as _kp
import json
import os
import sys
import threading

import frida

SP = _kp.SP
KIT = _kp.KIT
REPO = _kp.REPO
sys.path.insert(0, KIT)
import buildcfg  # noqa: E402

JS = r"""
const CFG = %s;
let active = false, tpass = 0, blk = 0, pos = null, ctx = null;
function cstr(p) { try { const s = p.readCString(); return /^[\x20-\x7e]{1,80}$/.test(s) ? s : null; } catch (e) { return null; } }
function hex(p, n) { try { return Array.from(new Uint8Array(p.readByteArray(n))).map(x => ('0' + x.toString(16)).slice(-2)).join(''); } catch (e) { return null; } }
function objinfo(o) {
  if (o.isNull()) return null;
  const r = {p: o.toString(), raw: hex(o, 0x40), strs: {}};
  for (let k = 0; k < 0x40; k += 4) {
    try {
      const q = o.add(k).readPointer();
      const s = cstr(q) || cstr(q.add(0xa)) || cstr(q.add(8));
      if (s) r.strs[k.toString(16)] = s;
    } catch (e) {}
  }
  return r;
}
function alias(a) {
  if (a.isNull()) return null;
  return {p: a.toString(), kind: a.add(0x30).readU8(), off: a.add(0x18).readS32(), size: a.add(0x1c).readS32(),
          idx: a.add(0x2c).readS32(), vec: a.add(0x28).readPointer().toString(), raw: hex(a, 0x34),
          obj: objinfo(a.add(0x14).readPointer())};
}
Interceptor.attach(ptr('0x4ffa30'), {
  onEnter() {
    let name = null;
    try { name = this.context.ebp.readCString(); } catch (e) {}
    active = name === CFG.want;
    if (active) { tpass++; blk = 0; }
  },
  onLeave() { active = false; }
});
Interceptor.attach(ptr('0x4ff8c0'), {
  onEnter(a) {
    if (!active) return;
    blk++;
    pos = new Map();
    let i = 0;
    const mem = [];
    for (let pc = a[0].add(0x14).readPointer(); !pc.isNull(); pc = pc.readPointer()) {
      if (CFG.blocks && (!CFG.blk || CFG.blk === blk) && (pc.add(0xc).readU32() & 0x60006)) {
        const al = pc.add(0x1c).readPointer();
        mem.push([i, pc.add(0x28).readU16(), al.isNull() ? null : (al.equals(ptr(0x63bb8c).readPointer()) ? 'WORST' : alias(al))]);
      }
      pos.set(pc.toString(), i++);
    }
    if (mem.length) send({pass: tpass, blk: blk, mem: mem});
  },
  onLeave() { pos = null; }
});
for (const [addr, nm] of [['0x4ff250', 'load'], ['0x4ff2b0', 'store']]) {
  Interceptor.attach(ptr(addr), { onEnter(a) { ctx = nm; }, onLeave() { ctx = null; } });
}
let inner = null;
Interceptor.attach(ptr('0x559c40'), {
  onEnter(a) { this.a = a[0]; this.b = a[1]; },
  onLeave(r) { if (inner) inner.al = [alias(this.a), alias(this.b)]; }
});
Interceptor.attach(ptr('0x559dc0'), {
  onEnter(a) { inner = {pa: a[0], pb: a[1]}; },
  onLeave(r) {
    const it = inner; inner = null;
    if (!active || pos === null || ctx === null) return;
    if (CFG.pass && CFG.pass !== tpass) return;
    if (CFG.blk && CFG.blk !== blk) return;
    const ia = pos.get(it.pa.toString()), ib = pos.get(it.pb.toString());
    if (CFG.pos && !(CFG.pos.includes(ia) && CFG.pos.includes(ib))) return;
    send({pass: tpass, blk: blk, ctx: ctx, a: ia, b: ib, opa: it.pa.add(0x28).readU16(), opb: it.pb.add(0x28).readU16(),
          r: r.toInt32() & 0xff, al: it.al});
  }
});
send({worst: cstr(ptr('0x5ef31c')), worsthex: hex(ptr('0x5ef31c'), 16)});
"""


def run(src, cfg):
    obj = os.path.join(os.environ.get("TEMP", "."), "aliasdump_%d.o" % os.getpid())
    argv = buildcfg.tool_command(os.path.normpath(buildcfg.cc_path(None))) + list(buildcfg.FLAGS) + ["-c", src, "-o", obj]
    events = []
    done = threading.Event()
    device = frida.get_local_device()
    pid, session = buildcfg.frida_spawn(device, argv, REPO)
    script = session.create_script(buildcfg.frida_script(JS % json.dumps(cfg)))
    script.on("message", lambda m, d: events.append(m["payload"]) if m["type"] == "send" else print(m))
    script.load()
    session.on("detached", lambda *a: done.set())
    device.resume(pid)
    done.wait(600)
    return events


if __name__ == "__main__":
    src, want = os.path.abspath(sys.argv[1]), sys.argv[2]
    cfg = {"want": want}
    if len(sys.argv) > 3:
        p, b = sys.argv[3].split(":")[:2]
        cfg["pass"], cfg["blk"] = int(p), int(b)
    if len(sys.argv) > 4:
        cfg["pos"] = [int(x) for x in sys.argv[4].split(",")]
    if os.environ.get("AD_BLOCKS"):
        cfg["blocks"] = True
    for e in run(src, cfg):
        print(json.dumps(e))
