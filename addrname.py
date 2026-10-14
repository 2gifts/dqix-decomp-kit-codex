import collections
import glob
import os
import re
import subprocess
import sys

import kitpaths
import mechclean

REPO, SP = kitpaths.REPO, kitpaths.SP
WORK = f"{SP}/handwork/addrname"
USAGE = """python addrname.py [--skip-author EMAIL]... [-j N] [--dry]
       python addrname.py --remangle [--skip-author EMAIL]... [-j N]

Renames every mangled function symbol in config/usa whose name lacks its USA address, so the address
stays visible: <name>_<addr> in main, <name>_ovNNN_<addr> in an overlay, in every region's symbols.txt
and in every source, header and .s file that names it. --skip-author leaves the symbols whose
symbols.txt line that author last wrote. A rename is kept only when every C/C++ translation unit it
touches compiles to the same object, symbol names aside; an identifier that names two symbols is left.
The rewrites go to $SP/clean/manifest.tsv for landclean.sh. --dry prints the counts and writes
nothing.

--remangle renames a symbol whose `extern "C"` mangled-name callers agree on scalar parameter widths its
mangling contradicts (the name was guessed wrong; pointer types and arity are left to the mangling) to
the name that signature mangles to: every region's symbols.txt, its definition and declarations retyped,
its callers declaring the plain C++ function. It is kept on the same terms."""
FUNC = re.compile(r"(?m)^(\S+) kind:function\([^)]*\) addr:0x([0-9a-fA-F]+)")
SOURCES = (".c", ".cpp", ".h", ".hpp", ".inc")


def git(*args):
    return subprocess.run(["git", "-C", REPO, *args], capture_output=True, text=True, encoding="utf-8",
                          errors="replace").stdout


def own_name(mangled):
    if not mangled.startswith("_Z") or mangled.startswith(("_ZL", "_ZZ", "_ZT", "_ZGV", "_ZS")):
        return None
    nested = mangled.startswith("_ZN")
    i, span = (3 + mangled.startswith("_ZNK")) if nested else 2, None
    while i < len(mangled) and mangled[i].isdigit():
        j = i + re.match(r"\d+", mangled[i:]).end()
        span = (i, j, j + int(mangled[i:j]))
        i = span[2]
        if not nested:
            break
    if not span or span[2] > len(mangled) or (nested and mangled[i:i + 1] != "E") or mangled[i:i + 1] == "I":
        return None
    return span


def renamed(mangled, suffix):
    a, b, c = own_name(mangled)
    ident = mangled[b:c] + suffix
    return mangled[:a] + str(len(ident)) + ident + mangled[c:]


def authors(rel):
    out, who = {}, None
    for ln in git("blame", "--line-porcelain", "HEAD", "--", rel).split("\n"):
        if ln.startswith("author-mail "):
            who = ln.split()[1].strip("<>")
        elif ln.startswith("\t") and FUNC.match(ln[1:]):
            out[FUNC.match(ln[1:]).group(1)] = who
    return out


def targets(skip):
    out = {}
    for cfg in [f"{REPO}/config/usa/arm9"] + sorted(glob.glob(f"{REPO}/config/usa/arm9/overlays/ov*")):
        rel = os.path.relpath(f"{cfg}/symbols.txt", REPO).replace("\\", "/")
        tag = "" if cfg.endswith("arm9") else "ov%03d_" % int(os.path.basename(cfg)[2:])
        who = authors(rel) if skip else {}
        for name, addr in FUNC.findall(open(f"{cfg}/symbols.txt", encoding="utf-8").read()):
            addr = addr.lower()[-8:]
            if addr not in name.lower() and own_name(name) and who.get(name) not in skip:
                out[name] = renamed(name, "_%s%s" % (tag, addr))
    return out


def ident(mangled):
    a, b, c = own_name(mangled)
    return mangled[b:c]


def alternation(words):
    return re.compile(r"\b(?:%s)\b" % "|".join(sorted(map(re.escape, words), key=len, reverse=True))) if words else None


def rewrite(text, rel, mangled, idents):
    if mangled[0]:
        text = mangled[0].sub(lambda m: mangled[1][m.group(0)], text)
    if idents[0] and rel.endswith(SOURCES):
        text = idents[0].sub(lambda m: idents[1][m.group(0)], text)
    return text


remap = mechclean.remap
VIEW = re.compile(r'(?m)^[ \t]*extern\s+"C"\s+[^;{}()=]*?\b(_Z\w+)\s*\(([^;{}]*)\)\s*[;{]')


def probe_name(qual, ident, params, const):
    words = {w for t in params for w in re.findall(r"[A-Za-z_]\w*(?:::\w+)*", t)} - mechclean.BUILTIN_WORDS
    if any("::" in w for w in words) or (qual and "::" in qual):
        return None
    head = "".join("struct %s;\n" % w for w in sorted(words - {qual}))
    sig, c = ", ".join(params), " const" if const else ""
    body = ("struct %s { void %s(%s)%s; };\nvoid %s::%s(%s)%s {}\n" % (qual, ident, sig, c, qual, ident, sig, c)
            if qual else "void %s(%s) {}\n" % (ident, sig))
    os.makedirs(WORK, exist_ok=True)
    sig = mechclean.signature(head + body, f"{WORK}/probe.cpp", "usa")
    names = [s[0] for s in sig[0] if s[0].startswith("_Z") and ident in s[0]] if sig else []
    return names[0] if names else None


def remangle_targets(original, every, skip=()):
    owners = {}
    if skip:
        for cfg in [f"{REPO}/config/usa/arm9"] + sorted(glob.glob(f"{REPO}/config/usa/arm9/overlays/ov*")):
            owners.update(authors(os.path.relpath(f"{cfg}/symbols.txt", REPO).replace("\\", "/")))
    seen = collections.defaultdict(list)
    for rel, text in original.items():
        if rel.endswith((".c", ".cpp")):
            for m in VIEW.finditer(text):
                seen[m.group(1)].append([p.strip() for p in mechclean.split_args(m.group(2))]
                                        if m.group(2).strip() not in ("", "void") else [])
    out = {}
    for name, lists in seen.items():
        parsed = mechclean.demangle(name)
        if not parsed or not own_name(name) or len(every[ident(name)]) > 1 or owners.get(name) in skip:
            continue
        full, real, const = parsed
        qual = full.rsplit("::", 1)[0] if "::" in full else None
        votes, spelling = collections.Counter(), {}
        for params in lists:
            member = qual is not None and len(params) == len(real) + 1
            types = [mechclean.param_type(p) for p in params[member:]]
            if len(types) != len(real):
                continue
            merged = [r if "*" in mechclean.norm_type(r) or "*" in mechclean.norm_type(t) else t
                      for t, r in zip(types, real)]
            key = tuple(mechclean.norm_type(t) for t in merged)
            votes[key] += 1
            spelling.setdefault(key, merged)
        if not votes:
            continue
        key = votes.most_common(1)[0][0]
        if key == tuple(mechclean.norm_type(r) for r in real):
            continue
        new = probe_name(qual, ident(name), spelling[key], const)
        if new and new != name:
            out[name] = (new, qual, ident(name), real, spelling[key])
    return out


def retype(text, name, info):
    new, qual, base, real, params = info
    if re.search(r"\b%s\b" % re.escape(name), text):
        for scalar_cast, with_header in ((False, False), (True, False), (False, True), (True, True)):
            done = mechclean.typed_call(text, name, scalar_cast, with_header, params)
            if done is not None:
                text = done
                break
    old = tuple(mechclean.norm_type(r) for r in real)
    pattern = re.compile(r"\b(%s%s)\s*\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)(\s*const)?(?=\s*[;{])"
                         % ("(?:%s::)?" % re.escape(qual) if qual else "", re.escape(base)))

    def fix(m):
        given = [p.strip() for p in mechclean.split_args(m.group(2))] if m.group(2).strip() not in ("", "void") else []
        if tuple(mechclean.norm_type(p) for p in given) != old:
            return m.group(0)
        names = [mechclean.param_name(p) for p in given]
        spelled = ["%s %s" % (t, names[i]) if i < len(names) and names[i] else t for i, t in enumerate(params)]
        return "%s(%s)%s" % (m.group(1), ", ".join(spelled), m.group(3) or "")
    return pattern.sub(fix, text)


def remangle(jobs, skip):
    blobs, original = mechclean.tree_texts()
    every = collections.defaultdict(set)
    for path in glob.glob(f"{REPO}/config/*/arm9/**/symbols.txt", recursive=True):
        for name, _ in FUNC.findall(open(path, encoding="utf-8").read()):
            if own_name(name):
                every[ident(name)].add(name)
    targets = remangle_targets(original, every, skip)
    print(f"symbols whose callers agree on another signature: {len(targets)}", flush=True)
    while targets:
        mapping = {name: info[0] for name, info in targets.items()}
        literal = alternation(mapping)
        new = {}
        for rel, text in original.items():
            out = text
            if rel.endswith(SOURCES):
                for name, info in targets.items():
                    if name in out or info[2] in out:
                        out = retype(out, name, info)
            out = literal.sub(lambda m: mapping[m.group(0)], out)
            if out != text:
                new[rel] = out
        bad, seen = mechclean.verify(original, new, mapping, jobs, WORK)
        if not bad:
            break
        drop = set()
        for rel in bad:
            text = original[rel] + "".join(original[h] for h in seen[rel])
            drop |= {name for name, info in targets.items() if name in text or re.search(r"\b%s\b" % info[2], text)}
        print(f"{len(bad)} unit(s) differ, e.g. {bad[0]}; dropping {len(drop)} symbol(s)", flush=True)
        if not drop:
            return 1
        targets = {name: info for name, info in targets.items() if name not in drop}
    if targets and not mechclean.queue(new, blobs, "remangle"):
        return 1
    with open(f"{WORK}/remangled.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{name}\t{info[0]}\n" for name, info in sorted(targets.items()))
    print(f"remangled {len(targets)} symbol(s) in {len(new) if targets else 0} file(s)")
    return 0


def main():
    args = sys.argv[1:]
    skip, jobs, dry = set(), 8, "--dry" in args
    while args:
        a = args.pop(0)
        if a == "--skip-author" and args:
            skip.add(args.pop(0))
        elif a == "-j" and args:
            jobs = int(args.pop(0))
        elif a not in ("--dry", "--remangle"):
            sys.exit(USAGE)
    if "--remangle" in sys.argv:
        return remangle(jobs, skip)
    mapping = targets(skip)
    every = collections.defaultdict(set)
    for path in glob.glob(f"{REPO}/config/*/arm9/**/symbols.txt", recursive=True):
        for name, _ in FUNC.findall(open(path, encoding="utf-8").read()):
            if own_name(name):
                every[ident(name)].add(name)
    shared = {m for m in mapping if len(every[ident(m)]) > 1}
    mapping = {m: n for m, n in mapping.items() if m not in shared}
    print(f"targets {len(mapping) + len(shared)}; one identifier for two symbols: {len(shared)}", flush=True)
    blobs, original = mechclean.tree_texts()
    if dry:
        return 0
    while True:
        print(f"renames {len(mapping)}", flush=True)
        mangled = (alternation(mapping), mapping)
        names = {ident(m): ident(n) for m, n in mapping.items()}
        idents = (alternation(names), names)
        new = {rel: t for rel, t in ((rel, rewrite(text, rel, mangled, idents)) for rel, text in original.items())
               if t != original[rel]}
        bad, seen = mechclean.verify(original, new, mapping, jobs, WORK)
        if not bad:
            break
        back = collections.defaultdict(set)
        for m in mapping:
            back[ident(m)].add(m)
        drop = set()
        for rel in bad:
            text = original[rel] + "".join(original[h] for h in seen[rel])
            drop |= set(mangled[0].findall(text)) | {m for w in idents[0].findall(text) for m in back[w]}
        print(f"{len(bad)} unit(s) differ, e.g. {bad[0]}; dropping {len(drop)} rename(s)", flush=True)
        if not drop:
            return 1
        mapping = {m: n for m, n in mapping.items() if m not in drop}
    if not mechclean.queue(new, blobs, "addrname"):
        return 1
    os.makedirs(WORK, exist_ok=True)
    with open(f"{WORK}/renamed.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{m}\t{n}\n" for m, n in sorted(mapping.items()))
    print(f"renamed {len(mapping)} symbol(s) in {len(new)} file(s); left {sorted(shared)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
