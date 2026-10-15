import collections
import glob
import os
import re
import shutil
import subprocess
import sys

import gitread
import kitpaths
import mechclean
import plausible

REPO, SP = kitpaths.REPO, kitpaths.SP
WORK = f"{SP}/handwork/addrname"
USAGE = """python addrname.py [--keep REF] [-j N] [--dry]
       python addrname.py --remangle [--history REV]... [--casts] [--refine FILE] [--keep REF] [-j N]
       python addrname.py --shared [--keep REF]

Renames every function symbol in config/usa whose name lacks its USA address, so the address stays
visible: <name>_<addr> in main, <name>_ovNNN_<addr> in an overlay, in every region's symbols.txt and in
every source, header and .s file that names it. A mangled name gets the suffix on its own name; a plain
C name only when it has a capital letter (runtime and C library names stay). --keep REF leaves every
name REF's symbols.txt files already carry (e.g. upstream/main). A rename is kept only when every C/C++ translation unit it
touches compiles to the same object, symbol names aside; an identifier that names two symbols is left.
The rewrites go to $SP/clean/manifest.tsv for landclean.sh. --dry prints the counts and writes
nothing.

--remangle renames a symbol whose `extern "C"` mangled-name callers agree on a parameter count or scalar
widths its mangling contradicts (the name was guessed wrong; the mangling's pointer types are kept, an
added pointer parameter takes the callers' type when they agree on a real class, never void*) to the name that
signature mangles to: every region's symbols.txt, its definition and declarations retyped, its callers
declaring the plain C++ function, a file that never names the new class given `struct <class>;`. A call never
gains a cast; a cast an earlier rewrite added to fit the old name (`(int)ptr`, `(unsigned char)v`) is removed.
--history REV also takes the views REV's parent had, for a symbol whose calls still carry a cast REV added to
fit the old name. --casts types an integer parameter as the class every call casts to it (`Foo((int)self)`
with `GameObject* self` gives `GameObject*`), dropping the casts. --refine FILE retypes each listed symbol's
void* parameters as the one class every call passes. It is kept on the same terms, each symbol verified alone
when a unit differs.

--shared renames the names whose identifier another symbol shares: the class's own declaration and
qualified definitions directly, each unqualified call one at a time, kept when the unit's object moves to
the new name and nothing else changes."""
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


def plain_name(name):
    return re.fullmatch(r"[A-Za-z]\w*", name) is not None and re.search(r"[A-Z]", name) is not None \
        and not name.startswith("func_")


def renamed(name, suffix):
    if not own_name(name):
        return name + suffix
    a, b, c = own_name(name)
    base = name[b:c] + suffix
    return name[:a] + str(len(base)) + base + name[c:]


def kept_names(ref):
    rels = [r for r in git("ls-tree", "-r", "--name-only", ref, "--", "config").split() if r.endswith("symbols.txt")] \
        if ref else []
    return {n for text in gitread.blobs(REPO, [f"{ref}:{r}" for r in rels]).values() for n, _ in FUNC.findall(text)}


def targets(keep):
    out = {}
    for cfg in [f"{REPO}/config/usa/arm9"] + sorted(glob.glob(f"{REPO}/config/usa/arm9/overlays/ov*")):
        tag = "" if cfg.endswith("arm9") else "ov%03d_" % int(os.path.basename(cfg)[2:])
        for name, addr in FUNC.findall(open(f"{cfg}/symbols.txt", encoding="utf-8").read()):
            addr = addr.lower()[-8:]
            if addr not in name.lower() and (own_name(name) or plain_name(name)) and name not in keep:
                out[name] = renamed(name, "_%s%s" % (tag, addr))
    return out


def ident(name):
    span = own_name(name)
    return name[span[1]:span[2]] if span else name


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


def pointer(t):
    return any(c in mechclean.norm_type(t) for c in "*&")


def placeholder(t):
    return mechclean.norm_type(t) in ("void*", "voidconst*")


def remangle_targets(original, every, keep=()):
    seen = collections.defaultdict(list)
    for rel, text in original.items():
        if rel.endswith((".c", ".cpp")):
            for m in VIEW.finditer(text):
                seen[m.group(1)].append([p.strip() for p in mechclean.split_args(m.group(2))]
                                        if m.group(2).strip() not in ("", "void") else [])
    out = {}
    mechclean.demangle_all(seen)
    for name, lists in seen.items():
        parsed = mechclean.demangle(name)
        if not parsed or not own_name(name) or len(every[ident(name)]) > 1 or name in keep:
            continue
        full, real, const = parsed
        qual = full.rsplit("::", 1)[0] if "::" in full else None
        votes, spelling = collections.Counter(), collections.defaultdict(list)
        for params in lists:
            member = qual is not None and len(params) > len(real)
            types = [mechclean.param_type(p) for p in params[member:]]
            if any(pointer(r) and not pointer(t) for t, r in zip(types, real)):
                continue
            merged = [r if pointer(r) else t for t, r in zip(types, real)] + types[len(real):]
            key = tuple("*" if pointer(t) and not (i < len(real) and pointer(real[i])) else mechclean.norm_type(t)
                        for i, t in enumerate(merged))
            votes[key] += 1
            spelling[key].append(merged)
        if not votes:
            continue
        key = votes.most_common(1)[0][0]
        if key == tuple(mechclean.norm_type(r) for r in real):
            continue
        columns = list(zip(*spelling[key]))
        if any(k == "*" and (len({mechclean.norm_type(t) for t in col}) > 1 or placeholder(col[0]))
               for k, col in zip(key, columns)):
            continue
        spelled = [col[0] for col in columns]
        new = probe_name(qual, ident(name), spelled, const)
        if new and new != name:
            out[name] = (new, qual, ident(name), real, spelled)
    return out


def retype(text, name, info):
    new, qual, base, real, params = info
    if re.search(r"\b%s\b" % re.escape(name), text):
        for with_header in (False, True):
            done = mechclean.typed_call(text, name, False, with_header, params)
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


CAST = re.compile(r"\(\s*((?:const\s+)?(?:unsigned\s+|signed\s+)?[A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)?\s*\**)\s*\)\s*(.+)",
                  re.S)


def uncast(text, info):
    new, qual, base, real, params = info
    at = {i: {mechclean.norm_type(params[i])} | ({mechclean.norm_type(real[i])} if i < len(real) else set())
          for i in range(len(params)) if i >= len(real) or mechclean.norm_type(params[i]) != mechclean.norm_type(real[i])}
    for start, end, args in reversed(mechclean.call_sites(text, base)):
        kept = list(args)
        for i, arg in enumerate(args):
            m = CAST.fullmatch(arg)
            if i in at and m and mechclean.norm_type(m.group(1)) in at[i] and \
                    (mechclean.plain_expr(m.group(2)) or mechclean.wrapped(m.group(2))):
                inner = m.group(2).strip()
                kept[i] = inner[1:-1].strip() if mechclean.wrapped(inner) and mechclean.plain_expr(inner[1:-1].strip()) \
                    else inner
        if kept != args:
            open_at = text.index("(", start)
            text = text[:open_at + 1] + ", ".join(kept) + text[end - 1:]
    return text


INTEGER = {"int", "unsignedint", "long", "unsignedlong"}
CALL = re.compile(r"\b([A-Za-z_]\w*)\s*\(")


def call_args(text, at):
    depth, i, cur, args = 1, at, at, []
    while depth and i < len(text):
        if text[i] in "([{":
            depth += 1
        elif text[i] in ")]}":
            depth -= 1
        elif text[i] == "," and depth == 1:
            args.append(text[cur:i].strip())
            cur = i + 1
        i += 1
    args.append(text[cur:i - 1].strip())
    return [] if args == [""] else args


def class_pointer(fn, expr):
    expr = expr.strip()
    while mechclean.wrapped(expr):
        expr = expr[1:-1].strip()
    m = re.fullmatch(r"(&\s*)?([A-Za-z_]\w*)", expr)
    t = fn.types.get(m.group(2)) if m and fn else None
    if not t or "[" in t:
        return None
    t = re.sub(r"\b(?:const|volatile|struct|class)\b", "", t).replace(" ", "")
    base, stars = t.rstrip("*"), t.count("*") + (1 if m.group(1) else 0)
    if stars != 1 or not re.fullmatch(r"[A-Za-z_]\w*(?:::\w+)*", base) or base in mechclean.BUILTIN_WORDS \
            or base in plausible.SCALAR_TYPEDEFS:
        return None
    return base + "*"


def typed_calls(original, names, pick):
    votes = collections.defaultdict(lambda: collections.defaultdict(set))
    for rel, text in original.items():
        if not rel.endswith((".c", ".cpp")) or not any(n in text for n in names):
            continue
        src, fns = plausible.Src(text), {}
        for m in CALL.finditer(src.code):
            name = names.get(m.group(1))
            if not name:
                continue
            f = next((f for f in src.funcs if f["open"] < m.start() < f["close"]), None)
            if f is None:
                continue
            if f["open"] not in fns:
                try:
                    fns[f["open"]] = plausible.Fn(src, f)
                except Exception:
                    fns[f["open"]] = None
            fn = fns[f["open"]]
            for i, arg in enumerate(call_args(text, m.end())):
                operand = pick(name, i, arg)
                if operand is not None:
                    votes[name][i].add(class_pointer(fn, operand))
    return votes


def retyped(name, real, votes, wanted):
    spelled = list(real)
    for i, types in votes.items():
        if i < len(real) and wanted(real[i]) and len(types) == 1 and None not in types:
            spelled[i] = next(iter(types))
    if spelled == real:
        return None
    new = probe_name(None, ident(name), spelled, mechclean.demangle(name)[2])
    return (new, None, ident(name), real, spelled) if new and new != name else None


def free_functions(every, keep):
    return {base: next(iter(names)) for base, names in every.items()
            if len(names) == 1 and next(iter(names)).startswith("_Z") and not next(iter(names)).startswith("_ZN")
            and own_name(next(iter(names))) and next(iter(names)) not in keep}


def cast_targets(original, every, keep=()):
    def pick(name, i, arg):
        c = CAST.fullmatch(arg)
        if c and mechclean.norm_type(c.group(1)) in INTEGER and \
                (mechclean.plain_expr(c.group(2)) or mechclean.wrapped(c.group(2))):
            return c.group(2)
        return None
    votes = typed_calls(original, free_functions(every, keep), pick)
    mechclean.demangle_all(votes)
    out = {}
    for name, at in votes.items():
        if mechclean.demangle(name):
            info = retyped(name, mechclean.demangle(name)[1], at,
                           lambda r: mechclean.norm_type(r) in INTEGER)
            if info:
                out[name] = info
    return out


def refine_targets(original, every, names):
    free = {b: n for b, n in free_functions(every, ()).items() if n in names}
    votes = typed_calls(original, free, lambda name, i, arg: arg)
    out = {}
    for name, at in votes.items():
        parsed = mechclean.demangle(name)
        if parsed:
            info = retyped(name, parsed[1], at, placeholder)
            if info:
                out[name] = info
    return out


def history(revs):
    current = {}
    for path in glob.glob(f"{REPO}/config/usa/arm9/**/symbols.txt", recursive=True):
        cfg = os.path.relpath(path, REPO).replace("\\", "/")
        for name, addr in FUNC.findall(open(path, encoding="utf-8").read()):
            current[(cfg, addr.lower())] = name
    out = {}
    for rev in revs:
        old = {}
        cfgs = [c for c in git("ls-tree", "-r", "--name-only", f"{rev}^", "--", "config/usa/arm9").split()
                if c.endswith("symbols.txt")]
        for spec, text in gitread.blobs(REPO, [f"{rev}^:{c}" for c in cfgs]).items():
            cfg = spec.split(":", 1)[1]
            for name, addr in FUNC.findall(text):
                if current.get((cfg, addr.lower()), name) != name:
                    old[name] = current[(cfg, addr.lower())]
        names = alternation(old)
        rels = [r for r in git("diff-tree", "--no-commit-id", "-r", "--name-only", rev, "--", "src").split()
                if r.endswith((".c", ".cpp"))]
        for spec, text in gitread.blobs(REPO, [f"{rev}^:{r}" for r in rels]).items():
            out[f"{rev}:{spec.split(':', 1)[1]}"] = names.sub(lambda m: old[m.group(0)], text) if names else text
    return out


def symbol_idents():
    every = collections.defaultdict(set)
    for path in glob.glob(f"{REPO}/config/*/arm9/**/symbols.txt", recursive=True):
        for name, _ in FUNC.findall(open(path, encoding="utf-8").read()):
            every[ident(name)].add(name)
    return every


def declare(text, before, params):
    words = sorted({w for t in params for w in re.findall(r"[A-Za-z_]\w*", t)} - mechclean.BUILTIN_WORDS
                   - set(plausible.SCALAR_TYPEDEFS) - {"struct", "class", "const", "volatile"})
    missing = [w for w in words if not re.search(r"\b%s\b" % w, before)]
    if not missing:
        return text
    last = list(re.finditer(r"(?m)^#include[^\n]*\n", text))
    at = last[-1].end() if last else 0
    return text[:at] + "".join("struct %s;\n" % w for w in missing) + text[at:]


def remangle_texts(original, targets):
    mapping = {name: info[0] for name, info in targets.items()}
    literal = alternation(mapping)
    new = {}
    for rel, text in original.items():
        out = text
        if rel.endswith(SOURCES):
            for name, info in targets.items():
                if name in out or info[2] in out:
                    before = out
                    out = retype(out, name, info)
                    if rel.endswith((".c", ".cpp")):
                        out = uncast(out, info)
                    if out != before:
                        out = declare(out, before, info[4])
        out = literal.sub(lambda m: mapping[m.group(0)], out)
        if out != text:
            new[rel] = out
    return new, mapping


def remangle(jobs, keep, revs=(), casts=False, refine=()):
    blobs, original = mechclean.tree_texts()
    every = symbol_idents()
    landed = remangle_targets(history(revs), every, keep) if revs else {}
    targets = remangle_targets(original, every, keep)
    if casts:
        targets.update({n: i for n, i in cast_targets(original, every, keep).items() if n not in targets})
    if refine:
        targets.update({n: i for n, i in refine_targets(original, every, set(refine)).items() if n not in targets})
    calls = [text for rel, text in original.items() if rel.endswith((".c", ".cpp"))]
    targets.update({name: info for name, info in landed.items() if name not in targets and
                    any(info[2] in text and uncast(text, info) != text for text in calls)})
    print(f"symbols whose callers agree on another signature: {len(targets)}", flush=True)
    while targets:
        new, mapping = remangle_texts(original, targets)
        bad, seen = mechclean.verify(original, new, mapping, jobs, WORK)
        if not bad:
            break
        suspects = set()
        for rel in bad:
            text = original[rel] + "".join(original[h] for h in seen[rel])
            suspects |= {name for name, info in targets.items() if name in text or re.search(r"\b%s\b" % info[2], text)}
        drop = {name for name in suspects if len(suspects) == 1 or
                mechclean.verify(original, *remangle_texts(original, {name: targets[name]}), jobs, WORK, True)[0]} or suspects
        print(f"{len(bad)} unit(s) differ, e.g. {bad[0]}; dropping {len(drop)} of {len(suspects)} symbol(s) there",
              flush=True)
        if not drop:
            return 1
        targets = {name: info for name, info in targets.items() if name not in drop}
    if targets and not mechclean.queue(new, blobs, "remangle"):
        return 1
    with open(f"{WORK}/remangled.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{name}\t{info[0]}\n" for name, info in sorted(targets.items()))
    print(f"remangled {len(targets)} symbol(s) in {len(new) if targets else 0} file(s)")
    return 0


def distance(before, after, mapping):
    if before is None or after is None:
        return None
    want = remap(before, mapping)
    return sum(1 for a, b in zip(want, after) if a != b) + abs(len(want) - len(after))


def shared_rename(original, new, name, renamed_to, prior=None):
    base, fresh = ident(name), ident(renamed_to)
    parsed = mechclean.demangle(name)
    qual = parsed[0].rsplit("::", 1)[0] if parsed and "::" in parsed[0] else None
    real = tuple(mechclean.norm_type(t) for t in parsed[1]) if parsed else None
    call = re.compile(r"\b%s(?=\s*\()" % re.escape(base))
    out = {}
    for rel, text in ((r, new.get(r, t)) for r, t in original.items()):
        if not rel.endswith(SOURCES + (".s", "symbols.txt")) or (name not in text and base not in text):
            continue
        t = re.sub(r"\b%s\b" % re.escape(name), renamed_to, text)
        if rel.endswith(SOURCES) and qual:
            t = re.sub(r"\b%s::%s(?=\s*\()" % (re.escape(qual), re.escape(base)), "%s::%s" % (qual, fresh), t)
            body = mechclean.class_body(t, qual)
            if body:
                t = t[:body[0]] + call.sub(fresh, t[body[0]:body[1]]) + t[body[1]:]
        elif rel.endswith(SOURCES):
            def decl(m):
                params = [p for p in mechclean.split_args(m.group(2))] if m.group(2).strip() not in ("", "void") else []
                return m.group(0).replace(base, fresh, 1) if tuple(mechclean.norm_type(p) for p in params) == real \
                    else m.group(0)
            t = re.sub(r"\b(%s)\s*\(([^;{}()]*)\)(?=\s*[;{])" % re.escape(base), decl, t)
        if t != text:
            out[rel] = t
    merged = {**new, **out}
    mapping = {**(prior or {}), name: renamed_to}
    headers = [rel for rel in out if rel.startswith("include/")]
    tus = sorted(rel for rel in original if rel.endswith((".c", ".cpp")) and (
        rel in out or base in original[rel] or any(h[len("include/"):] in original[rel] for h in headers)))
    work = f"{WORK}/shared"
    os.makedirs(f"{work}/tu", exist_ok=True)
    shutil.rmtree(f"{work}/include", ignore_errors=True)
    shutil.copytree(f"{REPO}/include", f"{work}/include")
    for rel, text in merged.items():
        if rel.startswith("include/"):
            with open(f"{work}/{rel}", "w", encoding="utf-8", newline="\n") as fh:
                fh.write(text)
    for rel in tus:
        path = f"{work}/tu/{rel.replace('/', '_')}"
        regions = ["usa"] + [r for r in ("jpn", "eur") if re.search(r"\b%s\b" % r, original[rel])]
        before = {r: mechclean.signature(original[rel], path, r, f"{REPO}/include") for r in regions}
        if None in before.values():
            continue
        text = merged.get(rel, original[rel])
        score = lambda t: [distance(before[r], mechclean.signature(t, path, r, f"{work}/include"), mapping)
                           for r in regions]
        current = score(text)
        for m in reversed(list(call.finditer(text))):
            if all(d == 0 for d in current if d is not None) and None not in current:
                break
            trial = text[:m.start()] + fresh + text[m.end():]
            got = score(trial)
            if None not in got and (None in current or sum(got) < sum(current)):
                text, current = trial, got
        if None in current or any(current):
            return None, rel
        if text != original[rel]:
            merged[rel] = text
    return merged, None


def main():
    args = sys.argv[1:]
    ref, jobs, dry, revs, refine = None, 8, "--dry" in args, [], []
    while args:
        a = args.pop(0)
        if a == "--keep" and args:
            ref = args.pop(0)
        elif a == "-j" and args:
            jobs = int(args.pop(0))
        elif a == "--history" and args:
            revs.append(args.pop(0))
        elif a == "--refine" and args:
            refine = open(args.pop(0), encoding="utf-8").read().split()
        elif a not in ("--dry", "--remangle", "--shared", "--casts"):
            sys.exit(USAGE)
    keep = kept_names(ref)
    if "--remangle" in sys.argv:
        return remangle(jobs, keep, revs, "--casts" in sys.argv, refine)
    mapping = targets(keep)
    every = symbol_idents()
    shared = {m for m in mapping if len(every[ident(m)]) > 1}
    if "--shared" in sys.argv:
        blobs, original = mechclean.tree_texts()
        new, done = {}, []
        mechclean.demangle_all(shared)
        for name in sorted(shared):
            merged, bad = shared_rename(original, new, name, mapping[name], {d: mapping[d] for d in done})
            print(f"{name}: " + (f"left, {bad} differs" if merged is None else f"renamed to {mapping[name]}"),
                  flush=True)
            if merged is not None:
                new, done = merged, done + [name]
        new = {rel: text for rel, text in new.items() if text != original[rel]}
        return 0 if not new or mechclean.queue(new, blobs, "addrname") else 1
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
