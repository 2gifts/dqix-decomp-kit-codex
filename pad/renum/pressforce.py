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

USAGE = "pressforce.py <src> <mod> <addr> <size> [off:p1b77 ...] [lim:p1b77=+4 ...] [dump:p1b77 ...]"

JS = r"""
const CFG = %s;
const LIM = ptr('0x61667c'), LIVE = ptr('0x616680');
const LIVETAB = ptr('0x63ccb8'), NV = ptr('0x63d0a4');
const pressOrig = new NativeFunction(ptr('0x4feb70'), 'uint32', [], 'mscdecl');
let active = false, tpass = 0, blk = 0, key = '', curBlock = null;
function bits(p, n) { const r = []; for (let i = 0; i < n; i++) if ((p.add((i >> 5) * 4).readU32() >>> (i & 31)) & 1) r.push(i); return r; }
function insns(b) {
  const r = [];
  for (let pc = b.add(0x14).readPointer(); !pc.isNull(); pc = pc.readPointer()) {
    const n = pc.add(0x2c).readS16(), ops = [];
    for (let i = 0; i < n && i < 8; i++) { const o = pc.add(0x2e + i * 14); ops.push([o.readU8(), o.add(1).readS8(), o.add(2).readU16(), o.add(4).readS16(), o.add(6).readS32()]); }
    r.push({op: pc.add(0x28).readU16(), ops: ops});
  }
  return r;
}
Interceptor.attach(ptr('0x4ffa30'), {
  onEnter() { let name = null; try { name = this.context.ebp.readCString(); } catch (e) {} active = name === CFG.want; if (active) { tpass++; blk = 0; } },
  onLeave() { active = false; }
});
Interceptor.attach(ptr('0x4ff8c0'), {
  onEnter(a) {
    if (!active) return;
    blk++; key = 'p' + tpass + 'b' + blk; curBlock = a[0];
    if (CFG.dump.indexOf(key) >= 0) send({ev: 'insns', key: key, insns: insns(a[0])});
  },
  onLeave() { curBlock = null; }
});
Interceptor.attach(ptr('0x4fe8c0'), {
  onLeave() {
    if (!active || curBlock === null) return;
    if (CFG.lim[key] !== undefined) LIM.writeS32(LIM.readS32() + CFG.lim[key]);
    if (CFG.dump.indexOf(key) >= 0) {
      const rec = LIVETAB.readPointer().add(curBlock.add(0x1c).readS32() * 0x10), n = NV.readS32();
      send({ev: 'live', key: key, lim: LIM.readS32(), live: LIVE.readS32(), nv: n,
            livein: bits(rec.add(8).readPointer(), n), liveout: bits(rec.add(0xc).readPointer(), n)});
    }
  }
});
let cycle = 0;
Interceptor.attach(ptr('0x4ff700'), { onEnter(a) { cycle = a[1].toInt32() & 0xffff; } });
function offNow() {
  for (const o of CFG.off) {
    const [b, r] = o.split('@');
    if (b !== key) continue;
    if (r === undefined) return true;
    const [lo, hi] = r.split('-').map(Number);
    if (cycle >= lo && cycle <= hi) return true;
  }
  return false;
}
Interceptor.replace(ptr('0x4feb70'), new NativeCallback(function () {
  if (active && curBlock !== null && offNow()) return 0;
  return pressOrig();
}, 'uint32', [], 'mscdecl'));
"""


def run(src, cfg, obj):
    events, done = [], threading.Event()
    dev = frida.get_local_device()
    pid, ses = sf.buildcfg.frida_spawn(dev, sf.compile_argv(src, obj), sf.REPO)
    scr = ses.create_script(sf.buildcfg.frida_script(JS % json.dumps(cfg)))
    scr.on("message", lambda m, d: events.append(m["payload"]) if m["type"] == "send" else print(m))
    scr.load()
    ses.on("detached", lambda *a: done.set())
    dev.resume(pid)
    done.wait(600)
    return events


def main():
    if len(sys.argv) < 5:
        sys.exit(USAGE)
    src = os.path.abspath(sys.argv[1]).replace("\\", "/")
    mod, addr, size = sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4], 16)
    cfg = {"off": [], "lim": {}, "dump": []}
    for a in sys.argv[5:]:
        k, v = a.split(":", 1)
        if k == "lim":
            b, d = v.split("=")
            cfg["lim"][b] = int(d)
        else:
            cfg[k].append(v)
    out = os.path.splitext(src)[0] + ".pf"
    os.makedirs(out, exist_ok=True)
    subprocess.run(sf.compile_argv(src, out + "/n.o"), cwd=sf.REPO, check=True)
    base = sf.Func(out + "/n.o", addr=addr)
    cfg["want"] = base.name
    ev = run(src, cfg, out + "/f.o")
    for e in ev:
        if e["ev"] == "live":
            print("%s lim %d live-in %d (nv %d)\n  in  %s\n  out %s" % (e["key"], e["lim"], e["live"], e["nv"], e["livein"], e["liveout"]))
        elif e["ev"] == "insns":
            for i, ins in enumerate(e["insns"]):
                print("  ir%-3d op%-4d %s" % (i, ins["op"], " ".join(("=" if o[2] & 2 else "") + str(o[3]) if o[0] == 0 else "#%d" % o[4] for o in ins["ops"])))
    R = sf.rom(mod, addr, size)
    pool = min([a for a, b in base.data_ranges] + [size])
    for tag, obj in (("base", out + "/n.o"), ("forced", out + "/f.o")):
        fn = sf.Func(obj, name=base.name)
        d = sf.score(fn, R, size, pool)
        print(tag, "size-mismatch" if d is None else ("MATCH" if not d else "%d words differ %s" % (len(d), " ".join("%x" % o for o in d))))


if __name__ == "__main__":
    main()
