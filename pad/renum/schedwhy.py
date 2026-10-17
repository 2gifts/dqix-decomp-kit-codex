import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__)))))
import kitpaths as _kp
import json
import os
import subprocess
import sys
import threading

import frida

sys.path.insert(0, (_kp.KIT + "/frida"))
import schedforce as sf  # noqa: E402

JS = r"""
const CFG = %s;
const BLOCKS = ptr('0x63a828');
const VREGS = ptr('0x63a364');
const scoreFn = new NativeFunction(ptr('0x4feba0'), 'int', ['pointer'], 'mscdecl');
let active = false, tpass = 0, blk = 0, npick = 0, pos = null, picks = [];
function pcOf(n) { return n.add(0xc).readPointer(); }
function uniq(n) { let c = 0; for (let e = n.add(8).readPointer(); !e.isNull(); e = e.readPointer()) if (e.add(4).readPointer().add(0x1a).readS16() === 1) c++; return c; }
function succs(n) { const r = []; for (let e = n.add(8).readPointer(); !e.isNull(); e = e.readPointer()) r.push([pos.get(pcOf(e.add(4).readPointer()).toString()), e.add(8).readU16()]); return r; }
function ops(pc) {
  const n = pc.add(0x2c).readS16(), r = [];
  for (let i = 0; i < n && i < 8; i++) { const o = pc.add(0x2e + i * 14); r.push([o.readU8(), o.add(1).readS8(), o.add(2).readU16(), o.add(4).readS16(), o.add(6).readS32()]); }
  return r;
}
Interceptor.attach(ptr('0x4ffa30'), {
  onEnter() { let name = null; try { name = this.context.ebp.readCString(); } catch (e) {} active = name === CFG.want; if (active) { tpass++; blk = 0; } },
  onLeave() {
    if (active) {
      const layout = [];
      for (let b = BLOCKS.readPointer(); !b.isNull(); b = b.readPointer()) {
        const pcs = [];
        for (let pc = b.add(0x14).readPointer(); !pc.isNull(); pc = pc.readPointer()) pcs.push(pc.toString());
        layout.push(pcs);
      }
      send({ev: 'pass', pass: tpass, picks: picks, layout: layout});
      picks = [];
    }
    active = false;
  }
});
Interceptor.attach(ptr('0x4ff8c0'), {
  onEnter(a) {
    if (!active) return;
    blk++;
    pos = new Map();
    let i = 0;
    for (let pc = a[0].add(0x14).readPointer(); !pc.isNull(); pc = pc.readPointer()) pos.set(pc.toString(), i++);
  },
  onLeave() { pos = null; }
});
Interceptor.attach(ptr('0x4ff700'), {
  onEnter(a) {
    if (!active || pos === null) { this.skip = true; return; }
    this.cycle = a[1].toInt32() & 0xffff;
    const c = [];
    for (let n = a[0]; !n.isNull(); n = n.readPointer())
      c.push({pc: pcOf(n).toString(), ir: pos.get(pcOf(n).toString()), op: pcOf(n).add(0x28).readU16(), lat: n.add(0x10).readU16(), x12: n.add(0x12).readU16(),
              ready: n.add(0x14).readU16(), dl: n.add(0x16).readU16(), h: n.add(0x18).readU16(), preds: n.add(0x1a).readS16(),
              uniq: uniq(n), succ: succs(n), ops: ops(pcOf(n)), score: VREGS.readU32() ? scoreFn(pcOf(n)) : null});
    this.c = c;
    this.lim = ptr('0x61667c').readS32(); this.live = ptr('0x616680').readS32();
  },
  onLeave(r) {
    if (this.skip) return;
    if (r.isNull()) return;
    picks.push({n: npick++, pass: tpass, blk: blk, cycle: this.cycle, pressure: VREGS.readU32(), lim: this.lim, live: this.live, pick: pcOf(r).toString(), cands: this.c});
  }
});
"""


def run(src, want, obj):
    events, done = [], threading.Event()
    dev = frida.get_local_device()
    pid, ses = sf.buildcfg.frida_spawn(dev, sf.compile_argv(src, obj), sf.REPO)
    scr = ses.create_script(sf.buildcfg.frida_script(JS % json.dumps({"want": want})))
    scr.on("message", lambda m, d: events.append(m["payload"]) if m["type"] == "send" else print(m))
    scr.load()
    ses.on("detached", lambda *a: done.set())
    dev.resume(pid)
    done.wait(600)
    return [e for e in events if e.get("ev") == "pass"]


def main():
    src = os.path.abspath(sys.argv[1]).replace("\\", "/")
    addr = int(sys.argv[3], 16)
    want = sys.argv[4:]
    out = os.path.splitext(src)[0] + ".why"
    os.makedirs(out, exist_ok=True)
    subprocess.run(sf.compile_argv(src, out + "/n.o"), cwd=sf.REPO, check=True)
    fn = sf.Func(out + "/n.o", addr=addr)
    passes = run(src, fn.name, out + "/r.o")
    json.dump(passes, open(out + "/why.json", "w"))
    words, codes = fn.words(), fn.code_offsets()
    final = [pc for pcs in passes[-1]["layout"] for pc in pcs]
    at = {pc: codes[i] for i, pc in enumerate(final)} if len(final) == len(codes) else {}

    def name(pc):
        o = at.get(pc)
        return "%3x %-28s" % (o, sf.dis(words[o // 4], o)) if o is not None else "%-32s" % pc

    for p in passes:
        for pk in p["picks"]:
            if str(pk["n"]) not in want and "p%db%d" % (pk["pass"], pk["blk"]) not in want:
                continue
            mode = "PRESSURE" if pk["pressure"] and pk["lim"] <= pk["live"] else "normal"
            print("pick %d pass %d blk %d cycle %d prera %d live %d/%d %s" % (
                pk["n"], pk["pass"], pk["blk"], pk["cycle"], pk["pressure"], pk["live"], pk["lim"], mode))
            for c in pk["cands"]:
                ok = c["preds"] == 0 and c["ready"] <= pk["cycle"]
                print(" %s%s ir%-3d op%-4d lat%d x12=%d rdy%d dl%d h%d preds%d uniq%d score%s succ%s %s" % (
                    "*" if c["pc"] == pk["pick"] else " ", "R" if ok else " ", c["ir"], c["op"], c["lat"], c["x12"], c["ready"],
                    c["dl"], c["h"], c["preds"], c["uniq"], c["score"], c["succ"], name(c["pc"]) if ok else ""))
                if ok and os.environ.get("OPS"):
                    print("       ops", c["ops"])


if __name__ == "__main__":
    main()
