"""Flag source constructs the original developers would not plausibly have written.

    python plausible.py <file.cpp> [...]      prints one line per finding; exit 1 if any

blocker.py records a miss on a flagged file as IMPLAUSIBLE; pad/evo_score.py penalises one.
"""
import re
import sys

HW = re.compile(r"\(\s*volatile[^)]*\*\s*\)\s*0x0?4[0-9a-fA-F]{6}\b")
IDENT = r"[A-Za-z_]\w*"


def _body_start(lines):
    for i, ln in enumerate(lines):
        if ln.startswith("// USA:"):
            return i
    return 0


def findings(text):
    out = []
    lines = text.split("\n")
    start = _body_start(lines)
    body = "\n".join(lines[start:])

    for i in range(start, len(lines)):
        ln = lines[i]
        m = re.match(r"^\s+volatile\s+[^;(]*?\b(%s)\s*(=\s*([^;]+))?;" % IDENT, ln)
        if m and not (m.group(3) and HW.search(ln)):
            out.append((i + 1, "VOLATILE-LOCAL", m.group(1)))

        m = re.match(r"^\s+(?:const\s+)?(?:struct\s+)?%s\s*\*+\s*(%s)\s*=\s*&[^;]+;\s*$" % (IDENT, IDENT), ln)
        if m:
            name = m.group(1)
            rest = "\n".join(lines[i + 1:])
            if not re.search(r"(?<![\w.>])%s\b" % re.escape(name), rest):
                out.append((i + 1, "DEAD-ADDRESS-LOCAL", name))

        m = re.match(r"^\s+(%s)\s*=\s*(%s)\s*(\+\s*0\s*)?;\s*$" % (IDENT, IDENT), ln)
        if m and m.group(1) == m.group(2):
            out.append((i + 1, "SELF-ASSIGN", m.group(1)))

    inl = {}
    for m in re.finditer(r"(?m)^(?:static\s+)?inline\s+[^({;]*?\b(%s)\s*\(([^)]*)\)\s*\{([^{}]*)\}" % IDENT, text):
        name, params, fbody = m.group(1), m.group(2), m.group(3).strip()
        inl[name] = fbody
    for name, fbody in inl.items():
        callees = [c for c in re.findall(r"\b(%s)\s*\(" % IDENT, fbody) if c in inl and c != name]
        stmts = [s for s in fbody.split(";") if s.strip()]
        if callees and len(stmts) <= 2:
            depth, cur, seen = 1, callees[0], {name}
            while cur in inl and cur not in seen:
                seen.add(cur)
                nxt = [c for c in re.findall(r"\b(%s)\s*\(" % IDENT, inl[cur]) if c in inl and c not in seen]
                depth += 1
                if not nxt:
                    break
                cur = nxt[0]
            if depth >= 3:
                out.append((0, "INLINE-WRAPPER-CHAIN", "%s (depth %d)" % (name, depth)))
    return out


def main():
    bad = 0
    for p in sys.argv[1:]:
        for line, kind, what in findings(open(p, encoding="utf-8", errors="ignore").read()):
            print("%s:%d: %s %s" % (p, line, kind, what))
            bad += 1
    return 1 if bad else 0


if __name__ == "__main__":
    raise SystemExit(main())
