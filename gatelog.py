"""Per-address gate history: what every gate said, and whether this session has stopped improving.

    python gatelog.py <main|NNN> <addr> <session>     -> "STALL <phase> <n>" or "OK <n>"
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import os
import sys
import threading
import time

SP = _kp.SP
KIT = _kp.KIT
NEAR_BYTES = int(os.environ.get("STALL_NEAR_BYTES", "32"))
NEAR_LIMIT = int(os.environ.get("STALL_NEAR_LIMIT", "12"))
LIMITS = {"compile": int(os.environ.get("STALL_NOCOMPILE", "6")),
          "size": int(os.environ.get("STALL_SIZE", "6")),
          "diff": int(os.environ.get("STALL_DIFF", "4"))}


def path(mod, addr):
    return f"{SP}/wlog/gates/{mod}_{addr.lower()}.tsv"


def phase(cls):
    if cls in ("NO-COMPILE", "COMPILE-FAIL", "UNDEF-SYM"):
        return "compile"
    if cls in ("OVERGEN", "UNDERGEN"):
        return "size"
    return "diff"


def read(mod, addr):
    try:
        return [l.rstrip("\n").split("\t")
                for l in open(path(mod, addr), encoding="utf-8", errors="ignore")
                if l.count("\t") >= 3]
    except IOError:
        return []


_APPEND = threading.Lock()


def record(mod, addr, sess, cls, metric, src):
    d = f"{SP}/wlog/gates"
    os.makedirs(d, exist_ok=True)
    with _APPEND, open(path(mod, addr), "a", encoding="utf-8") as fh:
        fh.write("\t".join([str(int(time.time())), sess, cls, str(metric),
                            os.path.basename(str(src))]) + "\n")


def state(rows, sess, cls):
    """-> (gates_this_session, gates_all_time, phase, best_metric, gates_since_best, limit)"""
    mine = [r for r in rows if len(r) > 1 and r[1] == sess]
    ph = phase(cls)
    run = []
    for r in reversed(mine):
        if phase(r[2]) != ph:
            break
        run.append(r)
    run.reverse()
    vals = [int(r[3]) for r in run if r[3].lstrip("-").isdigit()]
    if ph == "compile":
        return len(mine), len(rows), ph, -1, len(run), LIMITS[ph]
    good = [v for v in vals if v >= 0]
    best = min(good) if good else -1
    last = min((i for i, v in enumerate(vals) if v == best), default=-1)
    since = (len(vals) - 1 - last) if last >= 0 else len(vals)
    # A NEAR MISS EARNS MORE ATTEMPTS. The limit counts gates since the best improved, so a session
    # handed an artifact that already gates well sets its best on gate one and stalls on the fourth
    # variant that fails to beat it: 020227dc, fourteen bytes from matching, stopped after five
    # gates and $1.14. Distance from a match, not gate count, is what says whether to keep paying.
    limit = LIMITS[ph]
    if ph == "diff" and 0 <= best <= NEAR_BYTES:
        limit = max(limit, NEAR_LIMIT)
    return len(mine), len(rows), ph, best, since, limit


def banner(rows, sess, addr, cls, metric):
    if sess == "-":
        return []
    n, allt, ph, best, since, limit = state(rows, sess, cls)
    out = ["GATE %d this session, %d all-time on %s. best %s %s, %d gate(s) since it improved"
           % (n, allt, addr, ph, best if best >= 0 else "n/a", since)]
    if since >= limit:
        out += ["STOP: %d gate(s) with no improvement in the %s phase. Emit exactly:" % (since, ph),
                "  BLOCKED %s %s %s <path to your best file>" % (addr, cls, metric),
                "Do not start another variant. End the session with that line."]
    return out


def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    mod, addr, sess = sys.argv[1], sys.argv[2], sys.argv[3]
    rows = read(mod, addr)
    mine = [r for r in rows if len(r) > 1 and r[1] == sess]
    if not mine:
        print("OK 0")
        return 0
    _n, _a, ph, _b, since, limit = state(rows, sess, mine[-1][2])
    print(("STALL %s %d" if since >= limit else "OK %s %d") % (ph, since))
    return 0


if __name__ == "__main__":
    sys.exit(main())
