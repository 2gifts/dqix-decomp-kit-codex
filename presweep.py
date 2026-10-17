#!/usr/bin/env python
"""Run the free rewrites on an address BEFORE a paid worker sees it.

    python presweep.py <main|NNN> <addr>     -> prints MATCH <file>, IMPROVED <a> <b>, or nothing

colorsweep only ever ran inside repairsweep, on an hourly timer over a 475-candidate pool walked
sequentially, so the chance it had recently touched the address a worker was about to claim was
small -- and the worker prompt then asked the WORKER to run it, paying tokens for a mechanical
rewrite that costs CPU. Every rule in colorsweep is meaning-preserving and gated, so there is no
reason a session should ever be the first thing to try them.

Two outcomes worth having:
  * MATCH    -- the address is closed for free and no worker is spawned at all.
  * IMPROVED -- clsbest is replaced, so the doc's "START HERE" artifact is the swept one and the
                session begins past everything the rules can reach.

Nothing happens on an address with no artifact yet: there is nothing to rewrite until a first
source exists, which is what a worker is for.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import concurrent.futures
import glob
import hashlib
import os
import re
import shutil
import subprocess
import sys

SP = _kp.SP
KIT = _kp.KIT
REPO = _kp.REPO
BEST = f"{SP}/clsbest"
DEPTH = os.environ.get("PRESWEEP_DEPTH", "3")
BUDGET = os.environ.get("PRESWEEP_BUDGET", "250")
TIMEOUT = int(os.environ.get("PRESWEEP_TIMEOUT", "1800"))
GATE_JOBS = int(os.environ.get("PRESWEEP_GATE_JOBS", "6"))


def rank(verdict):
    if verdict.startswith("MATCH"):
        return (0, 0)
    m = re.search(r"(\d+) bytes differ", verdict)
    if m:
        return (1, int(m.group(1)))
    m = re.search(r"total=0x([0-9a-fA-F]+)\s+slot=0x([0-9a-fA-F]+)", verdict)
    if m:
        return (2, abs(int(m.group(1), 16) - int(m.group(2), 16)))
    return (3, 1 << 30)


def gate(mod, addr, path):
    r = subprocess.run([sys.executable, f"{KIT}/wgate.py", mod, addr, path],
                       capture_output=True, text=True, cwd=REPO)
    return (r.stdout + r.stderr).strip()


def stamp(path):
    st = os.stat(path)
    return "%d %d" % (st.st_size, int(st.st_mtime))


def main():
    if len(sys.argv) < 3:
        return 2
    mod, addr = sys.argv[1], sys.argv[2].lower()
    args = [a for a in sys.argv[3:] if not a.startswith("--")]
    if args:
        cand = [args[0]]
    else:
        cand = [f"{BEST}/{addr}.cpp"] + sorted(glob.glob(f"{SP}/attempts/*{addr}*.cpp"))
    cand = [p for p in cand if os.path.exists(p)]

    # COLD START: MAKE an artifact so the rules have something to work on. A fresh address has no
    # source, so every rewrite rule is inapplicable and the whole function goes to a paid session.
    # synth.py writes a compilable body with every callee and data name already resolved; it used to
    # delete that unless it matched outright, which threw away the one thing automation needs. Now
    # a near miss is kept and swept, so the chain synth -> colorsweep can close a cold function for
    # the price of a few compiles.
    if not cand:
        subprocess.run([sys.executable, f"{KIT}/synth.py", mod, addr],
                       capture_output=True, text=True, cwd=REPO)
        seeded = f"{SP}/lab/synth_{addr}.cpp"
        if os.path.exists(seeded):
            cand = [seeded]
    # SECOND COLD-START SEED: the transliterator. synth only reaches accessor-shaped functions, so
    # every large function fell through to a paid session with nothing tried for free -- 021941fc
    # cost $22.45 to produce its FIRST source. translate gates its own draft, so a MATCH here closes
    # the address for the price of one compile.
    if not cand:
        tr = subprocess.run([sys.executable, f"{KIT}/translate.py", mod, addr],
                            capture_output=True, text=True, cwd=REPO, timeout=1800)
        draft = f"{SP}/lab/trans_{addr}.cpp"
        if (tr.stdout or "").strip().startswith("MATCH") and os.path.exists(draft):
            print("MATCH " + draft)
            return 0
        # A draft that does NOT match is deliberately NOT swept and NOT promoted to clsbest. Measured
        # over the massive band: right size, ~75% different bytes, so the hill-climb has no hill to
        # climb and "START HERE" would point a session at the wrong basin. recipe_select picks it up
        # from lab/ as SEMANTICS instead, which is what it is good for.
        return 0
    if not cand:
        return 0

    # SWEEP EVERY DISTINCT ARTIFACT, NOT JUST THE BEST ONE. colorsweep is a hill-climb, so the
    # starting point decides which local optimum it can reach: 02079cf8 has eight artifacts, several
    # sitting at the same 3 bytes with DIFFERENT levers already applied, and they are different
    # basins. Taking scored[0] explored one of them. repairsweep has always walked every distinct
    # file per address (it dedupes on (addr, content hash)); this is the same rule.
    seen, uniq = set(), []
    for p in cand[:12]:
        try:
            h = hashlib.md5(open(p, "rb").read()).hexdigest()
        except OSError:
            continue
        if h in seen:
            continue
        seen.add(h)
        uniq.append(p)
    with concurrent.futures.ThreadPoolExecutor(max(1, min(GATE_JOBS, len(uniq)))) as pool:
        scored = [(rank(v), p) for v, p in zip(pool.map(lambda p: gate(mod, addr, p), uniq), uniq)]
    if not scored:
        return 0
    scored.sort()
    entry, src = scored[0]
    if entry[0] >= 3:
        return 0

    # SKIP WHAT HAS ALREADY BEEN SWEPT. The rules are deterministic, so re-running them on an
    # unchanged artifact cannot find anything new -- it only makes a worker wait. The marker records
    # the artifact this address was last swept AT, so a claim costs nothing when the advance sweeper
    # has already been here and a full sweep when it has not.
    # The marker covers EVERY candidate, not just the best one: stamping only the winner meant a
    # newly written artifact that did not happen to be the best still looked "already swept".
    mark = f"{BEST}/{addr}.swept"
    cur = hashlib.md5("|".join(sorted(stamp(p) + p for _r, p in scored))
                      .encode()).hexdigest()
    if "--force" not in sys.argv:
        try:
            if open(mark, encoding="utf-8").read().strip() == cur:
                return 0
        except OSError:
            pass

    # A HILL-CLIMB FROM A BAD SOURCE IS WASTED CPU. Every rule is a local rewrite, so it cannot fix
    # a wrong structure: measured today, 400 compiles on a 411-byte residue and 400 on a 281-byte
    # one both returned "(no change)". Sweep what is already close; leave the rest to a worker.
    _cap = int(os.environ.get("PRESWEEP_MAX_RESIDUE", "160"))
    if entry[1] > _cap:
        return 0

    os.makedirs(f"{SP}/handwork", exist_ok=True)
    # ONE CANDIDATE UNLESS ASKED FOR MORE. The multi-candidate sweep belongs to the ADVANCE pass,
    # which nobody waits on; at claim time it just blocks a slot. Measured 2026-09-08: a three
    # candidate sweep on main:02079cf8 ate the whole 240s claim budget, was killed by the timeout,
    # and the partial work was discarded -- the worker waited five minutes for nothing.
    # presweep_watch.sh passes --deep; pull_worker.sh does not.
    ncand = int(os.environ.get("PRESWEEP_CANDS", "3")) if "--deep" in sys.argv else 1
    best, work = entry, None
    for k, (r0, p) in enumerate(scored[:ncand]):
        if r0[0] >= 3 or r0[1] > _cap:
            continue
        w = f"{SP}/handwork/presweep_{addr}_{k}.cpp"
        shutil.copy(p, w)
        try:
            subprocess.run([sys.executable, f"{KIT}/colorsweep.py", mod, addr, w,
                            "--depth", DEPTH, "--budget", BUDGET, "--apply"],
                           capture_output=True, text=True, cwd=REPO, timeout=TIMEOUT)
        except subprocess.TimeoutExpired:
            pass
        r1 = rank(gate(mod, addr, w))
        if r1 < best:
            best, work = r1, w
        if r1 == (0, 0):
            break
    if work is None:
        try:
            with open(mark, "w", encoding="utf-8") as fh:
                fh.write(cur)
        except OSError:
            pass
        return 0

    now = best
    try:
        with open(mark, "w", encoding="utf-8") as fh:
            fh.write(cur)
    except OSError:
        pass
    if now >= entry:
        return 0

    if now == (0, 0):
        print(f"MATCH {work}")
        return 0

    text = open(work, encoding="utf-8", errors="ignore").read()
    keep = f"{BEST}/{addr}.cpp"
    os.makedirs(BEST, exist_ok=True)
    with open(keep + ".tmp_pre", "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text.replace("// USA: func_", "// SCRATCH-USA: func_"))
    os.replace(keep + ".tmp_pre", keep)
    with open(keep + ".rank.tmp_pre", "w", encoding="utf-8") as fh:
        fh.write("%d %d" % now)
    os.replace(keep + ".rank.tmp_pre", keep + ".rank")
    print(f"IMPROVED {entry[1]} {now[1]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
