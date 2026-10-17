import ast
import bisect
import collections
import functools
import glob
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor

import buildcfg
import kitpaths
import mechclean
import plausible
from elfobj import ELFFile

REPO, SP = kitpaths.REPO, kitpaths.SP
WORK = f"{SP}/handwork/memberize"
MANIFEST = f"{WORK}/manifest.tsv"
REGIONS = ("usa", "jpn", "eur")
DEFINES = {r: set(flags[1::2]) for r, flags in mechclean.REGION_DEFINES.items()}
USAGE = """python offsetmembers.py [-j N] [--dry] [--class C]... [--header-classes]

Rewrites byte-offset accesses through a pointer to a header class (plausible.game_types()) as member
accesses: `*(T*)((char*)p + K)` -> `p->m`, `(T*)((char*)p + K)` -> `&p->m` or `p->m` (an array),
`((T*)p)[i]` -> `p->m[i]`, `((S*)((char*)p + K))->x` -> `p->m.x`, also through a `char*` alias of p
and with K picked per region by a `#define X(jpnValue, usaValue)`. Layouts are read from the compiler
(offsetof/sizeof probes per region); an access is rewritten only when the member has a compatible type
at that offset in every region the line is built for (config/<region>/**/delinks.txt, #if region
blocks), never into a pad. Headers are not changed. A function's rewrites are kept only when its unit
compiles to the same object in every region it is built for, retried with exact member types when a
signedness change differs. Output: $SP/handwork/memberize/<path>, manifest.tsv (path, blob it
replaces, why) and offsets.tsv. --dry writes only offsets.tsv; --class limits the classes;
--header-classes adds every class exactly one header defines."""

NUM = r"(?:0[xX][0-9a-fA-F]+|\d+)[uUlL]*"
TERM = r"(?:%s|[A-Za-z_]\w*\(\s*%s\s*,\s*%s\s*\))" % (NUM, NUM, NUM)
OFF = r"%s(?:\s*\+\s*%s)*" % (TERM, TERM)
TYPE = r"(?:const\s+)?(?:struct\s+|class\s+)?(?:unsigned\s+|signed\s+)?[A-Za-z_]\w*(?:\s+(?:int|long|short|char))?" \
       r"(?:\s*\*)*"
BYTECAST = r"\(\s*(?:const\s+)?(?:unsigned\s+|signed\s+)?(?:char|u8|s8)\s*\*\s*\)"
INTCAST = r"\(\s*(?:unsigned\s+int|unsigned\s+long|unsigned|int|long|u32|s32)\s*\)"
BYTES = {"signed char", "unsigned char"}
SIZES = {"signed char": 1, "unsigned char": 1, "bool": 1, "short": 2, "unsigned short": 2, "int": 4,
         "unsigned int": 4, "float": 4, "long long": 8, "unsigned long long": 8, "double": 8}
INTS = set(SIZES) - {"float", "double"}
PAD = re.compile(r"^(?:_*pad|_*unk|unknown|reserved|filler|gap)", re.I)
TYPE_HEAD = re.compile(r"\b(struct|class|union)\s+([A-Za-z_]\w*)\s*(?::\s*([^{};()]*))?\{")
ACCESS = re.compile(r"^\s*(?:(?:public|private|protected)\s*:\s*)+")
NOT_FIELD = re.compile(r"^(?:typedef|using|friend|static|enum|template|virtual|explicit|inline)\b|\boperator\b")
LAYOUT_FORMAT = 2
LOCK = threading.Lock()
LOCKS = {}
LAYOUTS = {}


def value(text, region="usa", macros=None):
    def pick(m):
        which = (macros or {}).get(m.group(1), {}).get(region)
        return m.group(2 + which) if which in (0, 1) else "?"
    picked = re.sub(r"([A-Za-z_]\w*)\(\s*(%s)\s*,\s*(%s)\s*\)" % (NUM, NUM), pick, text)
    return None if "?" in picked else number(picked)


@functools.lru_cache(maxsize=64)
def region_macros(text):
    out = collections.defaultdict(dict)
    live = line_regions(text, REGIONS)
    for n, line in enumerate(text.split("\n")):
        m = re.match(r"\s*#\s*define\s+([A-Za-z_]\w*)\(\s*(\w+)\s*,\s*(\w+)\s*\)\s+(\w+)\s*$", line)
        if m and m.group(4) in (m.group(2), m.group(3)):
            for r in live[n]:
                out[m.group(1)][r] = 0 if m.group(4) == m.group(2) else 1
    return dict(out)


def number(text):
    total = 0
    for x in re.findall(r"0[xX][0-9a-fA-F]+|\d+", text):
        total += int(x, 16) if x[:2] in ("0x", "0X") else int(x, 8) if len(x) > 1 and x[0] == "0" else int(x)
    return total


@functools.lru_cache(maxsize=None)
def built_regions():
    out = collections.defaultdict(list)
    for region in REGIONS:
        for path in [f"{REPO}/config/{region}/arm9/delinks.txt"] + \
                sorted(glob.glob(f"{REPO}/config/{region}/arm9/overlays/ov*/delinks.txt")):
            try:
                text = open(path, encoding="utf-8").read()
            except OSError:
                continue
            for rel in re.findall(r"(?m)^(src/\S+?):\s*$", text):
                if region not in out[rel]:
                    out[rel].append(region)
    return dict(out)


def regions_for(rel, text=""):
    return built_regions().get(rel) or mechclean.text_regions(rel, text)


def condition(kind, rest, defines):
    rest = re.sub(r"//.*|/\*.*?\*/", "", rest).strip()
    if kind in ("ifdef", "ifndef"):
        word = (rest.split() or [""])[0]
        return None if word not in REGIONS else (word in defines) == (kind == "ifdef")
    expr = re.sub(r"\bdefined\s*(?:\(\s*(\w+)\s*\)|\s+(\w+))",
                  lambda m: ("1" if (m.group(1) or m.group(2)) in defines else "0")
                  if (m.group(1) or m.group(2)) in REGIONS else "X", rest)
    if re.search(r"[A-Za-z_]", expr):
        return None
    try:
        return truth(ast.parse(re.sub(r"!(?!=)", " not ", expr.replace("&&", " and ").replace("||", " or ")),
                               mode="eval").body)
    except (SyntaxError, ValueError):
        return None


def truth(node):
    if isinstance(node, ast.BoolOp):
        vals = [truth(v) for v in node.values]
        return all(vals) if isinstance(node.op, ast.And) else any(vals)
    if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.Not):
        return not truth(node.operand)
    if isinstance(node, ast.Constant) and isinstance(node.value, int):
        return bool(node.value)
    raise ValueError(ast.dump(node))


def line_regions(text, regions):
    frames, out = [], []
    for line in text.split("\n"):
        out.append({r for r in regions if all(f[0][r] is not False for f in frames)})
        m = re.match(r"\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)", line)
        if not m:
            continue
        kind, rest = m.groups()
        if kind in ("if", "ifdef", "ifndef"):
            vals = {r: condition(kind, rest, DEFINES[r]) for r in regions}
            frames.append([vals, dict(vals)])
        elif frames and kind == "elif":
            cur, taken = frames[-1]
            for r in regions:
                v = condition("if", rest, DEFINES[r])
                cur[r] = False if taken[r] is True else None if taken[r] is None else v
                taken[r] = True if taken[r] is True or v is True else None if v is None or taken[r] is None else False
        elif frames and kind == "else":
            cur, taken = frames[-1]
            for r in regions:
                cur[r] = None if taken[r] is None else not taken[r]
        elif frames and kind == "endif":
            frames.pop()
    return out


def flat(text):
    disguised = re.sub(r"(?m)^[ \t]*#.*$", lambda m: re.sub(r"\b(?:jpn|eur|usa|JPN|EUR|USA|IS_JPN|IS_EUR)\b",
                                                           lambda w: "_" * len(w.group(0)), m.group(0)), text)
    return plausible.strip_code(disguised)


def closing(code, i):
    depth = 0
    for j in range(i, len(code)):
        if code[j] == "{":
            depth += 1
        elif code[j] == "}":
            depth -= 1
            if depth == 0:
                return j
    return len(code) - 1


def statements(body):
    out, i, seg, depth, opaque = [], 0, 0, 0, False
    while i < len(body):
        c = body[i]
        if c in "([":
            depth += 1
        elif c in ")]":
            depth -= 1
        elif c == ";" and depth == 0:
            out.append(body[seg:i])
            seg = i + 1
        elif c == "{" and depth == 0:
            head = ACCESS.sub("", body[seg:i]).strip()
            end = closing(body, i)
            kind = re.match(r"(struct|union|class|enum)\b\s*([A-Za-z_]\w*)?", head)
            if kind and not head.endswith(")"):
                semi = body.find(";", end)
                semi = len(body) if semi < 0 else semi
                decl = body[end + 1:semi].strip()
                if kind.group(1) == "enum":
                    if decl:
                        out.append("int " + decl)
                elif kind.group(2) and decl:
                    out.append("%s %s" % (kind.group(2), decl))
                elif not kind.group(2):
                    opaque = True
                i = seg = semi + 1
                continue
            i = seg = end + 1
            continue
        i += 1
    return out, opaque


def fields(stmts):
    out, opaque = [], False
    for s in stmts:
        s = ACCESS.sub("", s).strip()
        if not s or NOT_FIELD.search(s):
            continue
        fp = re.match(r"^[^()]*\(\s*\*\s*([A-Za-z_]\w*)\s*\)\s*\(.*\)\s*$", s, re.S)
        if fp:
            out.append({"name": fp.group(1), "base": "void", "ptr": 1, "dims": 0})
            continue
        if "(" in s or re.search(r"(?<!:):(?!:)", s):
            continue
        d = plausible.parse_decl(s)
        if not d:
            opaque = True
            continue
        for name, ptr, arr, init in d[1]:
            if init is not None:
                continue
            m = re.search(r"\b%s\s*((?:\[[^\]]*\]\s*)*)" % re.escape(name), s)
            out.append({"name": name, "base": d[0], "ptr": ptr, "dims": m.group(1).count("[") if m else 0})
    return out, opaque


def type_defs(code):
    found = collections.defaultdict(list)
    for m in TYPE_HEAD.finditer(code):
        end = closing(code, m.end() - 1)
        found[m.group(2)].append((m.group(3) or "", code[m.end():end]))
    out = {}
    for name, defs in found.items():
        if len({re.sub(r"\s+", " ", d[1]).strip() for d in defs}) == 1:
            stmts, opaque = statements(defs[0][1])
            flds, odd = fields(stmts)
            bases = [w for w in re.findall(r"[A-Za-z_]\w*", defs[0][0]) if w not in ("public", "private", "protected",
                                                                                     "virtual")]
            out[name] = {"fields": flds, "opaque": opaque or odd, "bases": bases}
    return out


def inherited(defs, name, seen=()):
    d = defs.get(name)
    if d is None or name in seen:
        return []
    out = []
    for b in d["bases"]:
        out += inherited(defs, b, seen + (name,))
    return out + d["fields"]


def typedefs(code):
    out = {}
    for m in re.finditer(r"\btypedef\s+([^;{}()]+?)\s*;", code):
        d = plausible.parse_decl(m.group(1))
        if d and len(d[1]) == 1 and not d[1][0][2]:
            out[d[1][0][0]] = d[0] + "*" * d[1][0][1]
    return out


def compile_values(text, region, tag):
    os.makedirs(f"{WORK}/probe", exist_ok=True)
    path = f"{WORK}/probe/{tag}.{threading.get_ident()}.cpp"
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    obj = path + ".o"
    r = subprocess.run(buildcfg.tool_command(buildcfg.cc_path(None)) + mechclean.region_flags(region) +
                       ["-c", path, "-o", obj], cwd=REPO, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if r.returncode:
        return None
    out = {}
    with open(obj, "rb") as fh:
        elf = ELFFile(fh)
        for s in elf.get_section_by_name(".symtab").iter_symbols():
            if s.name.startswith("memberize_probe_") and isinstance(s["st_shndx"], int):
                data = elf.get_section(s["st_shndx"]).data()
                out[s.name] = list(struct.unpack_from("<%dI" % (s["st_size"] // 4), data, s["st_value"]))
    os.remove(obj)
    return out


def preprocess(text, region, tag):
    os.makedirs(f"{WORK}/probe", exist_ok=True)
    path = f"{WORK}/probe/{tag}.{threading.get_ident()}.pp.cpp"
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    r = subprocess.run(buildcfg.tool_command(buildcfg.cc_path(None)) + mechclean.region_flags(region) + ["-E", path],
                       cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace")
    return r.stdout if r.returncode == 0 else None


def measure(prelude, types, region, tag):
    if not types:
        return {}
    lines = [prelude, ""]
    for i, (name, flds) in enumerate(types):
        vals = ["sizeof(%s)" % name]
        for f in flds:
            acc = "((%s*)0)->%s" % (name, f["name"])
            vals += ["(unsigned int)&%s" % acc, "sizeof(%s)" % acc] + \
                    ["sizeof(%s%s)" % (acc, "[0]" * (k + 1)) for k in range(f["dims"])]
        lines.append("unsigned int memberize_probe_%d[] = {%s};" % (i, ", ".join(vals)))
    got = compile_values("\n".join(lines) + "\n", region, tag)
    if got is not None:
        return {name: (flds, got.get("memberize_probe_%d" % i)) for i, (name, flds) in enumerate(types)}
    if len(types) > 1:
        half = len(types) // 2
        return {**measure(prelude, types[:half], region, tag), **measure(prelude, types[half:], region, tag)}
    name, flds = types[0]
    if len(flds) < 2:
        return {}
    parts = [measure(prelude, [(name, part)], region, tag).get(name)
             for part in (flds[:len(flds) // 2], flds[len(flds) // 2:])]
    parts = [p for p in parts if p and p[1]]
    if not parts:
        return {}
    return {name: ([f for p in parts for f in p[0]], parts[0][1][:1] + [v for p in parts for v in p[1][1:]])}


def parse(prelude, region):
    pp = preprocess(prelude, region, "layout")
    if pp is None:
        return None
    pp = re.sub(r"(?m)^#line.*$", "", pp)
    code = plausible.strip_code(pp)
    key = "%s\0%s\0%s\0%s" % (LAYOUT_FORMAT, region, prelude, pp)
    return {"sha": hashlib.sha1(key.encode("utf-8", "replace")).hexdigest(),
            "defs": type_defs(code), "typedefs": typedefs(code),
            "enums": sorted(set(re.findall(r"\benum\s+([A-Za-z_]\w*)", code)))}


def closure(info, cls):
    seen, todo = [], [cls]
    lay = {"typedefs": info["typedefs"], "enums": info["enums"]}
    while todo:
        name = todo.pop()
        if name in seen or name not in info["defs"]:
            continue
        seen.append(name)
        todo += [canon(f["base"], lay) for f in inherited(info["defs"], name) if f["ptr"] == 0]
    return seen


def compute(prelude, region, cls):
    info = cached(("parse", prelude, region), lambda: parse(prelude, region))
    if info is None:
        return None
    cache = f"{WORK}/layout/{info['sha']}_{cls}.json"
    try:
        with open(cache, encoding="utf-8") as fh:
            return json.load(fh)
    except (OSError, ValueError):
        pass
    types = [(name, inherited(info["defs"], name)) for name in closure(info, cls)]
    got = measure(prelude, types, region, "layout")
    out = {"types": {}, "typedefs": info["typedefs"], "enums": info["enums"]}
    for name, _ in types:
        flds, vals = got.get(name, (None, None))
        if not vals:
            continue
        laid, k = [], 1
        for f in flds:
            laid.append(dict(f, off=vals[k], size=vals[k + 1], elem=vals[k + 2:k + 2 + f["dims"]]))
            k += 2 + f["dims"]
        out["types"][name] = {"size": vals[0], "fields": laid, "opaque": info["defs"][name]["opaque"]}
    os.makedirs(os.path.dirname(cache), exist_ok=True)
    tmp = "%s.%d.%d.tmp" % (cache, os.getpid(), threading.get_ident())
    with open(tmp, "w", encoding="utf-8") as fh:
        json.dump(out, fh)
    os.replace(tmp, cache)
    return out


def cached(key, make):
    with LOCK:
        lock = LOCKS.setdefault(key, threading.Lock())
    with lock:
        if key not in LAYOUTS:
            LAYOUTS[key] = make()
    return LAYOUTS[key]


def layouts(prelude, region, cls):
    return cached((prelude, region, cls), lambda: compute(prelude, region, cls))


HEADERS = []


def defining_headers(original):
    with LOCK:
        if not HEADERS or HEADERS[0][0] is not original:
            out = collections.defaultdict(list)
            for rel, text in original.items():
                if rel.startswith("include/"):
                    for name in set(re.findall(r"(?m)^\s*(?:typedef\s+)?(?:struct|class)\s+([A-Za-z_]\w*)\s*"
                                               r"(?::[^;{]*)?\{", flat(text))):
                        out[name].append(rel[len("include/"):])
            HEADERS[:] = [(original, dict(out))]
    return HEADERS[0][1]


def header_of(original, cls):
    found = defining_headers(original).get(cls, [])
    return found[0] if len(found) == 1 else None


def class_layouts(original, cls, regions):
    header = header_of(original, cls)
    if not header:
        return None
    return {r: layouts('#include "%s"\n' % header, r, cls) for r in regions}


def canon(t, lay):
    memo = lay.setdefault("memo", {})
    if t not in memo:
        memo[t] = normalized(t, lay)
    return memo[t]


def prefetch(original, classes, regions, jobs):
    with ThreadPoolExecutor(jobs) as pool:
        got = list(pool.map(lambda cls: (cls, class_layouts(original, cls, regions)), sorted(classes)))
    return {cls: lays for cls, lays in got if lays}


def normalized(t, lay):
    stars = t.count("*")
    base = re.sub(r"\s+", " ", re.sub(r"\*|\b(?:const|volatile|struct|class|union|enum)\b", " ", t)).strip()
    for _ in range(8):
        td = lay["typedefs"].get(base)
        if not td or base in SIZES:
            break
        stars += td.count("*")
        base = re.sub(r"\s+", " ", re.sub(r"\*|\b(?:const|volatile|struct|class|union|enum)\b", " ", td)).strip()
    if base in lay["enums"]:
        base = "int"
    return plausible.norm_type(base + "*" * stars)


def size_of(t, lay):
    if t.endswith("*"):
        return 4
    if t in SIZES:
        return SIZES[t]
    return lay["types"][t]["size"] if t in lay["types"] else None


def is_pad(f):
    return bool(PAD.match(f["name"])) and (f["dims"] > 0 or f["name"].lower().startswith(("pad", "_pad")))


def chain(lay, cls, off, path=""):
    t = lay["types"].get(cls)
    if not t or off < 0 or off >= t["size"]:
        return
    for f in t["fields"]:
        if not f["off"] <= off < f["off"] + max(f["size"], 1):
            continue
        name = path + f["name"]
        if is_pad(f):
            yield "pad", name, f, 0
            continue
        rel = off - f["off"]
        sub = name
        if f["dims"]:
            if rel == 0:
                yield "array", name, f, f["dims"]
            for k, es in enumerate(f["elem"]):
                if es <= 0:
                    return
                idx, rel = divmod(rel, es)
                sub = "%s[%d]" % (sub, idx)
                if k + 1 < f["dims"] and rel == 0:
                    yield "array", sub, f, f["dims"] - k - 1
        if rel == 0:
            yield "value", sub, f, 0
        if f["ptr"] == 0:
            base = canon(f["base"], lay)
            if base in lay["types"]:
                yield from chain(lay, base, rel, sub + ".")


def field_type(f, lay):
    return canon(f["base"] + "*" * f["ptr"], lay)


def signedness(ft, want):
    return ft != want and ft in INTS and want in INTS and SIZES[ft] == SIZES[want]


def fits(ft, want, strict, exact=False):
    if ft == want:
        return ""
    if strict == "address":
        return None
    if signedness(ft, want) and not exact:
        return ""
    if ft.endswith("*") and want.endswith("*"):
        if want == "void*" and not strict:
            return ""
        if ft == "void*":
            return "" if strict else "(%s)" % want
    return None


def resolve(lay, cls, off, mode, want, strict=False, idx=None, exact=False):
    cands = list(chain(lay, cls, off))
    signed = False
    for kind, path, f, rest in cands:
        if kind == "pad":
            continue
        ft = field_type(f, lay)
        if mode == "load" and kind == "value":
            cast = fits(ft, want, strict, exact)
            if cast is not None:
                return ("(%s{v}->%s)" % (cast, path) if cast else "{v}->" + path), None
            signed = signed or signedness(ft, want)
        elif mode == "address":
            if kind == "array" and rest == 1 and (ft == want or want == "void" or
                                                  (want in BYTES and ft in BYTES and not exact)):
                return "{v}->" + path, None
            if kind == "value" and (ft == want or want == "void"):
                return "&{v}->" + path, None
            if want == "void":
                break
        elif mode == "arrow":
            if kind == "value" and ft == want:
                return "{v}->%s." % path, None
            if kind == "array" and rest == 1 and ft == want:
                return "{v}->%s[0]." % path, None
        elif mode == "index" and kind == "array" and rest == 1 and ft == want:
            return "{v}->%s[%s]" % (path, idx), None
        elif mode == "cast":
            if kind == "value" and ft == want:
                return "&{v}->" + path, None
            if kind == "array" and rest == 1 and ft == want:
                return "{v}->" + path, None
    if mode == "cast" and want == cls:
        return "{v}", None
    if signed:
        return None, "signedness"
    if any(c[0] == "pad" for c in cands):
        return None, "pad"
    return None, "type" if cands else "none"


def view(var, raw):
    v = r"(?<![\w.])(?<!->)%s\b" % re.escape(var)
    return r"(?:(?:%s|%s)\s*)%s(?:\(\s*%s\s*\)|%s)" % (BYTECAST, INTCAST, "?" if raw else "", v, v), v


@functools.lru_cache(maxsize=None)
def patterns(var, raw):
    seen, v = view(var, raw)
    addr = r"\(\s*(?P<view>%s)\s*\+\s*(?P<off>%s)\s*\)" % (seen, OFF)
    bare = r"(?:\(\s*%s\s*\)|%s)" % (v, v)
    t = r"\(\s*(?P<type>%s)\s*\*\s*\)" % TYPE
    pats = [("index", r"\(\s*%s\s*(?:%s|%s)\s*\)\s*\[(?P<idx>[^\[\]]+)\]" % (t, addr, bare)),
            ("arrow", r"\(\s*%s\s*(?:%s|%s)\s*\)\s*->" % (t, addr, bare)),
            ("load", r"\*\s*%s\s*%s" % (t, addr)),
            ("loadidx", r"\*\s*\(\s*%s\s*%s\s*\+\s*(?P<idx>%s)\s*\)" % (t, bare, TERM)),
            ("load0", r"\*\s*%s\s*%s(?!\s*(?:[\[+\-.]|->))" % (t, bare)),
            ("address", r"%s\s*%s" % (t, addr)),
            ("cast", r"%s\s*%s(?!\s*(?:[\[+\-.]|->))" % (t, bare)),
            ("byteidx", r"\(\s*(?P<view>%s)\s*\+\s*(?P<off>%s)\s*\)\s*\[\s*(?P<idx>%s)\s*\]" % (seen, OFF, TERM))]
    if raw:
        pats += [("byte", r"%s\s*\[\s*(?P<idx>%s)\s*\]" % (v, TERM)),
                 ("raw", r"(?P<open>\(\s*)?(?P<view>%s)\s*\+\s*(?P<off>%s)(?(open)\s*\)(?!\s*\[)|(?!\s*\)\s*\[))"
                         r"(?!\s*[\w(\[])" % (seen, OFF))]
    return [(kind, re.compile(p)) for kind, p in pats]


def view_elem(text, elem):
    if re.search(r"\bunsigned\b|\bu8\b", text or ""):
        return "unsigned char"
    return "signed char" if re.search(r"\bchar\b|\bs8\b", text or "") else elem


def matches(code, lo, hi, var, raw):
    pats, pos = patterns(var, raw), lo
    while True:
        best = None
        for kind, rx in pats:
            m = rx.search(code, pos, hi)
            if m and (best is None or m.start() < best[1].start()):
                best = (kind, m)
        if best is None:
            return
        yield best
        pos = best[1].end()


def strictness(code, m):
    after, before = code[m.end():m.end() + 4], code[max(0, m.start() - 3):m.start()].rstrip()
    if before.endswith("&") and not before.endswith("&&"):
        return "address"
    if re.match(r"\s*(?:\+\+|--|[-+*/%&|^]=|<<=|>>=)", after) or before.endswith(("++", "--")):
        return "address"
    return "store" if re.match(r"\s*=(?!=)", after) else False


def access(kind, m, elem):
    g = m.groupdict()
    t = re.sub(r"\s+", " ", g["type"]).strip() if g.get("type") else None
    off = g.get("off") or "0"
    if kind in ("load", "load0"):
        return off, "load", t, None
    if kind == "loadidx":
        return "0", "loadidx", t, g["idx"]
    if kind == "index":
        idx = g["idx"].strip()
        return (off, "loadidx", t, idx) if re.fullmatch(OFF, idx) else (off, "index", t, idx)
    if kind in ("arrow", "address", "cast"):
        return off, kind, t, None
    if kind == "byte":
        return g["idx"], "load", elem, None
    if kind == "byteidx":
        return "%s + %s" % (off, g["idx"]), "load", view_elem(g["view"], elem), None
    return off, "address", view_elem(g["view"], elem), None


def place(off, mode, want, extra, lay, region, macros):
    o = value(off, region, macros)
    if o is None:
        return None, mode, "macro"
    if mode != "loadidx":
        return o, mode, None
    size, i = size_of(want, lay), value(extra, region, macros)
    if size is None or i is None:
        return None, mode, "size" if size is None else "macro"
    return o + i * size, "load", None


def rewrite(text, code, lo, hi, var, raw, cls, lays, live, built, elem="signed char", out=None, exact=False):
    edits, misses = [], []
    starts = [0] + [i + 1 for i, c in enumerate(text) if c == "\n"]
    macros = region_macros(text)
    for kind, m in matches(code, lo, hi, var, raw):
        off, mode, t, extra = access(kind, m, elem)
        if mode == "cast" and (t is None or re.sub(r"\b(?:const|struct|class)\b|\s", "", t) in
                               ("void", "char", "unsignedchar", "signedchar", "u8", "s8")):
            continue
        here = live[bisect.bisect_right(starts, m.start()) - 1]
        regions = sorted(here & set(built) or here)
        if not regions:
            continue
        strict = strictness(code, m)
        got, why, where = set(), None, None
        for r in regions:
            lay = lays.get(r) if lays else None
            if not lay or cls not in lay["types"]:
                why = "no layout"
                break
            want = canon(t, lay) if t else "void"
            o, md, why = place(off, mode, want, extra, lay, r, macros)
            where = o if where is None else where
            if why:
                break
            expr, why = resolve(lay, cls, o, md, want, strict, extra, exact)
            if expr is None:
                break
            got.add(expr)
        if mode == "cast" and (why is not None or len(got) != 1):
            continue
        if why is not None or len(got) != 1:
            misses.append({"off": where, "type": t or "", "mode": mode, "why": why or "regions differ",
                           "at": m.start(), "text": text[m.start():m.end()]})
            continue
        edits.append((m.start(), m.end(), next(iter(got)).replace("{v}", out or var)))
    return edits, misses


def aliases(code, lo, hi, var):
    rx = re.compile(r"(?<![\w>.])((?:const\s+)?(?:unsigned\s+|signed\s+)?(?:char|u8|s8))\s*\*\s*([A-Za-z_]\w*)\s*=\s*"
                    r"((?:\(\s*[^()]*\*\s*\)\s*)*)\(?\s*%s\s*\)?\s*;" % re.escape(var))
    out = []
    for m in rx.finditer(code, lo, hi):
        name = m.group(2)
        writes = re.compile(r"(?<![\w.>])%s\s*(?:=(?!=)|[-+*/%%&|^]=|<<=|>>=|\+\+|--)|(?:\+\+|--)\s*%s\b"
                            % (re.escape(name), re.escape(name)))
        if len(list(writes.finditer(code, lo, hi))) == 1:
            out.append((name, view_elem(m.group(1), "signed char"), m.start(), m.end()))
    return out


def unused_alias(text, code, lo, hi, name, start, end, edits):
    for m in re.compile(r"(?<![\w.>])\b%s\b" % re.escape(name)).finditer(code, lo, hi):
        if not start <= m.start() < end and not any(a <= m.start() < b for a, b, _ in edits):
            return None
    a = text.rfind("\n", 0, start) + 1
    b = text.find("\n", end)
    b = len(text) if b < 0 else b + 1
    if text[a:start].strip() or text[end:b].strip():
        return start, end, ""
    return a, b, ""


def alias_edits(text, code, lo, hi, var, cls, lays, live, built, exact=False):
    edits, misses = [], []
    for name, elem, start, end in aliases(code, lo, hi, var):
        e, miss = rewrite(text, code, lo, hi, name, True, cls, lays, live, built, elem, var, exact)
        edits += e
        misses += miss
        gone = unused_alias(text, code, lo, hi, name, start, end, e)
        if gone:
            edits.append(gone)
    return edits, misses


def apply_edits(text, edits):
    for start, end, rep in sorted(edits, reverse=True):
        text = text[:start] + rep + text[end:]
    return text


def include_closure(original, rel):
    headers, edges = mechclean.include_graph(original)
    seen, todo = set(), list(edges.get(rel, ()))
    while todo:
        h = todo.pop()
        if h not in seen:
            seen.add(h)
            todo += edges.get(headers[h], ())
    return seen


def add_include(text, header):
    if re.search(r'#\s*include\s*[<"]%s[>"]' % re.escape(header), text):
        return text
    last = list(re.finditer(r"(?m)^#include[^\n]*\n", text))
    at = last[-1].end() if last else 0
    return text[:at] + '#include "%s"\n' % header + text[at:]


def locally_defined(code, cls):
    return re.search(r"(?m)^\s*(?:typedef\s+)?(?:struct|class|union)\s+%s\s*(?::[^;{]*)?\{" % re.escape(cls),
                     code) is not None


def game_pointer(t, classes):
    t = re.sub(r"\b(?:const|volatile|struct|class)\b|\s", "", t or "")
    return t[:-1] if t.endswith("*") and t.count("*") == 1 and t[:-1] in classes else None


def plan_unit(original, rel, classes):
    text = original[rel]
    code = flat(text)
    if not re.search(r"%s|%s" % (BYTECAST, INTCAST), code):
        return {}
    built = regions_for(rel, text)
    live = line_regions(text, REGIONS)
    src = plausible.Src(text)
    out = {}
    for f in src.funcs:
        try:
            fn = plausible.Fn(src, f)
        except Exception:
            continue
        variants = {}
        for exact in (False, True):
            edits, misses, needs = [], [], set()
            for var, t in fn.types.items():
                cls = game_pointer(t, classes)
                if not cls or locally_defined(code, cls):
                    continue
                lays = class_layouts(original, cls, REGIONS)
                if not lays:
                    continue
                e, miss = rewrite(text, code, f["open"], f["close"], var, False, cls, lays, live, built, exact=exact)
                e2, miss2 = alias_edits(text, code, f["open"], f["close"], var, cls, lays, live, built, exact)
                e, miss = e + e2, miss + miss2
                edits += e
                misses += [dict(m, var=var, cls=cls) for m in miss]
                if e:
                    needs.add(header_of(original, cls))
            variants["exact" if exact else "loose"] = {"edits": edits, "misses": misses, "headers": sorted(needs)}
        if variants["loose"]["edits"]:
            if not variants["exact"]["edits"] or variants["exact"]["edits"] == variants["loose"]["edits"]:
                del variants["exact"]
            out[f["name"]] = variants
    return out


def unit_text(original, rel, plans, chosen):
    picked = [plans[n][v] for n, v in chosen]
    text = apply_edits(original[rel], [e for p in picked for e in p["edits"]])
    reach = include_closure(original, rel)
    for header in sorted({h for p in picked for h in p["headers"]}):
        if header not in reach:
            text = add_include(text, header)
    return text


def same_object(before, after, rel, regions, work):
    path = f"{work}/tu/{rel.replace('/', '_')}"
    os.makedirs(os.path.dirname(path), exist_ok=True)
    for r in regions:
        a = mechclean.signature(before, path, r)
        if a is None:
            continue
        if mechclean.signature(after, path, r) != a:
            return False
    return True


def overlay(original, blobs):
    rows = manifest_rows()
    current = dict(original)
    for rel, (base, why) in rows.items():
        if blobs.get(rel) == base and os.path.exists(f"{WORK}/{rel}"):
            current[rel] = open(f"{WORK}/{rel}", encoding="utf-8").read()
    return current


def manifest_rows():
    rows = {}
    if os.path.exists(MANIFEST):
        for ln in open(MANIFEST, encoding="utf-8"):
            parts = ln.rstrip("\n").split("\t")
            if len(parts) >= 3:
                rows[parts[0]] = (parts[1], parts[2])
    return rows


def record(new, blobs, why):
    rows = manifest_rows()
    for rel, text in sorted(new.items()):
        dest = f"{WORK}/{rel}"
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        with open(dest, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(text)
        old = rows.get(rel)
        whys = old[1].split(",") if old and old[0] == blobs.get(rel, "") else []
        rows[rel] = (blobs.get(rel, ""), ",".join(whys + [why] if why not in whys else whys))
    os.makedirs(WORK, exist_ok=True)
    with open(MANIFEST, "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{rel}\t{base}\t{why}\n" for rel, (base, why) in sorted(rows.items()))


def plan_all(current, classes, jobs):
    units = [rel for rel in current if rel.startswith("src/") and rel.endswith((".c", ".cpp"))]
    plans = {}
    with ThreadPoolExecutor(jobs) as pool:
        for rel, plan in zip(units, pool.map(lambda rel: plan_unit(current, rel, classes), units)):
            if plan:
                plans[rel] = plan
    return plans


def settle(current, plans, jobs, work):
    kept = {rel: [(name, "loose") for name in sorted(plan)] for rel, plan in plans.items()}
    while True:
        new = {rel: unit_text(current, rel, plans[rel], chosen) for rel, chosen in kept.items() if chosen}
        new = {rel: text for rel, text in new.items() if text != current[rel]}
        if not new:
            return {}, kept
        bad, _ = mechclean.verify(current, new, {}, jobs, work, regions=regions_for)
        if not bad:
            return new, kept
        print(f"{len(bad)} unit(s) differ, e.g. {bad[0]}; checking their functions one at a time", flush=True)
        for rel in bad:
            regions = regions_for(rel, current[rel])
            alone = []
            for name, _ in kept.get(rel, []):
                for variant in [v for v in ("loose", "exact") if v in plans[rel][name]]:
                    text = unit_text(current, rel, plans[rel], [(name, variant)])
                    if same_object(current[rel], text, rel, regions, work):
                        alone.append((name, variant))
                        break
            together = unit_text(current, rel, plans[rel], alone)
            kept[rel] = alone if alone and same_object(current[rel], together, rel, regions, work) else []


def header_classes(original, widen):
    found = set(plausible.game_types())
    if widen:
        found |= {name for name, headers in defining_headers(original).items() if len(headers) == 1}
    return found


def main():
    args, jobs, dry, classes, widen = sys.argv[1:], 8, False, None, False
    while args:
        a = args.pop(0)
        if a == "-j" and args:
            jobs = int(args.pop(0))
        elif a == "--dry":
            dry = True
        elif a == "--class" and args:
            classes = (classes or set()) | {args.pop(0)}
        elif a == "--header-classes":
            widen = True
        else:
            sys.exit(USAGE)
    start = time.time()
    blobs, original = mechclean.tree_texts()
    current = overlay(original, blobs)
    classes = classes or header_classes(current, widen)
    plans = plan_all(current, classes, jobs)
    loose = [f["loose"] for p in plans.values() for f in p.values()]
    misses = collections.Counter(m["why"] for f in loose for m in f["misses"])
    print(f"functions with member rewrites: {len(loose)} in {len(plans)} file(s); accesses "
          f"{sum(len(f['edits']) for f in loose)}; left {sum(misses.values())} {dict(misses)} "
          f"({time.time() - start:.0f}s)", flush=True)
    kept, new = {}, {}
    if not dry:
        new, kept = settle(current, plans, jobs, f"{WORK}/offsets")
        record(new, blobs, "offsetmembers")
    os.makedirs(WORK, exist_ok=True)
    with open(f"{WORK}/offsets.tsv", "w", encoding="utf-8", newline="\n") as fh:
        for rel in sorted(plans):
            for name, variants in sorted(plans[rel].items()):
                state = "planned" if dry else dict(kept.get(rel, [])).get(name, "differs")
                fh.write(f"{rel}\t{name}\t{state}\t{len(variants['loose']['edits'])}\t" +
                         ";".join(f"{m['var']}+{'' if m['off'] is None else hex(m['off'])}:{m['type']}:{m['why']}"
                                  for m in variants["loose"]["misses"]) + "\n")
    if not dry:
        print(f"kept {sum(len(v) for v in kept.values())} function(s) in {len(new)} file(s) "
              f"({time.time() - start:.0f}s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
