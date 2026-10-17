import bisect
import collections
import os
import re
import shutil
import sys
import time

import addrname
import kitpaths
import mechclean
import offsetmembers as om
import plausible

REPO, SP = kitpaths.REPO, kitpaths.SP
WORK = om.WORK
USAGE = """python memberize.py [-j N] [--dry] [--class C]... [--any-name] [--header-classes]

Turns a free function that takes its object first, `R F(C* self, args...)` (extern "C" or C++), into
the member `R C::NAME(args...)`: self becomes this (members reached directly), the method is declared
in every definition of C in its header, the symbol is renamed to the member's mangled name in every
region's symbols.txt and .s, and every call `F(x, a)` becomes `x->NAME(a)` with the callers' own
declarations of F dropped. NAME is the `(semantic: NAME)` of the `// USA:` tag, else the identifier,
with the USA address appended when it lacks it. C must be a header class (plausible.game_types()).
A `char*`/`void*`/private-struct self is converted only when its class is proven: casts of it to one
class, the arguments every caller passes or the parameters it is passed on to (propagated across
files), or offsetmembers resolving its every offset against exactly one class; its byte offsets
become members through offsetmembers, and a function is left when one would need a member the header
lacks (listed in needs.tsv). A conversion is kept only when every unit it touches compiles to the
same object in every region it is built for, symbol names mapped. Output: $SP/handwork/memberize/
(manifest.tsv, memberized.tsv, needs.tsv, skipped.tsv). --dry stops before compiling and writes only the
reports; --class limits the
classes converted; --any-name takes a first parameter of any name, not only `self`; --header-classes adds
every class exactly one header defines."""
TAG = re.compile(r"//\s*USA:\s*(\S+)(?:\s*\(semantic:\s*([A-Za-z_]\w*)\s*\))?")
SYMBOL = re.compile(r"(?m)^(\S+) kind:function\([^)]*\) addr:0x([0-9a-fA-F]+)")
CAST = r"\(\s*(?:const\s+)?(?:struct\s+|class\s+)?([A-Za-z_]\w*)\s*\*\s*\)"
BYTEPTRS = {"char*", "unsignedchar*", "signedchar*", "u8*", "s8*"}


def bare(t):
    return re.sub(r"\b(?:const|volatile|struct|class)\b|\s", "", t or "")


def matching(code, i):
    depth = 0
    for j in range(i, len(code)):
        if code[j] in "([":
            depth += 1
        elif code[j] in ")]":
            depth -= 1
            if depth == 0:
                return j
    return len(code) - 1


def arg_spans(code, lparen):
    rparen = matching(code, lparen)
    spans, depth, cur = [], 0, lparen + 1
    for i in range(lparen + 1, rparen):
        if code[i] in "([{":
            depth += 1
        elif code[i] in ")]}":
            depth -= 1
        elif code[i] == "," and depth == 0:
            spans.append((cur, i))
            cur = i + 1
    spans.append((cur, rparen))
    out = []
    for a, b in spans:
        while a < b and code[a].isspace():
            a += 1
        while b > a and code[b - 1].isspace():
            b -= 1
        out.append((a, b))
    return ([] if len(out) == 1 and out[0][0] == out[0][1] else out), rparen


def unwrap(expr):
    e = expr.strip()
    while mechclean.wrapped(e):
        e = e[1:-1].strip()
    return e


def uncast(expr):
    e, casts = unwrap(expr), []
    while True:
        m = re.fullmatch(r"\(\s*(?:const\s+)?(?:struct\s+|class\s+)?([A-Za-z_][\w\s]*?)\s*\*+\s*\)\s*(.+)", e, re.S)
        if not m:
            return e, casts
        casts.append(re.sub(r"\s+", " ", m.group(1)))
        e = unwrap(m.group(2))


def alias_base(code, f, fn, name, depth=0):
    if fn is None or depth > 4:
        return name
    inits = [l[4] for l in fn.locals if l[1] == name]
    if len(inits) != 1 or not inits[0] or name in fn.params:
        return name
    writes = re.compile(r"(?<![\w.>])%s\s*(?:=(?!=)|[-+*/%%&|^]=|<<=|>>=|\+\+|--)|(?:\+\+|--)\s*%s\b"
                        % (re.escape(name), re.escape(name)))
    if len(list(writes.finditer(code, f["open"], f["close"]))) != 1:
        return name
    base = uncast(inits[0])[0]
    return alias_base(code, f, fn, base, depth + 1) if re.fullmatch(r"[A-Za-z_]\w*", base) else name


class Unit:
    def __init__(self, rel, text):
        self.rel, self.text = rel, text
        self.src = plausible.Src(text)
        self.code = om.flat(text)
        self.funcs = sorted(self.src.funcs, key=lambda f: f["open"])
        self.opens = [f["open"] for f in self.funcs]
        self.fns = {}
        self.globals = {}
        for _line, _stmt, d in self.src.decls:
            for name, ptr, arr, init in d[1]:
                self.globals[name] = d[0] + "*" * ptr + ("[]" if arr else "")

    def enclosing(self, pos):
        i = bisect.bisect_right(self.opens, pos) - 1
        return self.funcs[i] if i >= 0 and self.funcs[i]["open"] < pos < self.funcs[i]["close"] else None

    def fn(self, f):
        if f["open"] not in self.fns:
            try:
                self.fns[f["open"]] = plausible.Fn(self.src, f)
            except Exception:
                self.fns[f["open"]] = None
        return self.fns[f["open"]]

    def definitions(self):
        out, prev = [], 0
        for f in self.funcs:
            d = self.definition(f, prev)
            prev = f["close"] + 1
            if d:
                out.append(d)
        return out

    def definition(self, f, prev):
        name = f["name"]
        if f["in_struct"] or not re.fullmatch(r"[A-Za-z_]\w*", name) or not f["params"]:
            return None
        hits = list(re.finditer(r"\b%s\s*\(" % re.escape(name), self.code[prev:f["open"]]))
        if not hits:
            return None
        at = prev + hits[-1].start()
        lparen = prev + hits[-1].end() - 1
        rparen = matching(self.code, lparen)
        line = self.text.rfind("\n", 0, at) + 1
        head = self.code[line:at]
        rest = re.sub(r'extern\s+"C"|\b(?:ARM|THUMB)\b', " ", head)
        tags = list(TAG.finditer(self.text[prev:line]))
        ptype, pname = f["params"][0]
        return {"rel": self.rel, "name": name, "at": at, "line": line, "lparen": lparen, "rparen": rparen,
                "open": f["open"], "close": f["close"], "head": head,
                "extern_c": bool(re.search(r'extern\s+"C"', head)),
                "storage": bool(re.search(r"\b(?:static|inline|asm|template)\b", rest)),
                "ret": re.sub(r"\s+", " ", rest).strip(), "params": f["params"],
                "self": pname if pname and pname != "..." else None, "self_type": ptype,
                "semantic": tags[-1].group(2) if tags else None,
                "variants": len(re.findall(r"\b%s\s*\([^;{}]*\)\s*(?:const\s*)?\{" % re.escape(name), self.code))}


def symbol_index(current):
    usa, every = {}, collections.defaultdict(set)
    for rel, text in current.items():
        m = re.match(r"config/(\w+)/arm9/(?:overlays/(ov\d+)/)?symbols\.txt$", rel)
        if not m:
            continue
        for name, addr in SYMBOL.findall(text):
            every[addrname.ident(name)].add(name)
            if m.group(1) == "usa":
                usa[name] = (m.group(2) or "main", addr.lower()[-8:])
    return usa, every


def intrinsic(how):
    return bool(set(how.split("+")) & {"declared", "cast", "offsets", "declaration"})


def member_name(d, entry):
    name = d["semantic"] or d["name"]
    mod, addr = entry
    return name if addr in name.lower() else "%s_%s%s" % (name, "" if mod == "main" else mod + "_", addr)


class Model:
    def __init__(self, current, classes, jobs):
        self.current, self.classes, self.jobs = current, classes, jobs
        self.units = {rel: Unit(rel, text) for rel, text in current.items()
                      if (rel.startswith("src/") and rel.endswith((".c", ".cpp"))) or
                      (rel.startswith("include/") and rel.endswith((".h", ".hpp", ".inc")))}
        self.defs = {}
        for rel, u in self.units.items():
            if rel.startswith("src/"):
                for d in u.definitions():
                    self.defs.setdefault((rel, d["name"]), d)
        self.usa, self.every = symbol_index(current)
        self.bases, self.proven, self.conflicts = {}, {}, {}
        names = collections.Counter(k[1] for k in self.defs)
        self.idents = {}
        for k, d in self.defs.items():
            if names[k[1]] == 1:
                self.idents[k[1]] = k
                sym = self.symbol(d)
                if sym and sym != k[1]:
                    self.idents[sym] = k
        self.index()

    def symbol(self, d):
        if d["extern_c"]:
            return d["name"] if d["name"] in self.usa else None
        found = [n for n in self.every.get(d["name"], ()) if n.startswith("_Z") and n in self.usa]
        return found[0] if len(found) == 1 else None

    def index(self):
        self.calls, self.decls, self.other = (collections.defaultdict(list) for _ in range(3))
        macros = collections.defaultdict(set)
        for rel, u in self.units.items():
            for line in re.findall(r"(?m)^[ \t]*#[ \t]*define\b[^\n]*", u.text):
                for word in set(re.findall(r"[A-Za-z_]\w*", line)) & self.idents.keys():
                    macros[rel].add(word)
                    self.other[self.idents[word]].append((rel, 0))
        for rel, u in self.units.items():
            for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", u.code):
                key = self.idents.get(m.group(1))
                if key is None or m.group(1) in macros[rel]:
                    continue
                before = u.code[max(0, m.start() - 2):m.start()]
                if before.endswith((".", "->", "::")):
                    self.other[key].append((rel, m.start()))
                    continue
                lparen = m.end() - 1
                f = u.enclosing(m.start())
                if f is not None and m.start() > f["open"]:
                    spans, rparen = arg_spans(u.code, lparen)
                    self.calls[key].append({"rel": rel, "start": m.start(), "end": rparen + 1, "func": f,
                                            "args": [u.text[a:b] for a, b in spans]})
                    continue
                rparen = matching(u.code, lparen)
                tail = u.code[rparen + 1:rparen + 64].lstrip()
                if key[0] == rel and self.defs[key]["at"] == m.start():
                    continue
                if tail.startswith(";") and re.match(r"[^;{}()=]*$", u.code[u.code.rfind("\n", 0, m.start()) + 1:
                                                                             m.start()]):
                    params = u.code[lparen + 1:rparen]
                    self.decls[key].append({"rel": rel, "start": m.start(), "params": params})
                else:
                    self.other[key].append((rel, m.start()))
            for m in re.finditer(r"(?<![\w.>])(?<!::)\b([A-Za-z_]\w*)\b(?!\s*\()", u.code):
                key = self.idents.get(m.group(1))
                if key is not None:
                    self.other[key].append((rel, m.start()))

    def base_of(self, rel, f, expr):
        key = (rel, f["open"], expr)
        if key not in self.bases:
            u = self.units[rel]
            e, casts = uncast(expr)
            self.bases[key] = (alias_base(u.code, f, u.fn(f), e) if re.fullmatch(r"[A-Za-z_]\w*", e) else e), casts
        return self.bases[key]

    def typed(self, rel, f, e):
        key = (rel, f["open"], e, "typed")
        if key not in self.bases:
            u, fn, got = self.units[rel], self.units[rel].fn(f), (None, None)
            m = re.fullmatch(r"&\s*([A-Za-z_]\w*)", e)
            if e == "this" and "::" in f["name"]:
                cls = f["name"].rsplit("::", 1)[0]
                got = (cls, "this") if cls in self.classes else got
            elif m:
                t = bare((fn.types.get(m.group(1)) if fn else None) or u.globals.get(m.group(1)))
                got = (t, "addr") if t in self.classes else got
            elif re.fullmatch(r"[A-Za-z_]\w*", e):
                cls = om.game_pointer((fn.types.get(e) if fn else None) or u.globals.get(e), self.classes)
                got = (cls, "ptr") if cls else got
            self.bases[key] = got
        return self.bases[key]

    def arg_class(self, rel, f, expr):
        e = self.base_of(rel, f, expr)[0]
        got = self.typed(rel, f, e)
        if got[0] is None:
            key = (rel, f["name"])
            if key in self.defs and self.defs[key]["self"] == e and key in self.proven:
                return self.proven[key][0], "param"
        return got

    def prove(self):
        declared = {k: om.game_pointer(d["self_type"], self.classes) for k, d in self.defs.items()}
        declared = {k: c for k, c in declared.items() if c}
        self.proven = {k: (c, "declared") for k, c in declared.items()}
        for _ in range(16):
            votes = collections.defaultdict(lambda: collections.defaultdict(set))
            for k, d in self.defs.items():
                if k in declared or not d["self"]:
                    continue
                code = self.units[d["rel"]].code
                for m in re.finditer(CAST + r"\s*\(?\s*%s\b(?!\s*(?:[-+\[]|->))" % re.escape(d["self"]),
                                     code[d["open"]:d["close"]]):
                    if m.group(1) in self.classes:
                        votes[k][m.group(1)].add("cast")
            for g, sites in self.calls.items():
                gd = self.defs[g]
                for c in sites:
                    if g not in declared and c["args"]:
                        cls, how = self.arg_class(c["rel"], c["func"], c["args"][0])
                        if cls:
                            votes[g][cls].add("caller")
                    caller = (c["rel"], c["func"]["name"])
                    if caller in self.defs and caller not in declared and self.defs[caller]["self"]:
                        for i, a in enumerate(c["args"]):
                            if self.base_of(c["rel"], c["func"], a)[0] != self.defs[caller]["self"] or \
                                    i >= len(gd["params"]):
                                continue
                            cls = om.game_pointer(gd["params"][i][0], self.classes) or \
                                (self.proven[g][0] if i == 0 and g in self.proven and intrinsic(self.proven[g][1])
                                 else None)
                            if cls:
                                votes[caller][cls].add("callee")
            for g, ds in self.decls.items():
                if g in declared:
                    continue
                for dl in ds:
                    first = mechclean.split_args(dl["params"])[0] if dl["params"].strip() else ""
                    cls = om.game_pointer(mechclean.param_type(first) if first else "", self.classes)
                    if cls:
                        votes[g][cls].add("declaration")
            new = {k: (c, "declared") for k, c in declared.items()}
            for k, vs in votes.items():
                if k not in new and len(vs) == 1:
                    cls = next(iter(vs))
                    new[k] = (cls, "+".join(sorted(vs[cls])))
            if new == self.proven:
                break
            self.proven = new
        self.conflicts = {k: sorted(vs) for k, vs in votes.items() if len(vs) > 1}
        return self.proven

    def accesses(self, d):
        u, p = self.units[d["rel"]], d["self"]
        raw = bare(d["self_type"]) in BYTEPTRS | {"void*"}
        out = []
        for kind, m in om.matches(u.code, d["open"], d["close"], p, raw):
            off, mode, t, extra = om.access(kind, m, pointee(d["self_type"]))
            if mode != "cast":
                out.append((off, mode, t, extra, om.strictness(u.code, m)))
        return out

    def fit(self, d):
        found, macros = self.accesses(d), om.region_macros(self.current[d["rel"]])
        if len({a[0] for a in found}) < 2:
            return None
        fits = []
        for cls, lays in sorted(self.layouts.items()):
            lay = lays["usa"]
            if not lay or cls not in lay["types"]:
                continue
            good, bad = set(), 0
            for off, mode, t, extra, strict in found:
                want = om.canon(t, lay) if t else "void"
                o, md, why = om.place(off, mode, want, extra, lay, "usa", macros)
                if why:
                    continue
                expr, why = om.resolve(lay, cls, o, md, want, strict, extra)
                if expr:
                    good.add(o)
                elif why == "type" or o >= lay["types"][cls]["size"]:
                    bad += 1
            if not bad and len(good) >= 2:
                fits.append(cls)
        return fits[0] if len(fits) == 1 else None

    def prove_offsets(self, keys):
        self.layouts = om.prefetch(self.current, self.classes, ["usa"], self.jobs)
        for k in keys:
            d = self.defs[k]
            if k in self.proven or k in self.conflicts or not d["self"]:
                continue
            if bare(d["self_type"]) in BYTEPTRS | {"void*"}:
                cls = self.fit(d)
                if cls:
                    self.proven[k] = (cls, "offsets")


def class_bodies(text, cls):
    code = om.flat(text)
    out = []
    for m in re.finditer(r"(?m)^\s*(?:typedef\s+)?(?:struct|class)\s+%s\s*(?::[^;{]*)?\{" % re.escape(cls), code):
        out.append((m.end(), om.closing(code, m.end() - 1)))
    return out


def has_member(header_text, cls, name):
    code = om.flat(header_text)
    return any(re.search(r"\b%s\b" % re.escape(name), code[a:b]) for a, b in class_bodies(header_text, cls))


def static_checks(model, k, classes_wanted, any_name):
    d = model.defs[k]
    cls = model.proven.get(k, (None,))[0]
    if cls is None:
        return None, "class not proven" + (" (%s)" % "/".join(model.conflicts[k]) if k in model.conflicts else "")
    if classes_wanted and cls not in classes_wanted:
        return None, None
    if not any_name and d["self"] != "self":
        return None, None
    if d["storage"] or not d["ret"] or "::" in d["name"]:
        return None, "static, inline or member"
    if d["name"].startswith("_Z"):
        return None, "defined under its mangled name"
    if re.search(r"\bconst\b", d["self_type"]):
        return None, "const self"
    if d["variants"] != 1:
        return None, "defined once per region"
    if model.idents.get(d["name"]) != k:
        return None, "identifier defined more than once"
    header = om.header_of(model.current, cls)
    if not header:
        return None, "no single header defines %s" % cls
    if om.locally_defined(model.units[d["rel"]].code, cls):
        return None, "%s redefined in the unit" % cls
    sym = model.symbol(d)
    if not sym:
        return None, "symbol not found"
    if model.other.get(k):
        rel, at = model.other[k][0]
        return None, "named other than by a call in %s" % rel
    name = member_name(d, model.usa[sym])
    if has_member(model.current[f"include/{header}"], cls, name):
        return None, "%s already has %s" % (cls, name)
    return {"cls": cls, "header": header, "name": name, "old": sym, "how": model.proven[k][1]}, None


def receiver(model, S, c, cls):
    f = c["func"]
    e, casts = model.base_of(c["rel"], f, c["args"][0])
    key = (c["rel"], f["name"])
    if key in S and model.defs[key]["self"] == e and S[key]["cls"] == cls:
        return e + "->"
    got, how = model.arg_class(c["rel"], f, c["args"][0])
    if got == cls and how in ("ptr", "addr", "this"):
        return mechclean.receiver(e)
    if got is None and casts and casts[0] == cls:
        return mechclean.receiver(unwrap(c["args"][0]))
    return None


def line_span(text, start):
    a = text.rfind("\n", 0, start) + 1
    semi = text.find(";", start)
    b = text.find("\n", semi)
    return a, len(text) if b < 0 else b + 1


def replace_word(text, code, lo, hi, word, rep):
    rx = re.compile(r"(?<![\w.])(?<!->)(?<!::)\b%s\b" % re.escape(word))
    return om.apply_edits(text, [(m.start(), m.end(), rep) for m in rx.finditer(code, lo, hi)])


def callee_param(model, code, at, index):
    m = re.search(r"([A-Za-z_]\w*)\s*\($", code[:at].rstrip()[-200:])
    if not m:
        return None
    name = m.group(1)
    d = re.search(r"\b%s\s*\(([^;{}()]*)\)\s*;" % re.escape(name), code)
    if d:
        params = mechclean.split_args(d.group(1))
        return mechclean.param_type(params[index]) if index < len(params) else None
    key = model.idents.get(name)
    if key is not None and index < len(model.defs[key]["params"]):
        return model.defs[key]["params"][index][0]
    return None


def self_uses(model, code, lo, hi, cls):
    problems = []
    for m in re.compile(r"(?<![\w.])(?<!->)\bthis\b").finditer(code, lo, hi):
        before = code[lo:m.start()].rstrip()
        after = code[m.end():hi].lstrip()
        cast = re.search(r"\(\s*(?:const\s+)?(?:struct\s+|class\s+)?([A-Za-z_][\w\s]*?)\s*\*+\s*\)\s*\(?\s*$", before)
        if cast and cast.group(1) not in (cls, "void"):
            problems.append("casts self to %s*" % re.sub(r"\s+", " ", cast.group(1)))
        elif cast:
            continue
        elif after.startswith("->"):
            continue
        elif re.match(r"[-+\[]", after) and not after.startswith(("++", "--")):
            problems.append("does arithmetic on self")
        elif before.endswith(("(", ",")) and after.startswith((")", ",")):
            depth, i = 0, m.start() - 1
            while i > lo:
                if code[i] in ")]":
                    depth += 1
                elif code[i] in "([":
                    if depth == 0:
                        break
                    depth -= 1
                i -= 1
            spans, _ = arg_spans(code, i)
            index = next((n for n, (a, b) in enumerate(spans) if a <= m.start() < b + 1), 0)
            t = bare(callee_param(model, code, i + 1, index))
            if t.endswith("*") and t not in ("void*", cls + "*"):
                problems.append("passes self as %s" % t)
    return problems


def strip_this(text, code, lo, hi, keep):
    edits = []
    for m in re.compile(r"\bthis\s*->\s*([A-Za-z_]\w*)").finditer(code, lo, hi):
        if m.group(1) not in keep:
            edits.append((m.start(), m.start(1), ""))
    return om.apply_edits(text, edits)


def member_names(model, cls):
    names = set()
    lays = om.class_layouts(model.current, cls, om.REGIONS) or {}
    for lay in lays.values():
        if lay and cls in lay["types"]:
            names |= {f["name"] for f in lay["types"][cls]["fields"]}
    header = om.header_of(model.current, cls)
    text = model.current.get(f"include/{header}", "")
    code = om.flat(text)
    for a, b in class_bodies(text, cls):
        names |= set(re.findall(r"\b([A-Za-z_]\w*)\s*\(", code[a:b]))
    return names - mechclean.BUILTIN_WORDS - plausible.KEYWORDS


def private_type(model, d):
    t = bare(d["self_type"])
    if not t.endswith("*") or t.count("*") != 1:
        return None
    name = t[:-1]
    return name if name not in model.classes and om.locally_defined(model.units[d["rel"]].code, name) else None


def private_rewrite(text, code, lo, hi, var, private, cls, prelude, lays, live, built, exact=False):
    edits, misses = [], []
    starts = [0] + [i + 1 for i, c in enumerate(text) if c == "\n"]
    rx = re.compile(r"(&\s*)?(?<![\w.])(?<!->)\b%s\s*->\s*([A-Za-z_]\w*(?:\s*\.\s*[A-Za-z_]\w*)*)\b(?!\s*\()(\s*\[)?"
                    % re.escape(var))
    for m in rx.finditer(code, lo, hi):
        here = live[bisect.bisect_right(starts, m.start()) - 1]
        regions = sorted(here & set(built) or here)
        chain = re.sub(r"\s+", "", m.group(2)).split(".")
        got, why, where = set(), None, (None, m.group(2))
        for r in regions:
            own = om.layouts(prelude, r, private)
            lay = (lays or {}).get(r)
            if not own or private not in own["types"] or not lay or cls not in lay["types"]:
                why = "no layout"
                break
            owner, off, f = private, 0, None
            for name in chain:
                if f is not None:
                    owner = om.canon(f["base"], own) if not f["ptr"] and not f["dims"] else None
                f = next((x for x in own["types"].get(owner, {}).get("fields", []) if x["name"] == name), None)
                if f is None:
                    break
                off += f["off"]
            if f is None:
                why = "field"
                break
            ft = om.field_type(f, own)
            where = (off, "%s (%s.%s)" % (ft, private, ".".join(chain)))
            if m.group(1):
                expr, why = om.resolve(lay, cls, off, "address", ft, "address", exact=exact)
            elif f["dims"]:
                expr, why = om.resolve(lay, cls, off, "address", ft, exact=exact)
                if expr and expr.startswith("&"):
                    expr, why = None, "type"
            else:
                expr, why = om.resolve(lay, cls, off, "load", ft, om.strictness(code, m), exact=exact)
            if expr is None:
                break
            got.add(expr)
        if why is not None or len(got) != 1:
            misses.append({"off": where[0], "type": where[1], "mode": "field", "why": why or "regions differ",
                           "at": m.start(), "text": text[m.start():m.end()]})
            continue
        edits.append((m.start(), m.end() - len(m.group(3) or ""), next(iter(got)).replace("{v}", var)))
    return edits, misses


def locate(rel, text, name):
    u = Unit(rel, text)
    return u, next((x for x in u.definitions() if x["name"] == name), None)


def pointee(t):
    b = bare(t)
    return "void" if b == "void*" else "unsigned char" if b in ("unsignedchar*", "u8*") else "signed char"


def forward_aliases(text, code, lo, hi, p):
    edits = []
    for name, elem, start, end in om.aliases(code, lo, hi, p):
        uses = [m for m in re.compile(r"(?<![\w.>])\b%s\b" % re.escape(name)).finditer(code, lo, hi)
                if not start <= m.start() < end]
        if all(code[:m.start()].rstrip().endswith(("(", ",")) and code[m.end():].lstrip().startswith((")", ","))
               for m in uses):
            edits += [(m.start(), m.end(), p) for m in uses]
            gone = om.unused_alias(text, code, lo, hi, name, start, end, edits)
            if gone:
                edits.append(gone)
    return om.apply_edits(text, edits)


def convert_definition(model, S, rel, text, key):
    info, d0 = S[key], model.defs[key]
    u, d = locate(rel, text, d0["name"])
    if d is None:
        return None, "definition lost"
    cls, p = info["cls"], d["self"]
    lays = om.class_layouts(model.current, cls, om.REGIONS)
    live, built = om.line_regions(text, om.REGIONS), om.regions_for(rel, model.current[rel])
    raw = bare(d["self_type"]) in BYTEPTRS | {"void*"}
    edits, misses = [], []
    if p:
        lo, hi = d["open"], d["close"]
        exact = info.get("exact", False)
        edits, misses = om.rewrite(text, u.code, lo, hi, p, raw, cls, lays, live, built, pointee(d["self_type"]),
                                   exact=exact)
        e2, m2 = om.alias_edits(text, u.code, lo, hi, p, cls, lays, live, built, exact)
        edits, misses = edits + e2, misses + m2
        private = private_type(model, d)
        if private:
            e2, m2 = private_rewrite(text, u.code, lo, hi, p, private, cls, model.current[rel], lays, live, built,
                                     exact)
            edits, misses = edits + e2, misses + m2
    if misses:
        return None, misses
    new = om.apply_edits(text, edits)
    u, d = locate(rel, new, d0["name"])
    if p:
        new = forward_aliases(new, u.code, d["open"], d["close"], p)
        u, d = locate(rel, new, d0["name"])
        new = replace_word(new, u.code, d["open"], d["close"], p, "this")
        u, d = locate(rel, new, d0["name"])
        lo, hi = d["open"], d["close"]
        new = new[:lo] + re.sub(r"\(\s*(?:struct\s+|class\s+)?%s\s*\*\s*\)\s*this\b" % re.escape(cls), "this",
                                new[lo:hi]) + new[hi:]
        u, d = locate(rel, new, d0["name"])
        lo, hi = d["open"], d["close"]
        problems = self_uses(model, u.code, lo, hi, cls)
        if problems:
            return None, problems[0]
        fn = u.fn(next(f for f in u.funcs if f["open"] == lo))
        local = set(fn.types) if fn else set()
        members = member_names(model, cls)
        loose = {m.group(1) for m in re.compile(r"(?<![\w.>:])\b([A-Za-z_]\w*)\b").finditer(u.code, lo, hi)}
        clash = sorted((loose & members) - local - {"this"})
        if clash:
            return None, "name clash with member %s" % clash[0]
        new = strip_this(new, u.code, lo, hi, local)
        u, d = locate(rel, new, d0["name"])
    params = [new[a:b] for a, b in arg_spans(u.code, d["lparen"])[0]][1:]
    marks = " ".join(re.findall(r"\b(?:ARM|THUMB)\b", d["head"]))
    head = "%s%s %s::%s(%s)" % (marks + " " if marks else "", d["ret"], cls, info["name"], ", ".join(params))
    return new[:d["line"]] + head + new[d["rparen"] + 1:], None


def drop_declarations(text, code, spans):
    for a, b in sorted(set(spans), reverse=True):
        if (a == 0 or text[max(0, a - 2):a] == "\n\n") and text[b:b + 1] == "\n":
            b += 1
        text = text[:a] + text[b:]
    return text


def tidy_types(text, before):
    code = om.flat(text)
    for name in sorted(set(re.findall(r"(?m)^[ \t]*(?:typedef\s+)?struct\s+([A-Za-z_]\w*)\s*[;{]", code))):
        uses = len(re.findall(r"\b%s\b" % re.escape(name), code))
        own = re.findall(r"(?m)^[ \t]*struct\s+%s\s*;[ \t]*\n" % re.escape(name), code)
        blocks = [m for m in re.finditer(r"(?m)^[ \t]*(?:typedef\s+)?struct\s+%s\s*\{" % re.escape(name), code)]
        if uses != len(own) + len(blocks) or not re.search(r"\b%s\b" % re.escape(name), before):
            continue
        spans = []
        for m in blocks:
            end = om.closing(code, m.end() - 1)
            semi = code.find(";", end)
            stop = code.find("\n", semi)
            spans.append((m.start(), len(code) if stop < 0 else stop + 1))
        for m in re.finditer(r"(?m)^[ \t]*struct\s+%s\s*;[ \t]*\n" % re.escape(name), code):
            spans.append((m.start(), m.end()))
        text = drop_declarations(text, code, spans)
        code = om.flat(text)
    was = om.flat(before)
    for name in set(re.findall(r"(?m)^[ \t]*#\s*define\s+([A-Za-z_]\w*)(?![\w(])", text)):
        if re.search(r"\b%s\b" % re.escape(name), was) and not re.search(r"\b%s\b" % re.escape(name), code):
            text = re.sub(r"(?m)^[ \t]*#\s*define\s+%s(?![\w(])[^\n]*\n" % re.escape(name), "", text)
    text = re.sub(r"(?m)^[ \t]*#\s*if[^\n]*\n(?:[ \t]*\n)*[ \t]*#\s*else[ \t]*\n(?:[ \t]*\n)*[ \t]*#\s*endif[^\n]*\n", "",
                  text)
    return re.sub(r"(?m)^[ \t]*#\s*if[^\n]*\n(?:[ \t]*\n)*[ \t]*#\s*endif[^\n]*\n", "", text)


def rewrite_unit(model, S, rel):
    u, text, fails = model.units[rel], model.current[rel], {}
    edits, spans, touched = [], [], set()
    for key, info in S.items():
        for c in model.calls.get(key, ()):
            if c["rel"] != rel:
                continue
            if len(c["args"]) != len(model.defs[key]["params"]):
                fails[key] = "a call in %s passes %d argument(s)" % (rel, len(c["args"]))
                continue
            recv = receiver(model, S, c, info["cls"])
            if recv is None:
                fails[key] = "a call in %s passes %s" % (rel, c["args"][0].strip())
                continue
            edits.append((c["start"], c["end"], "%s%s(%s)" % (recv, info["name"],
                                                             ", ".join(a.strip() for a in c["args"][1:]))))
            touched.add(info["cls"])
        for dl in model.decls.get(key, ()):
            if dl["rel"] == rel:
                a, b = line_span(text, dl["start"])
                if re.fullmatch(r'[ \t]*(?:extern\s+"C"\s+)?[^;{}"]*;[ \t]*\n?', u.code[a:b]):
                    spans.append((a, b))
    new = om.apply_edits(text, edits)
    if spans:
        shift = lambda pos: pos + sum(len(r) - (b - a) for a, b, r in edits if b <= pos)
        new = drop_declarations(new, None, [(shift(a), shift(b)) for a, b in spans])
    for key, info in S.items():
        if key[0] != rel or key in fails:
            continue
        got, why = convert_definition(model, S, rel, new, key)
        if got is None:
            fails[key] = why
            continue
        new = got
        touched.add(info["cls"])
    if new == text:
        return text, fails
    if rel.startswith("src/"):
        reach = om.include_closure(model.current, rel)
        for cls in sorted(touched):
            header = om.header_of(model.current, cls)
            if header and header not in reach:
                new = om.add_include(new, header)
            new = re.sub(r"(?m)^[ \t]*struct\s+%s\s*;[ \t]*\n" % re.escape(cls), "", new)
        new = tidy_types(new, text)
    return new, fails


def declare_methods(model, S, current_headers):
    out = {}
    for cls in sorted({v["cls"] for v in S.values()}):
        rel = "include/" + om.header_of(model.current, cls)
        text = out.get(rel, current_headers.get(rel, model.current[rel]))
        pp = om.preprocess('#include "%s"\n' % rel[len("include/"):], "usa", "declare") or ""
        lines, words = [], set()
        for key, info in sorted(S.items(), key=lambda kv: kv[1]["name"]):
            if info["cls"] != cls:
                continue
            d = model.defs[key]
            params = [p for p in mechclean.split_args(model.current[d["rel"]][d["lparen"] + 1:d["rparen"]])][1:]
            params = [re.sub(r"\s+", " ", p).strip() for p in params]
            lines.append("    %s %s(%s);\n" % (d["ret"], info["name"], ", ".join(params)))
            for t in [d["ret"]] + params:
                words |= set(re.findall(r"[A-Za-z_]\w*", mechclean.param_type(t) if t != d["ret"] else t))
        words -= mechclean.BUILTIN_WORDS | set(plausible.SCALAR_TYPEDEFS) | {"struct", "class", "const", "volatile",
                                                                              "ARM", "THUMB"}
        missing = sorted(w for w in words if not re.search(r"\b%s\b" % re.escape(w), pp))
        for a, b in reversed(class_bodies(text, cls)):
            close_line = text.rfind("\n", 0, b) + 1
            text = text[:close_line] + "".join(lines) + text[close_line:]
        if missing:
            last = list(re.finditer(r"(?m)^#include[^\n]*\n", text))
            at = last[-1].end() if last else 0
            text = text[:at] + "".join("struct %s;\n" % w for w in missing) + text[at:]
        out[rel] = text
    return out


def build(model, S):
    fails, new = {}, {}
    rels = {k[0] for k in S} | {c["rel"] for k in S for c in model.calls.get(k, ())} | \
        {dl["rel"] for k in S for dl in model.decls.get(k, ())}
    for rel in sorted(rels):
        text, f = rewrite_unit(model, S, rel)
        for k, why in f.items():
            fails.setdefault(k, []).append({"needs": why} if isinstance(why, list) else why)
        if text != model.current[rel]:
            new[rel] = text
    headers = {rel: text for rel, text in new.items() if rel.startswith("include/")}
    new.update(declare_methods(model, S, headers))
    return new, fails


def settle_static(model, S):
    reasons = {}
    while S:
        new, fails = build(model, S)
        fails = {k: v for k, v in fails.items() if k in S}
        if not fails:
            return S, new, reasons
        for k, why in fails.items():
            reasons[k] = why
            S = {key: v for key, v in S.items() if key != k}
    return S, {}, reasons


def setup_include(work, new):
    shutil.rmtree(f"{work}/include", ignore_errors=True)
    shutil.copytree(f"{REPO}/include", f"{work}/include")
    for rel, text in new.items():
        if rel.startswith("include/"):
            with open(f"{work}/{rel}", "w", encoding="utf-8", newline="\n") as fh:
                fh.write(text)


def mangled(model, S, new, work):
    setup_include(work, new)
    os.makedirs(f"{work}/tu", exist_ok=True)
    out, bad = {}, {}
    for key, info in S.items():
        rel = key[0]
        path = f"{work}/tu/{rel.replace('/', '_')}"
        names = set()
        for region in om.regions_for(rel, model.current[rel]):
            sig = mechclean.signature(new[rel], path, region, f"{work}/include")
            if sig is None:
                bad[key] = "does not compile in %s" % region
                break
            prefix = "_ZN%d%s%d%sE" % (len(info["cls"]), info["cls"], len(info["name"]), info["name"])
            names |= {s[0] for s in sig[0] if s[0].startswith(prefix) and s[5] not in ("SHN_UNDEF",)}
        if key not in bad:
            if len(names) != 1:
                bad[key] = "mangles to %s" % (sorted(names) or "nothing")
            else:
                out[key] = names.pop()
    return out, bad


def rename_symbols(model, new, mapping):
    literal = addrname.alternation(mapping)
    for rel, text in model.current.items():
        if rel.endswith("symbols.txt"):
            got = re.sub(r"(?m)^(%s)(?= kind:)" % "|".join(map(re.escape, sorted(mapping, key=len, reverse=True))),
                         lambda m: mapping[m.group(1)], text)
        elif rel.endswith(".s"):
            got = literal.sub(lambda m: mapping[m.group(0)], new.get(rel, text))
        else:
            continue
        if got != text:
            new[rel] = got
    return new


def unit_check(model, rel, new, mapping, work):
    setup_include(work, new)
    path = f"{work}/tu/{rel.replace('/', '_')}"
    for region in om.regions_for(rel, model.current[rel]):
        before = mechclean.signature(model.current[rel], path, region, f"{REPO}/include")
        if before is None:
            continue
        after = mechclean.signature(new.get(rel, model.current[rel]), path, region, f"{work}/include")
        if after is None or mechclean.remap(before, mapping) != after:
            return False
    return True


def converge(model, S, jobs, work, reasons):
    while S:
        S, new, more = settle_static(model, S)
        reasons.update(more)
        if not S:
            return S, {}, {}
        names, bad = mangled(model, S, new, work)
        if bad:
            reasons.update(bad)
            S = {k: v for k, v in S.items() if k not in bad}
            continue
        mapping = {S[k]["old"]: n for k, n in names.items()}
        new = rename_symbols(model, new, mapping)
        broken, seen = mechclean.verify(model.current, new, mapping, jobs, work, regions=om.regions_for)
        if not broken:
            for k, n in names.items():
                S[k]["new"] = n
            return S, new, mapping
        print(f"{len(broken)} unit(s) differ, e.g. {broken[0]}", flush=True)
        drop = set()
        for rel in broken:
            text = model.current[rel] + "".join(model.current.get(h, "") for h in seen[rel])
            suspects = sorted(k for k, v in S.items() if k[0] == rel or v["old"] in text or
                              re.search(r"\b%s\b" % re.escape(k[1]), text) or
                              f"include/{v['header']}" in seen[rel])
            if len(suspects) == 1:
                drop |= set(suspects)
                continue
            found = set()
            for k in suspects:
                rest = {key: v for key, v in S.items() if key != k}
                rest, trial, _ = settle_static(model, rest)
                trial_names, trial_bad = mangled(model, rest, trial, work) if rest else ({}, {})
                if trial_bad:
                    continue
                trial_map = {rest[key]["old"]: n for key, n in trial_names.items()}
                if unit_check(model, rel, rename_symbols(model, trial, trial_map), trial_map, work):
                    found.add(k)
                    break
            drop |= found or set(suspects)
        retry = {k for k in drop if not S[k].get("exact")}
        for k in drop - retry:
            reasons[k] = "object differs"
        S = {k: dict(v, exact=True) if k in retry else v for k, v in S.items() if k not in drop - retry}
    return S, {}, {}


def needs_rows(model, S0, reasons):
    rows = []
    for k, why in sorted(reasons.items()):
        for entry in why if isinstance(why, list) else [why]:
            for m in entry["needs"] if isinstance(entry, dict) else []:
                rows.append((S0[k]["cls"], m["off"], m["type"], m["mode"], m["why"], k[0], k[1], m["text"]))
    return rows


def described(why):
    out = []
    for entry in why if isinstance(why, list) else [why]:
        if isinstance(entry, dict):
            entry = "needs members: " + "; ".join("%s %s %s" % ("" if m["off"] is None else hex(m["off"]), m["type"],
                                                                m["why"]) for m in entry["needs"])
        if entry not in out:
            out.append(entry)
    return "; ".join(out)


def main():
    args, jobs, dry, wanted, any_name, widen = sys.argv[1:], 8, False, set(), False, False
    while args:
        a = args.pop(0)
        if a == "-j" and args:
            jobs = int(args.pop(0))
        elif a == "--dry":
            dry = True
        elif a == "--class" and args:
            wanted.add(args.pop(0))
        elif a == "--any-name":
            any_name = True
        elif a == "--header-classes":
            widen = True
        else:
            sys.exit(USAGE)
    start = time.time()
    blobs, original = mechclean.tree_texts()
    current = om.overlay(original, blobs)
    model = Model(current, om.header_classes(current, widen), jobs)
    print(f"definitions {len(model.defs)} ({time.time() - start:.0f}s)", flush=True)
    model.prove()
    model.prove_offsets(sorted(k for k, d in model.defs.items() if d["self"] == "self" or any_name))
    selfish = [k for k, d in model.defs.items() if d["self"] == "self" or (any_name and d["self"])]
    S0, skipped = {}, {}
    for k in sorted(selfish):
        info, why = static_checks(model, k, wanted, any_name)
        if info:
            S0[k] = info
        elif why:
            skipped[k] = why
    hows = collections.Counter(v["how"].split("+")[0] if v["how"] != "declared" else "declared" for v in S0.values())
    print(f"self-taking definitions {len(selfish)}; class proven {sum(1 for k in selfish if k in model.proven)}; "
          f"candidates {len(S0)} {dict(hows)} ({time.time() - start:.0f}s)", flush=True)
    reasons = {}
    S, new, more = settle_static(model, dict(S0))
    reasons.update(more)
    print(f"convertible before compiling: {len(S)} ({time.time() - start:.0f}s)", flush=True)
    os.makedirs(WORK, exist_ok=True)
    with open(f"{WORK}/needs.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (cls, "" if off is None else hex(off), t, mode, why, rel,
                                                           name, text.replace("\n", " "))
                      for cls, off, t, mode, why, rel, name, text in needs_rows(model, S0, reasons))
    if not dry:
        S, new, mapping = converge(model, S, jobs, f"{WORK}/members", reasons)
        if S:
            om.record(new, blobs, "memberize")
    write_skipped(model, skipped, reasons)
    with open(f"{WORK}/memberized.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{k[0]}\t{k[1]}\t{v['old']}\t{v.get('new', '')}\t{v['cls']}::{v['name']}\t{v['how']}\n"
                      for k, v in sorted(S.items()))
    print(f"{'convertible' if dry else 'converted'} {len(S)} function(s) in {len(new)} file(s) "
          f"({time.time() - start:.0f}s)")
    return 0


def write_skipped(model, skipped, reasons):
    with open(f"{WORK}/skipped.tsv", "w", encoding="utf-8", newline="\n") as fh:
        for k, why in sorted({**skipped, **reasons}.items()):
            fh.write(f"{k[0]}\t{k[1]}\t{described(why)}\n")


if __name__ == "__main__":
    sys.exit(main())
