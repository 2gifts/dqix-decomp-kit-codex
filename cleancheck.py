import re
import sys
from collections import Counter

VOL = re.compile(r"\bvolatile\b")
USAGE = """python cleancheck.py <before> <after> <line,line,...>

Accept a rewrite that drops volatile only where it was invented: the listed lines of <before> carry
the invented volatile; every other line of <before> that says volatile must still appear verbatim in
<after>, and <after> must say volatile fewer times than <before>. Exit 0 accepted, 1 refused."""


def lines(path):
    return open(path, encoding="utf-8", errors="ignore").read().replace("\r\n", "\n").split("\n")


def check(before, after, private):
    keep = Counter(ln.strip() for n, ln in enumerate(before, 1) if VOL.search(ln) and n not in private)
    have = Counter(ln.strip() for ln in after)
    lost = [ln for ln, k in keep.items() if have[ln] < k]
    total = lambda text: sum(len(VOL.findall(ln)) for ln in text)
    return lost, total(before), total(after)


def main():
    if len(sys.argv) != 4:
        sys.exit(USAGE)
    private = {int(n) for n in sys.argv[3].split(",") if n.strip()}
    lost, was, now = check(lines(sys.argv[1]), lines(sys.argv[2]), private)
    for ln in lost:
        print(f"LOST {ln}")
    print(f"volatile {was} -> {now}")
    return 0 if not lost and now < was else 1


if __name__ == "__main__":
    sys.exit(main())
