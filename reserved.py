#!/usr/bin/env python3
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import json
import os
import re
import subprocess
import sys
import time

USAGE = """usage: reserved.py [--refresh | --publish]
    (none)      addresses other contributors hold in open kit issues and open decomp pull requests
    --refresh   re-read them from GitHub now
    --publish   write this fleet's claims into its one open kit issue"""

KIT_REPO = os.environ.get("DQIX_KIT_REPO", "ZevyaDev/dqix-decomp-kit")
DECOMP_REPO = os.environ.get("DQIX_DECOMP_REPO", "ZevyaDev/dqix-decomp")
PATH = f"{_kp.SP}/wlog/reserved.txt"
MAX_AGE = 600
CLAIMS_TITLE = "Fleet claims"
ADDR = re.compile(r"(?<![0-9A-Za-z])(?:0x)?(0[12][0-9a-fA-F]{6})(?![0-9A-Za-z])")
_HEX = r"`?(?:0x)?(0[12][0-9a-fA-F]{6})`?"
RANGE = re.compile(r"\[\s*%s\s*,\s*%s\s*[\)\]]|%s\s*(?:-|–|\.\.)\s*%s" % (_HEX, _HEX, _HEX, _HEX))


def starts_only(text):
    text = RANGE.sub(lambda m: m.group(1) or m.group(3), text)
    out, ends = [], None
    lines = text.splitlines()
    for i, line in enumerate(lines):
        if not line.lstrip().startswith("|"):
            ends = None
            out.append(line)
            continue
        cells = line.strip().strip("|").split("|")
        nxt = lines[i + 1] if i + 1 < len(lines) else ""
        if ends is None and re.match(r"\s*\|?\s*:?-{3,}", nxt):
            ends = {k for k, c in enumerate(cells) if re.search(r"\bend\b", c, re.I)}
        elif ends:
            cells = ["" if k in ends else c for k, c in enumerate(cells)]
        out.append("|".join(cells))
    return "\n".join(out)


def _gh(*args):
    r = subprocess.run(["gh", *args], capture_output=True, text=True, encoding="utf-8", timeout=60)
    if r.returncode:
        raise RuntimeError(r.stderr.strip() or "gh %s failed" % args[0])
    return r.stdout


def me():
    return _gh("api", "user", "--jq", ".login").strip()


def holders(issues, prs, login):
    held = {}
    for kind, repo, items in (("issue", KIT_REPO, issues), ("pr", DECOMP_REPO, prs)):
        for it in items:
            if (it.get("author") or {}).get("login") == login:
                continue
            text = "\n".join([it.get("title") or "", it.get("body") or ""]
                             + [f.get("path", "") for f in it.get("files") or []])
            for a in ADDR.findall(starts_only(text)):
                held.setdefault(a.lower(), f"{repo}#{it['number']}")
    return held


def refresh():
    login = me()
    issues = json.loads(_gh("issue", "list", "-R", KIT_REPO, "--state", "open", "--limit", "200",
                            "--json", "number,author,title,body"))
    prs = json.loads(_gh("pr", "list", "-R", DECOMP_REPO, "--state", "open", "--limit", "100",
                         "--json", "number,author,title,body,files"))
    held = holders(issues, prs, login)
    os.makedirs(os.path.dirname(PATH), exist_ok=True)
    tmp = PATH + ".tmp"
    with open(tmp, "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{a} {held[a]}\n" for a in sorted(held))
    os.replace(tmp, PATH)
    return held


def read(path=None):
    held = set()
    try:
        for line in open(path or PATH, encoding="utf-8"):
            if line.strip():
                held.add(line.split()[0].lower())
    except OSError:
        pass
    return held


_HELD = None


def load():
    global _HELD
    if _HELD is not None:
        return _HELD
    try:
        age = time.time() - os.path.getmtime(PATH)
    except OSError:
        age = None
    if age is None or age >= MAX_AGE:
        try:
            _HELD = set(refresh())
            return _HELD
        except (OSError, RuntimeError, ValueError, subprocess.TimeoutExpired) as e:
            print("reserved.py: cannot read reservations from GitHub (%s); %s" % (
                e, "using the last list" if age is not None else "NO reservation is honoured"),
                file=sys.stderr)
    _HELD = read()
    return _HELD


def claims():
    out = []
    root = os.path.join(_kp.SP, "claims")
    if not os.path.isdir(root):
        return out
    for mod in sorted(os.listdir(root)):
        if not re.fullmatch(r"main|\d{3}", mod):
            continue
        d = os.path.join(root, mod)
        for a in sorted(os.listdir(d)):
            if re.fullmatch(r"[0-9a-f]{8}", a) and os.path.isdir(os.path.join(d, a)):
                out.append(("main" if mod == "main" else "ov" + mod, a))
    return out


def body(rows):
    if not rows:
        return "None."
    return "| Module | Address |\n|---|---|\n" + "".join(f"| {m} | `{a}` |\n" for m, a in rows)


def publish():
    login = me()
    mine = json.loads(_gh("issue", "list", "-R", KIT_REPO, "--state", "open", "--author", login,
                          "--limit", "100", "--json", "number,title,body"))
    mine = [i for i in mine if i["title"] == CLAIMS_TITLE]
    text = body(claims())
    if not mine:
        if text == body([]):
            return "no claims, no issue"
        url = _gh("issue", "create", "-R", KIT_REPO, "--title", CLAIMS_TITLE, "--body", text).strip()
        return "opened " + url
    issue = mine[0]
    if (issue.get("body") or "").strip() == text.strip():
        return "#%d unchanged" % issue["number"]
    _gh("issue", "edit", str(issue["number"]), "-R", KIT_REPO, "--body", text)
    return "#%d updated" % issue["number"]


def main():
    arg = sys.argv[1] if len(sys.argv) > 1 else ""
    if arg in ("-h", "--help"):
        print(USAGE)
        return 0
    try:
        if arg == "--publish":
            print(publish())
            return 0
        held = refresh() if arg == "--refresh" else None
    except (OSError, RuntimeError, ValueError, subprocess.TimeoutExpired) as e:
        print("reserved.py: %s" % e, file=sys.stderr)
        return 1
    if held is None:
        load()
        held = {}
        for line in open(PATH, encoding="utf-8") if os.path.exists(PATH) else []:
            parts = line.split()
            if parts:
                held[parts[0]] = parts[1] if len(parts) > 1 else ""
    for a in sorted(held):
        print(a, held[a])
    return 0


if __name__ == "__main__":
    sys.exit(main())
