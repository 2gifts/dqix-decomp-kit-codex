#!/usr/bin/env python3
"""Move a batch between a cloud session and the local coordinator.

    python cloud/batch.py load cloud/batches/<id>.tsv     cloud: put the batch's rows in the ledger;
                                                          cloud/batches/<id>/<mod>_<addr>.cpp, when
                                                          present, seeds the worker's source
    python cloud/batch.py export <id>                     cloud: copy every MATCH and report into
                                                          cloud/results/<id>/ for a commit
    python cloud/batch.py import <id>                     local: copy cloud/results/<id>/ into wip
                                                          and reports, then run `team.py collect`

A batch file lists `module addr size` per line. Reservation, integration and pull requests stay
with the local coordinator, so a cloud session needs no GitHub login beyond pushing a branch here.
"""
import glob
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "codex"))
_argv, sys.argv = sys.argv, ["team.py"]
import team  # noqa: E402
sys.argv = _argv


def load(path):
    rows = team.load_ledger()
    have = {team.key(r): r for r in rows}
    seeds = os.path.splitext(path)[0]
    added = 0
    for line in open(path, encoding="utf-8"):
        parts = line.split()
        if len(parts) < 3 or line.startswith("#"):
            continue
        mod, addr, size = parts[0].lower().removeprefix("ov"), parts[1].lower(), parts[2]
        seed = os.path.join(seeds, f"{mod}_{addr}.cpp")
        if os.path.exists(seed) and not os.path.exists(team.wip(mod, addr)):
            os.makedirs(os.path.dirname(team.wip(mod, addr)), exist_ok=True)
            shutil.copy(seed, team.wip(mod, addr))
        r = have.get((mod, addr))
        if r:
            r.update(status="ACTIVE", note="")
        else:
            rows.append({"module": mod, "addr": addr, "size": size, "status": "ACTIVE", "desc": "", "note": ""})
        added += 1
    team.save_ledger(rows)
    print(f"{added} row(s) ACTIVE; next: python codex/team.py prep --n {added} --prefix c")


def export(batch):
    out = os.path.join(HERE, "results", batch)
    os.makedirs(out, exist_ok=True)
    rows = team.load_ledger()
    with open(os.path.join(out, "ledger.tsv"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write("\t".join((r["module"], r["addr"], r["size"], r["status"], r["note"])) + "\n")
            src = team.wip(r["module"], r["addr"])
            if os.path.exists(src) and r["status"] in ("MATCH", "RESIDUE"):
                shutil.copy(src, os.path.join(out, f"{r['module']}_{r['addr']}.cpp"))
    for p in glob.glob(os.path.join(team.REPORTS, "*.json")):
        shutil.copy(p, os.path.join(out, "report_" + os.path.basename(p)))
    print(f"exported to {out}; commit it and push the branch")


def import_(batch):
    src = os.path.join(HERE, "results", batch)
    keys = []
    for line in open(os.path.join(src, "ledger.tsv"), encoding="utf-8"):
        mod, addr, _size, status, _note = (line.rstrip("\n").split("\t") + [""] * 5)[:5]
        f = os.path.join(src, f"{mod}_{addr}.cpp")
        if os.path.exists(f):
            os.makedirs(os.path.dirname(team.wip(mod, addr)), exist_ok=True)
            shutil.copy(f, team.wip(mod, addr))
            keys.append(addr)
    os.makedirs(team.REPORTS, exist_ok=True)
    for p in glob.glob(os.path.join(src, "report_*.json")):
        shutil.copy(p, os.path.join(team.REPORTS, "cloud_" + batch + "_" + os.path.basename(p)[7:]))
    print(f"imported {len(keys)} source(s); re-gating locally")
    if keys:
        team.collect([k for k in (team.key(r) for r in team.load_ledger()) if k[1] in keys])


if __name__ == "__main__":
    if len(sys.argv) != 3 or sys.argv[1] not in ("load", "export", "import"):
        sys.exit(__doc__)
    {"load": load, "export": export, "import": import_}[sys.argv[1]](sys.argv[2])
