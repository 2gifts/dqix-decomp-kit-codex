import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import json
import os
import sys
import threading

import frida

sys.path.insert(0, (_kp.KIT + "/frida"))
import forcenoalias  # noqa: E402

SP = _kp.SP
KIT = _kp.KIT
REPO = _kp.REPO
sys.path.insert(0, KIT)
import buildcfg  # noqa: E402

JS = r"""
const VREGS = ptr('0x63a364');
const LIMIT = ptr('0x61667c');
const LIVE = ptr('0x616680');
const USECNT = ptr('0x616678');
let pass = 0;
let curBlock = null;
function pcinfo(pc) {
  const n = pc.add(0x2c).readS16();
  const ops = [];
  for (let i = 0; i < n && i < 16; i++) {
    const o = pc.add(0x2e + i * 14);
    ops.push([o.readU8(), o.add(1).readS8(), o.add(2).readU16(), o.add(4).readS16(), o.add(6).readS32(), o.add(10).readU32()]);
  }
  const al = pc.add(0x1c).readPointer();
  let alias = null;
  if (!al.isNull()) {
    alias = {p: al.toString(), kind: al.add(0x30).readU8(), obj: al.add(0x14).readPointer().toString(), off: al.add(0x18).readS32(),
             size: al.add(0x1c).readS32(), idx: al.add(0x2c).readS32(), set: al.add(0x28).readPointer().toString()};
    try { const o = al.add(0x14).readPointer(); if (!o.isNull()) alias.name = o.add(0x0).readPointer().isNull() ? null : null; } catch (e) {}
  }
  return {pc: pc.toString(), alias: alias, op: pc.add(0x28).readU16(), cond: pc.add(0x2a).readU8(), b2b: pc.add(0x2b).readU8(),
          flags: pc.add(0xc).readU32(), f10: pc.add(0x10).readU32(), ops: ops};
}
Interceptor.attach(ptr('0x4ffa30'), { onEnter() { pass++; send({ev: 'pass', pass: pass, vregs: VREGS.readU32()}); } });
Interceptor.attach(ptr('0x4ff8c0'), {
  onEnter(args) {
    const b = args[0];
    curBlock = b;
    const list = [];
    let pc = b.add(0x14).readPointer();
    let g = 0;
    while (!pc.isNull() && g++ < 4000) { list.push(pcinfo(pc)); pc = pc.readPointer(); }
    send({ev: 'block', pass: pass, block: b.toString(), idx: b.add(0x1c).readS32(), vregs: VREGS.readU32(), insns: list});
  },
  onLeave() {
    const list = [];
    let pc = curBlock.add(0x14).readPointer();
    let g = 0;
    while (!pc.isNull() && g++ < 4000) { list.push(pc.toString()); pc = pc.readPointer(); }
    send({ev: 'blockdone', pass: pass, block: curBlock.toString(), order: list});
  }
});
Interceptor.attach(ptr('0x4feb70'), { onLeave(r) { this.r = r; send({ev: 'pressure', v: r.toInt32() & 0xff, limit: LIMIT.readS32(), live: LIVE.readS32()}); } });
Interceptor.attach(ptr('0x4ff700'), {
  onEnter(args) {
    this.cycle = args[1].toInt32() & 0xffff;
    const cands = [];
    let n = args[0];
    let g = 0;
    while (!n.isNull() && g++ < 4000) {
      cands.push({pc: n.add(0xc).readPointer().toString(), lat: n.add(0x10).readU16(), desc: n.add(0x12).readU16(),
                  ready: n.add(0x14).readU16(), dl: n.add(0x16).readU16(), h: n.add(0x18).readU16(), preds: n.add(0x1a).readS16()});
      n = n.readPointer();
    }
    this.cands = cands;
  },
  onLeave(r) {
    const pick = r.isNull() ? null : r.add(0xc).readPointer().toString();
    send({ev: 'pick', pass: pass, cycle: this.cycle, pick: pick, limit: LIMIT.readS32(), live: LIVE.readS32(), cands: this.cands});
  }
});
Interceptor.attach(ptr('0x4ff030'), {
  onEnter(a) {
    this.f = a[0]; this.t = a[1]; this.p3 = a[2].toInt32() & 0xff; this.p4 = a[3].toInt32() & 0xff;
    this.before = this.f.add(8).readPointer();
    this.cnt = 0; let e = this.before; while (!e.isNull()) { this.cnt++; e = e.readPointer(); }
    this.caller = this.returnAddress.toString();
  },
  onLeave() {
    let n = 0; let e = this.f.add(8).readPointer(); let lat = -1;
    while (!e.isNull()) { if (e.add(4).readPointer().equals(this.t)) lat = e.add(8).readU16(); n++; e = e.readPointer(); }
    send({ev: 'edge', pass: pass, from: this.f.add(0xc).readPointer().toString(), to: this.t.add(0xc).readPointer().toString(),
          lat: lat, isnew: n > this.cnt, p3: this.p3, p4: this.p4, caller: this.caller});
  }
});
Interceptor.attach(ptr('0x4feba0'), { onEnter(a) { this.pc = a[0]; }, onLeave(r) { send({ev: 'score', pc: this.pc.toString(), s: r.toInt32()}); } });
"""


def run(src, out):
    cc = os.path.normpath(buildcfg.cc_path(None))
    argv = buildcfg.tool_command(cc) + list(buildcfg.FLAGS) + ["-c", src, "-o", os.path.splitext(src)[0] + ".st.o"]
    events = []
    done = threading.Event()
    device = frida.get_local_device()
    pid = device.spawn(argv, cwd=REPO)
    session = device.attach(pid)
    extra = ""
    if os.environ.get("FORCE"):
        extra = "\n(function() {\n" + forcenoalias.JS.replace("'%s'", "'" + os.environ["FORCE"] + "'") + "\n})();\n"
    script = session.create_script(JS + extra)
    script.on("message", lambda m, d: events.append(m["payload"]) if m["type"] == "send" else print(m))
    script.load()
    session.on("detached", lambda *a: done.set())
    device.resume(pid)
    done.wait(300)
    with open(out, "w") as fh:
        for e in events:
            fh.write(json.dumps(e) + "\n")
    return events


if __name__ == "__main__":
    ev = run(sys.argv[1], sys.argv[2])
    print("events", len(ev), "passes", sum(1 for e in ev if e["ev"] == "pass"),
          "blocks", sum(1 for e in ev if e["ev"] == "block"),
          "pressure-true", sum(1 for e in ev if e["ev"] == "pressure" and e["v"]))
