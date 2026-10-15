import collections
import os
import re
import shutil
import sys

import addrname
import kitpaths
import mechclean
import plausible

REPO, SP = kitpaths.REPO, kitpaths.SP
REQUESTS = f"{SP}/handwork/issues/retype_requests.tsv"
OUT = f"{SP}/handwork/retype"
WORK = f"{SP}/handwork/retype_work"
USAGE = """python retype.py [--requests FILE] [--out DIR] [-j N] [--parse]

Applies the retype requests cleanup workers write when a cast can go only by changing a declaration in
another file: `<usa addr>\\t<request>` rows ($SP/handwork/issues/retype_requests.tsv) naming a function
parameter or return (`Func(param) Type`, `Func Type`, `Func Ret(params)`) or a class member
(`Class::member Type`, `Class::member Type at 0xOFF`).

A parameter is retyped in its definition, every declaration and every call: a cast the old type forced is
dropped, or retargeted to the new type where the argument is another type; a call never gains a cast. A
mangled symbol takes the new mangling in every region's symbols.txt and .s; an extern "C" name stays. A
file that never names the new class gets `struct X;`. A definition whose body still needs the old type
binds it from the new parameter. A member is retyped in its header, or added inside the padding that
covers the offset the request gives, only when sizeof and every other member's offset stay the same in
every region. The new type must be a header class or one the callers already name, never void* or a
byte pointer.

A change is kept only when every translation unit it touches compiles to the same object in every region
it is built for and plausible.py finds nothing new in any file it changes. Rewrites go to --out (default
$SP/handwork/retype) with manifest.tsv in $SP/clean/manifest.tsv's format, and results.tsv:
`<addr>\\tapplied|dropped|unparsed\\t<detail>`. --parse prints how each request reads and stops."""
QUALIFIERS = {"const", "volatile", "struct", "class", "union", "enum"}
ORDINAL = {"first": 1, "second": 2, "third": 3, "fourth": 4, "fifth": 5, "sixth": 6}
TYPE = re.compile(r"\s*((?:(?:const|volatile|struct|class|union|enum|unsigned|signed)\s+)*"
                  r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*(?:\s+(?:int|char|short|long|const)\b)*"
                  r"(?:\s*\(\s*\*\s*\)\s*\([^()]*\))?(?:\s*[*&](?:\s*const\b)?)*(?:\s*\[[^\]]*\])*)")
TARGET = re.compile(r"\s*([A-Za-z_]\w*)(?:::(\([^()]*\)|<[^<>]*>|\w+(?:\.\w+)*(?:\[[^\]]*\])?))?")
FORWARD = re.compile(r"\b(?:struct|class|union|enum)\s+([A-Za-z_]\w*)\s*;")
TYPEDEF = re.compile(r"\btypedef\b[^;{}]*?\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*;")
TYPEDEF_BODY = re.compile(r"\btypedef\s+(?:struct|union|enum)\b[^;{}]*\{(?:[^{}]|\{[^{}]*\})*\}\s*([A-Za-z_]\w*)\s*;")
ALIAS = re.compile(r"\btypedef\s+(?:(?:struct|class|union|enum)\s+)?((?:(?:const|volatile|unsigned|signed)\s+)*"
                   r"[A-Za-z_]\w*(?:::\w+)*(?:\s+(?:int|char|short|long))*)\s*(\**)\s*([A-Za-z_]\w*)\s*;")
TAGGED = re.compile(r"\btypedef\s+(?:struct|union|enum)\s+([A-Za-z_]\w*)\s*(?::[^{;]*)?\{(?:[^{}]|\{[^{}]*\})*\}\s*"
                    r"([A-Za-z_]\w*)\s*;")
OFFSET_NAME = re.compile(r"(?:unknown|unk|field)_?([0-9a-fA-F]+)_?")
PLACED = re.compile(r"\s*\(?\s*(?:an?\s+\w+\s+)?(?:new\s+)?(?:member\s+at|offset|at)\s+(?:usa\s+|USA\s+)?\+?(0x[0-9a-fA-F]+)")
VOIDS = {"void*", "void**"}
INTS = {"int", "unsignedint", "long", "unsignedlong"}
BYTES = {t.replace("const", "") for t in plausible.BYTE_POINTER}
DECL = (r"(?m)(^|[;{}])([ \t]*(?:extern\s+\"C\"\s+)?(?:(?:static|inline|ARM|THUMB|extern)\s+)*)"
        r"((?:(?:const|volatile|struct|class|union|enum|unsigned|signed)\s+)*[A-Za-z_]\w*(?:::\w+)*"
        r"(?:\s+(?:int|char|short|long))*[\s*&]*?)\s*\b(%s)\s*\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)(\s*const)?"
        r"(?=\s*([;{]))")
NOT_TYPES = {"return", "else", "case", "goto", "new", "delete", "throw", "sizeof", "do"}


class Drop(Exception):
    pass


def balanced(s, i):
    depth = 0
    for j in range(i, len(s)):
        depth += {"(": 1, ")": -1}.get(s[j], 0)
        if depth == 0:
            return j + 1
    return None


def top_split(s, sep):
    out, depth, last = [], 0, 0
    for m in re.finditer(r"[(){}]|%s" % sep, s):
        if m.group(0) in "({":
            depth += 1
        elif m.group(0) in ")}":
            depth -= 1
        elif depth == 0:
            out.append(s[last:m.start()])
            last = m.end()
    return out + [s[last:]]


def known_type(word, known):
    return word in known or ("::" in word and all(p in known for p in word.split("::")))


def type_words(t):
    return [w for w in re.findall(r"[A-Za-z_]\w*(?:::\w+)*", re.sub(r"\(\s*\*\s*\)", "", t))
            if w not in QUALIFIERS and w not in mechclean.BUILTIN_WORDS]


def read_type(s, known, unknown=None):
    m = TYPE.match(s)
    t = m.group(1).strip() if m else ""
    words = [w for w in re.findall(r"[A-Za-z_]\w*(?:::\w+)*", re.sub(r"\[[^\]]*\]", "", t)) if w not in QUALIFIERS]
    if not words or any(not known_type(w, known) for w in words):
        if unknown is not None:
            unknown.extend(w for w in words if not known_type(w, known))
        return None
    return re.sub(r"\s+", " ", t), m.end()


def read_params(inner, known):
    if inner.strip() in ("", "void"):
        return []
    out = []
    for p in mechclean.split_args(inner):
        r = read_type(p, known)
        if not r or not re.fullmatch(r"\s*(?:[A-Za-z_]\w*)?\s*", p[r[1]:]):
            return None
        out.append((r[0], p[r[1]:].strip() or None))
    return out


def read_spec(spec, known):
    m = re.fullmatch(r"\s*param(?:eter)?\s+(\d+)(?:\s+([A-Za-z_]\w*))?\s*", spec)
    if m:
        return {"index": [int(m.group(1)) - 1], "names": [m.group(2)] if m.group(2) else []}
    items = [p.strip() for p in mechclean.split_args(spec)]
    if all(re.fullmatch(r"[a-z_]\w*", p) and not known_type(p, known) for p in items):
        return {"names": items}
    ps = read_params(spec, known)
    if ps is None:
        raise ValueError("cannot read (%s)" % spec.strip())
    if any(n for _, n in ps):
        return {"sig": ps}
    return {"olds": [t for t, _ in ps]}


def is_target(g, i, funcs, known):
    m = TARGET.match(g, i)
    return m is not None and (m.group(1) in funcs or (m.group(2) is not None and known_type(m.group(1), known)))


def split_and(part, funcs, known):
    out, last = [], 0
    for piece in top_split(part, r"\s+and\s+")[:-1]:
        at = part.index(piece, last) + len(piece)
        m = re.compile(r"\s+and\s+").match(part, at)
        if m and is_target(part, m.end(), funcs, known):
            out.append(part[last:at])
            last = m.end()
    return out + [part[last:]]


def parse_targets(g, funcs, known):
    targets, i = [], 0
    while True:
        m = TARGET.match(g, i)
        if not m:
            break
        name, member, j = m.group(1), m.group(2), m.end()
        if member is not None and name in funcs and re.fullmatch(r"\w+", member):
            targets.append({"kind": "fn", "name": name, "names": [member]})
        elif member is not None and name not in funcs and known_type(name, known):
            targets.append({"kind": "member", "cls": name, "member": member})
        elif member is None and name in funcs:
            t = {"kind": "fn", "name": name}
            if g[j:j + 1] == "(":
                end = balanced(g, j)
                if end is None:
                    raise ValueError("unbalanced parenthesis after %s" % name)
                t.update(read_spec(g[j + 1:end - 1], known))
                j = end
            targets.append(t)
        else:
            break
        i = j
        while True:
            s = re.compile(r"\s*([,+/])\s*").match(g, i)
            if not s:
                return targets, i
            if is_target(g, s.end(), funcs, known):
                i = s.end()
                break
            w = re.compile(r"(\w+(?:\.\w+)*)\b(?!\s*\()").match(g, s.end())
            if s.group(1) == "/" and targets[-1]["kind"] == "member" and w:
                targets.append({"kind": "member", "cls": targets[-1]["cls"], "member": w.group(1)})
                i = w.end()
                continue
            return targets, i
    return targets, i


def parse_group(g, funcs, known):
    targets, i = parse_targets(g, funcs, known)
    if not targets:
        raise ValueError("names no known function or Class::member")
    fns = all(t["kind"] == "fn" for t in targets)
    rest, prose, sig, ret, typ, pname, offset = g[i:].strip(), [], None, None, None, None, None
    while rest.startswith("("):
        end = balanced(rest, 0)
        if end is None:
            prose.append(rest)
            rest = ""
            break
        ps = read_params(rest[1:end - 1], known) if fns else None
        if ps is not None:
            sig, rest = ps, rest[end:].strip()
            break
        prose.append(rest[1:end - 1])
        rest = rest[end:].strip()
    m = re.match(r"parameter\s+(?:([A-Za-z_]\w*)\s+should\s+be\s+)?", rest)
    if m:
        pname, rest = m.group(1), rest[m.end():]
    unknown = []
    r = read_type(rest, known, unknown) if rest and sig is None else None
    if r:
        typ, rest = r[0], rest[r[1]:]
        m = re.match(r"\s*(?:([A-Za-z_]\w*)\s*)?\(", rest)
        if fns and m and (m.group(1) is None or any(t["name"] == m.group(1) for t in targets)):
            end = balanced(rest, m.end() - 1)
            ps = read_params(rest[m.end():end - 1], known) if end else None
            if ps is not None:
                sig, ret, typ, rest = ps, typ, None, rest[end:]
        m = re.match(r"\s+([a-z_]\w*)(?=\s*(?:$|[(,;.]))", rest)
        if m and m.group(1) != "at":
            pname, rest = m.group(1), rest[m.end():]
        m = re.match(r"\s+at\s+(?:usa\s+|USA\s+)?\+?(0x[0-9a-fA-F]+)", rest)
        if m:
            offset, rest = int(m.group(1), 16), rest[m.end():]
        if rest.strip() and not re.match(r"\s*[(,.;:]", rest):
            raise ValueError("prose follows the type: %s" % rest.strip()[:40])
        prose.append(rest)
    elif sig is None and not (fns and all(t.get("sig") for t in targets)):
        raise ValueError("no type" if not unknown or not re.match(r"[A-Z]", unknown[0]) else
                         "%s is declared nowhere" % unknown[0])
    text = " ".join(prose)
    out = []
    for t in targets:
        if t["kind"] == "member":
            if typ is None:
                raise ValueError("a member needs a type")
            spec = t["member"][1:-1] if t["member"][0] in "(<" else None
            where = [PLACED.match(p) for p in [spec or ""] + prose]
            where = [int(w.group(1), 16) for w in where if w]
            found = offset if offset is not None else where[0] if where else None
            out.append({"kind": "member", "cls": t["cls"], "member": None if spec else t["member"],
                        "type": typ, "offset": found})
            continue
        c = {"kind": "fn", "name": t["name"], "names": list(t.get("names", [])), "olds": t.get("olds", []),
             "index": list(t.get("index", [])), "sig": t.get("sig") or sig, "ret": ret, "type": typ,
             "all": re.search(r"\b(?:both|all)\s+pointer\s+param", text) is not None}
        if pname:
            c["names"].append(pname)
        m = re.search(r"\b(first|second|third|fourth|fifth|sixth|(\d+)(?:st|nd|rd|th))\s+param", text)
        if m and not c["index"]:
            c["index"].append((int(m.group(2)) if m.group(2) else ORDINAL[m.group(1)]) - 1)
        if c["sig"] is not None and t.get("sig") and typ is not None:
            c["ret"] = typ
        if c["sig"] is None and typ is None:
            raise ValueError("no type for %s" % t["name"])
        out.append(c)
    return out


def parse_request(text, funcs, known):
    head = re.split(r"\s+[—–]\s+", text.replace("`", ""), maxsplit=1)[0]
    out = []
    for part in top_split(head, ";"):
        for g in split_and(part, funcs, known):
            if g.strip():
                try:
                    out.append((g.strip(), parse_group(g, funcs, known)))
                except ValueError as e:
                    out.append((g.strip(), str(e)))
    return out


def pointer(t):
    return "*" in mechclean.norm_type(t) or "&" in mechclean.norm_type(t)


def strip_dims(t):
    m = re.match(r"^(.*?)\s*((?:\[[^\]]*\]\s*)*)$", t)
    return m.group(1).strip(), re.sub(r"\s+", "", m.group(2))


class Fn:
    kind = "fn"

    def __init__(self, **kw):
        self.__dict__.update(kw)

    @property
    def info(self):
        return self.new, self.qual, self.base, self.real, self.params

    @property
    def words(self):
        return [w for t in list(self.params) + [self.newret or ""] for w in type_words(t)]


class Member:
    kind = "member"

    def __init__(self, **kw):
        self.__dict__.update(kw)

    words = ()


def postfix_start(text, at):
    j = at
    while j > 0:
        k = j
        while k > 0 and text[k - 1].isspace():
            k -= 1
        if k > 0 and text[k - 1] in ")]":
            depth, k = 0, k - 1
            while k >= 0:
                depth += {")": 1, "]": 1, "(": -1, "[": -1}.get(text[k], 0)
                if depth == 0:
                    break
                k -= 1
            j = k
            continue
        m = re.search(r"[A-Za-z_]\w*$", text[:k])
        if not m or m.group(0) in plausible.KEYWORDS:
            return j
        j = m.start()
        p = re.search(r"(?:->|\.|::)\s*$", text[:j])
        if not p:
            return j
        j = p.start()
    return j


C_CAST_BEFORE = re.compile(r"\(\s*((?:const\s+)?(?:struct\s+|class\s+)?(?:unsigned\s+|signed\s+)?[A-Za-z_]\w*(?:::\w+)*"
                           r"(?:\s+[A-Za-z_]\w*)?\s*\**)\s*\)\s*$")
CPP_CAST_BEFORE = re.compile(r"(?:reinterpret|static)_cast\s*<\s*([^<>]+?)\s*>\s*\(\s*$")


def grouping(prefix):
    if re.search(r"_cast\s*<[^<>]*>\s*$", prefix):
        return False
    return not re.search(r"[\w)\]]\s*$", prefix) or C_CAST_BEFORE.search(prefix) is not None


def enclosed(text, start, end):
    while start > 0 and end < len(text) and text[start - 1] == "(" and text[end] == ")" and grouping(text[:start - 1]):
        start, end = start - 1, end + 1
    return start, end


def tidy(text, start, end):
    while True:
        inner = text[start:end]
        if mechclean.wrapped(inner) and mechclean.plain_expr(inner[1:-1].strip()):
            text, end = text[:start] + inner[1:-1].strip() + text[end:], start + len(inner[1:-1].strip())
        elif start > 0 and end < len(text) and text[start - 1] == "(" and text[end] == ")" and \
                grouping(text[:start - 1]) and mechclean.plain_expr(inner):
            text, start, end = text[:start - 1] + inner + text[end + 1:], start - 1, end - 1
        else:
            return text, start, end


def drop_cast_around(text, start, end, types):
    start, end = enclosed(text, start, end)
    tail = text[end:]
    m = CPP_CAST_BEFORE.search(text[:start])
    if m and addrname.unconst(m.group(1)) in types and re.match(r"\s*\)", tail):
        close = end + re.match(r"\s*\)", tail).end()
        return tidy(text[:m.start()] + text[start:end] + text[close:], m.start(), m.start() + end - start)
    m = C_CAST_BEFORE.search(text[:start])
    if m and addrname.unconst(m.group(1)) in types and not re.match(r"\s*(?:->|\.|\[|\()", tail):
        return tidy(text[:m.start()] + text[start:end] + tail, m.start(), m.start() + end - start)
    return None


def address_of(text, start, end):
    deref = re.search(r"(?:[(,=!&|?:;{}]|^|\breturn)\s*(\*)\s*$", text[:start])
    if deref:
        return tidy(text[:deref.start(1)] + text[start:end] + text[end:], deref.start(1),
                    deref.start(1) + end - start)[0]
    wrap = re.match(r"\s*(?:->|\.|\[|\()", text[end:]) is not None
    return text[:start] + ("(&" if wrap else "&") + text[start:end] + (")" if wrap else "") + text[end:]


def split_pad(pad, lo, hi, offset, width, decl, indent):
    spans = re.fullmatch(r"\[\s*0x([0-9a-fA-F]+)\s*-\s*0x([0-9a-fA-F]+)\s*\]", pad["dims"])
    relative = spans and int(spans.group(2), 16) == lo and int(spans.group(1), 16) == hi
    length = lambda a, b: "[0x%x - 0x%x]" % (b, a) if relative else "[0x%x]" % (b - a)
    tail, hexed = offset + width, "%x" % lo
    at = pad["name"].lower().rfind(hexed)
    tail_name = pad["name"][:at] + "%x" % tail + pad["name"][at + len(hexed):] if at >= 0 else \
        "%s_%x" % (pad["name"], tail)
    parts = ["%s %s%s" % (pad["type"], pad["name"], length(lo, offset))] if offset > lo else []
    parts.append(decl)
    if tail < hi:
        parts.append("%s %s%s" % (pad["type"], tail_name, length(tail, hi)))
    return (";\n" + indent).join(parts)


def member_reads(new):
    base, dims = strip_dims(new)
    if dims:
        return {addrname.unconst(base + "*")}, set()
    if pointer(base) or not type_words(base):
        return {addrname.unconst(base)}, set()
    return set(), {addrname.unconst(base + "*")}


def member_assigns(text, name, old):
    out = []
    for m in re.finditer(r"(?:->|\.)\s*%s\b(?!\s*\()" % re.escape(name), text):
        rhs = re.compile(r"\s*=(?!=)\s*([^;]+);").match(text, m.end())
        cast = addrname.cast_parts(rhs.group(1)) if rhs else None
        if cast and addrname.unconst(cast[0]) == addrname.unconst(old):
            out.append((rhs.start(1), rhs.end(1), cast[1], addrname.CPP_CAST.fullmatch(rhs.group(1).strip()) is not None))
    return out


def member_uncast(text, name, new, old, retarget=()):
    sites = member_assigns(text, name, old)
    for k, (start, end, inner, cpp) in reversed(list(enumerate(sites))):
        inner = inner.strip()
        bare = inner[1:-1].strip() if mechclean.wrapped(inner) else inner
        if k in retarget:
            bare = "static_cast<%s>(%s)" % (new, bare) if cpp else \
                "(%s)%s" % (new, bare if mechclean.plain_expr(bare) else "(%s)" % bare)
        text = text[:start] + bare + text[end:]
    same, address = member_reads(new)
    for m in reversed(list(re.finditer(r"(?:->|\.)\s*%s\b(?!\s*\()" % re.escape(name), text))):
        start, end = postfix_start(text, m.start()), m.end()
        done = drop_cast_around(text, start, end, same)
        if done:
            text = done[0]
            continue
        done = drop_cast_around(text, start, end, address) if address else None
        if done:
            text = address_of(*done)
    return text


class Plan:
    def __init__(self, jobs, work):
        self.jobs, self.work = jobs, work
        self.blobs, self.original = mechclean.tree_texts()
        self.every = addrname.symbol_idents()
        self.funcs = set(self.every)
        self.known, self.header_types = set(mechclean.BUILTIN_WORDS) | set(plausible.SCALAR_TYPEDEFS), set()
        self.defined_in, aliases = {}, collections.defaultdict(set)
        for rel, text in self.original.items():
            if not rel.endswith(addrname.SOURCES):
                continue
            defined = {m.group(1) for m in plausible.TYPE_DEF.finditer(text)} | set(TYPEDEF.findall(text)) | \
                {m.group(1) for m in TYPEDEF_BODY.finditer(text)}
            self.known |= defined | set(FORWARD.findall(text))
            for m in ALIAS.finditer(text):
                aliases[m.group(3)].add(re.sub(r"\s+", " ", m.group(1)) + m.group(2))
            for m in TAGGED.finditer(text):
                aliases[m.group(2)].add(m.group(1))
            if rel.startswith("include/"):
                self.header_types |= defined
                for name in defined:
                    self.defined_in.setdefault(name, rel[len("include/"):])
        self.aliases = {n: next(iter(t)) for n, t in aliases.items() if len(t) == 1 and next(iter(t)) != n}
        shutil.rmtree(f"{work}/layout", ignore_errors=True)
        self.by_addr = {}
        for rel, text in self.original.items():
            if rel.endswith((".c", ".cpp")):
                for a in plausible.USA_ADDR.findall(text):
                    self.by_addr.setdefault(a.lower(), rel)
        self.choices, self.touch, self.headers_tus, self.closure = {}, {}, {}, {}
        self.state = mechclean.include_state(f"{REPO}/include")

    def mentioning(self, word):
        pattern = re.compile(r"\b%s\b" % re.escape(word))
        return [rel for rel, text in self.original.items() if word in text and pattern.search(text)
                and rel.endswith(addrname.SOURCES)]

    def decls(self, text, qual, base):
        q = "(?:%s::)?" % re.escape(qual) if qual else ""
        return [m for m in re.finditer(DECL % (q + re.escape(base)), text)
                if not set(m.group(3).split()) & NOT_TYPES]

    def real_class(self, word, users):
        if word in self.header_types or word in plausible.SCALAR_TYPEDEFS or mechclean.class_header(word):
            return True
        pattern = re.compile(r"\b%s\b" % re.escape(word.split("::")[0]))
        return any(pattern.search(self.original[rel]) for rel in users)

    def check_type(self, t, users, what):
        n = addrname.unconst(t)
        if n in VOIDS:
            raise Drop("%s would be void*" % what)
        if n in BYTES:
            raise Drop("%s would be a byte pointer (%s)" % (what, t))
        for w in type_words(t):
            if not self.real_class(w, users):
                raise Drop("%s: %s is neither a header class nor named where it is used" % (what, w))

    def vague(self, t):
        n = mechclean.norm_type(t).replace("const", "")
        return n in VOIDS or n in BYTES or n in INTS or (pointer(t) and not type_words(t)) or any(
            w not in self.header_types and not mechclean.class_header(w) for w in type_words(t))

    def canon(self, t):
        for _ in range(8):
            done = re.sub(r"\b[A-Za-z_]\w*\b", lambda m: self.aliases.get(m.group(0), m.group(0)), t)
            if done == t:
                break
            t = done
        return t

    def position(self, typ, real, ret, base, defs, row):
        other = lambda t: mechclean.norm_type(t) != mechclean.norm_type(typ) and self.vague(t)
        slots = [i for i, t in enumerate(real) if pointer(t) and other(t)] + \
            (["ret"] if ret and pointer(ret) and other(ret) else [])
        if not pointer(typ):
            slots = [i for i, t in enumerate(real) if not pointer(t) and other(t)]
        elif not slots:
            slots = [i for i, t in enumerate(real) if mechclean.norm_type(t) in INTS] + \
                (["ret"] if ret and mechclean.norm_type(ret) in INTS else [])
        if len(slots) > 1 and row in self.by_addr:
            text, votes = self.original[self.by_addr[row]], set()
            for start, end, args in mechclean.call_sites(text, base):
                if drop_cast_around(text, start, end, {addrname.unconst(typ)}):
                    votes.add("ret")
                votes |= {i for i, a in enumerate(args) if addrname.cast_parts(a)}
            for rel, m in defs:
                if rel == self.by_addr[row]:
                    params = mechclean.split_args(m.group(5))
                    body = text[m.end():self.close_brace(text, text.index("{", m.end()))]
                    votes |= {i for i, p in enumerate(params) if mechclean.param_name(p) and re.search(
                        r"(?:\)\s*|_cast\s*<[^<>]*>\s*\(\s*)%s\b" % re.escape(mechclean.param_name(p)), body)}
            if len([s for s in slots if s in votes]) == 1:
                slots = [s for s in slots if s in votes]
        if len(slots) != 1:
            raise Drop("%s: %s which to retype" % (base, "nothing says" if not slots else
                                                    "cannot tell (%s)" % ", ".join(map(str, slots))))
        return slots

    def resolve_fn(self, c, row):
        syms = self.every.get(c["name"], set())
        if len(syms) != 1:
            raise Drop("%s names %d symbols" % (c["name"], len(syms)))
        symbol = next(iter(syms))
        parsed = mechclean.demangle(symbol) if symbol.startswith("_Z") else None
        if symbol.startswith("_Z") and not parsed:
            raise Drop("cannot demangle %s" % symbol)
        full = parsed[0] if parsed else symbol
        qual, base = full.rsplit("::", 1) if "::" in full else (None, full)
        users = self.mentioning(base)
        found = [(rel, m) for rel in users if rel.endswith((".c", ".cpp", ".h"))
                 for m in self.decls(self.original[rel], qual, base)]
        defs = [(rel, m) for rel, m in found if m.group(7) == "{" and rel.startswith("src/")]
        if len(defs) > 1:
            raise Drop("%s has %d definitions" % (base, len(defs)))
        first = defs[0] if defs else found[0] if found else None
        declared = [p.strip() for p in mechclean.split_args(first[1].group(5))] \
            if first and first[1].group(5).strip() not in ("", "void") else []
        real = list(parsed[1]) if parsed else [mechclean.param_type(p) for p in declared]
        if not parsed and first is None:
            raise Drop("no declaration of %s" % base)
        names = [mechclean.param_name(p) for p in declared] if len(declared) == len(real) else [None] * len(real)
        ret = first[1].group(3).strip() if first else None
        params, newret = list(real), None
        if c["sig"] is not None:
            params = [t for t, _ in c["sig"]]
            newret = c["ret"]
        else:
            at = [names.index(n) for n in c["names"] if n in names] + list(c["index"])
            at += [i for i, t in enumerate(real) for o in c["olds"] if mechclean.norm_type(t) == mechclean.norm_type(o)]
            if c["all"]:
                at += [i for i, t in enumerate(real) if pointer(t)]
            missing = [n for n in c["names"] if n not in names]
            if missing:
                raise Drop("%s has no parameter %s" % (base, ", ".join(missing)))
            if c["olds"] and not any(mechclean.norm_type(t) == mechclean.norm_type(o) for t in real for o in c["olds"]):
                raise Drop("%s has no %s parameter" % (base, ", ".join(c["olds"])))
            if not at:
                at = self.position(c["type"], real, ret, base, defs, row)
            for i in at:
                if i == "ret":
                    newret = c["type"]
                elif i >= len(real):
                    raise Drop("%s has no parameter %d" % (base, i + 1))
                else:
                    params[i] = c["type"]
        params = [t if "(" in t else mechclean.param_type(t + " x") for t in params]
        if newret is not None:
            newret = mechclean.param_type(newret + " x")
            if ret is None:
                raise Drop("no declaration of %s to give a return type" % base)
        changed = [t for i, t in enumerate(params) if i >= len(real) or mechclean.norm_type(t) != mechclean.norm_type(real[i])]
        if newret is not None and mechclean.norm_type(newret) != mechclean.norm_type(ret):
            changed.append(newret)
        else:
            newret = None
        if not changed:
            raise Drop("%s already has that signature" % base)
        for t in changed:
            self.check_type(t, users, base)
        new = symbol
        if parsed:
            new = addrname.probe_name(qual, base, [self.canon(t) for t in params], parsed[2])
            if not new:
                raise Drop("cannot mangle %s(%s)" % (base, ", ".join(params)))
            if new != symbol and any(new in self.original[rel] for rel in self.original if rel.endswith("symbols.txt")):
                raise Drop("%s is already bound" % new)
        defrel, olddecl = (defs[0][0], declared) if defs else (None, None)
        wanted = [n for _, n in c["sig"]] if c["sig"] is not None else [None] * len(params)
        return Fn(key=symbol, symbol=symbol, qual=qual, base=base, real=real, params=params, ret=ret, newret=newret,
                  new=new, const=bool(parsed and parsed[2]), defrel=defrel, olddecl=olddecl, users=users, rows={row},
                  wanted=wanted)

    def scope(self, text, cls, path):
        code = plausible.strip_code(text)
        body = mechclean.class_body(code, cls)
        if not body:
            raise Drop("cannot find the body of %s" % cls)
        start, end, prefix = body[0], body[1], ""
        for part in path[:-1]:
            members = [m for m in self.members(code, start, end, prefix) if m["name"] == part and m.get("body")]
            if not members:
                raise Drop("%s has no inline aggregate %s" % (cls, part))
            start, end = members[0]["body"]
            prefix += part + "."
        return code, start, end, prefix

    def members(self, code, start, end, prefix):
        out, depth, stmt, i = [], 0, start, start
        while i < end:
            ch = code[i]
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0 and "(" in code[stmt:code.index("{", stmt)] and not re.match(r"\s*;", code[i + 1:]):
                    stmt = i + 1
            elif ch == ";" and depth == 0:
                out += self.declaration(code, stmt, i, prefix)
                stmt = i + 1
            i += 1
        return out

    def declaration(self, code, s, e, prefix):
        decl = code[s:e]
        lead = re.match(r"(?:\s*(?:public|private|protected)\s*:(?!:))*\s*", decl)
        s += lead.end()
        decl = code[s:e]
        if not decl or re.match(r"(?:typedef|static|friend|using|enum|template)\b", decl):
            return []
        if "{" in decl:
            agg = re.match(r"(?:struct|union|class)\s*(\w+)?\s*(?::[^{]*)?\{", decl)
            if not agg or "(" in decl[:decl.index("{")]:
                return []
            open_at = s + decl.index("{")
            close = self.close_brace(code, open_at)
            after = code[close + 1:e].strip()
            if not after:
                return [] if agg.group(1) else self.members(code, open_at + 1, close, prefix)
            m = re.fullmatch(r"([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*)", after)
            if not m:
                return []
            return [{"name": m.group(1), "path": prefix + m.group(1), "type": None, "body": (open_at + 1, close),
                     "start": s, "end": e, "bitfield": False}]
        if "(" in decl and not re.search(r"\(\s*\*\s*\w+\s*\)", decl):
            return []
        m = re.match(r"(.*?)([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*)(?::\s*\w+)?\s*$", decl, re.S)
        if not m or not m.group(1).strip():
            return []
        return [{"name": m.group(2), "path": prefix + m.group(2), "type": m.group(1).strip(), "dims": m.group(3).strip(),
                 "start": s, "end": e, "bitfield": ":" in decl[m.end(3):], "body": None}]

    @staticmethod
    def close_brace(code, open_at):
        depth = 0
        for j in range(open_at, len(code)):
            depth += {"{": 1, "}": -1}.get(code[j], 0)
            if depth == 0:
                return j
        return len(code)

    def probe(self, header, cls, paths, region, include):
        lines = ['#include <globaldefs.h>\n#define private public\n#define protected public\n#include "%s"\n' % header,
                 "char probe_size[sizeof(%s) + 1];\n" % cls]
        for k, p in enumerate(paths):
            lines.append("char probe_off_%d[(unsigned int)&((%s*)0)->%s + 1];\n" % (k, cls, p))
            lines.append("char probe_len_%d[sizeof(((%s*)0)->%s) + 1];\n" % (k, cls, p))
        os.makedirs(f"{self.work}/layout", exist_ok=True)
        sig = mechclean.signature("".join(lines), f"{self.work}/layout/probe.cpp", region, include)
        if sig is None:
            return None
        return {s[0]: s[2] - 1 for s in sig[0] if s[0].startswith("probe_")}

    def layout_include(self, rel, text):
        root = f"{self.work}/layout/include"
        if not os.path.isdir(root):
            shutil.copytree(f"{REPO}/include", root)
        with open(f"{self.work}/layout/{rel}", "w", encoding="utf-8", newline="\n") as fh:
            fh.write(text)
        return root

    def layout_same(self, rel, text, cls, paths, extra=None):
        header = rel[len("include/"):]
        try:
            for region in ("usa", "jpn", "eur"):
                before = self.probe(header, cls, paths, region, f"{REPO}/include")
                keep = paths
                if before is None:
                    keep = []
                    before = self.probe(header, cls, [], region, f"{REPO}/include")
                if before is None:
                    continue
                root = self.layout_include(rel, text)
                after = self.probe(header, cls, keep + ([extra[0]] if extra and region == "usa" else []), region, root)
                if after is None:
                    return "the header no longer compiles in %s" % region
                if extra and region == "usa":
                    k = len(keep)
                    if after.get("probe_off_%d" % k) != extra[1]:
                        return "%s lands at 0x%x, not 0x%x" % (extra[0], after.get("probe_off_%d" % k, -1), extra[1])
                    after = {n: v for n, v in after.items() if n not in ("probe_off_%d" % k, "probe_len_%d" % k)}
                if before != after:
                    moved = sorted(n for n in before if before[n] != after.get(n))
                    return "the layout of %s changes in %s (%s)" % (cls, region, ", ".join(moved[:3]))
            return None
        finally:
            with open(f"{self.work}/layout/{rel}", "w", encoding="utf-8", newline="\n") as fh:
                fh.write(self.original[rel])

    def twins(self, text, cls, member):
        out = []
        for m in re.finditer(r"(?m)^\s*(?:struct|class)\s+%s\b[^;{]*\{" % re.escape(cls.split("::")[-1]), text):
            close = self.close_brace(text, m.end() - 1)
            for d in re.finditer(r"(?m)^[ \t]*([^;{}()\n]*?)\b%s\s*((?:\[[^\]]*\]\s*)*);" % re.escape(member["name"]),
                                 text[m.end():close]):
                start = m.end() + d.start(1)
                if start != member["start"] and d.group(2).replace(" ", "") == member["dims"].replace(" ", "") and \
                        mechclean.norm_type(d.group(1) + " x") == mechclean.norm_type(member["type"] + " x"):
                    out.append((start, m.end() + d.end() - 1))
        return out

    def resolve_member(self, c, row):
        header = mechclean.class_header(c["cls"])
        if not header:
            raise Drop("no header defines %s" % c["cls"])
        rel = f"include/{header}"
        text = self.original[rel]
        typ, dims = strip_dims(mechclean.param_type(c["type"] + " x") if not re.search(r"\[", c["type"]) else c["type"])
        path = c["member"].split(".") if c["member"] else [None]
        if path[-1] and "[" in path[-1]:
            raise Drop("%s::%s is a range of a member, not a member" % (c["cls"], c["member"]))
        code, start, end, prefix = self.scope(text, c["cls"], path)
        members = self.members(code, start, end, prefix)
        found = [m for m in members if m["name"] == path[-1]]
        for w in type_words(typ):
            if not self.real_class(w, ()):
                raise Drop("%s has no header definition" % w)
        n = addrname.unconst(typ + dims)
        if n in VOIDS or n in BYTES:
            raise Drop("%s::%s would be %s" % (c["cls"], path[-1], typ + dims))
        plain = [m for m in members if not m["bitfield"] and m["type"] is not None]
        if found:
            m = found[0]
            if m["type"] is None:
                raise Drop("%s::%s is an inline aggregate" % (c["cls"], path[-1]))
            if mechclean.norm_type(m["type"] + " x") == mechclean.norm_type(typ + " x") and m["dims"].replace(" ", "") == dims:
                raise Drop("%s::%s already has that type" % (c["cls"], path[-1]))
            spans = [(m["start"], m["end"])] + (self.twins(text, c["cls"], m) if len(path) == 1 else [])
            t = Member(key=("member", c["cls"], m["path"]), rel=rel,
                       edits=[(a, b, "%s %s%s" % (typ, m["name"], dims)) for a, b in spans], words=type_words(typ),
                       name=m["name"], new=typ + dims, old=m["type"] + m["dims"], rows={row}, cls=c["cls"], retyped=True)
            why = self.layout_same(rel, self.file_text(rel, {t.key: t}, set()), c["cls"], [p["path"] for p in plain])
            if why:
                raise Drop(why)
            return t
        offset = c["offset"]
        if offset is None and path[-1]:
            m = OFFSET_NAME.fullmatch(path[-1])
            offset = int(m.group(1), 16) if m else None
        if offset is None:
            raise Drop("%s has no member %s and the request gives no offset" % (c["cls"], path[-1]))
        name = path[-1] or "unknown_%x_" % offset
        header_name = rel[len("include/"):]
        paths = [p["path"] for p in plain]
        size = self.probe(header_name, c["cls"], paths, "usa", f"{REPO}/include")
        if size is None:
            raise Drop("cannot compile %s's layout" % c["cls"])
        width = self.type_size(header_name, typ + dims)
        if width is None:
            raise Drop("cannot size %s" % (typ + dims))
        pads = [(k, p) for k, p in enumerate(plain) if re.fullmatch(r"(?:unsigned\s+|signed\s+)?char", p["type"])
                and p["dims"] and size["probe_off_%d" % k] <= offset and
                offset + width <= size["probe_off_%d" % k] + size["probe_len_%d" % k]]
        if not pads:
            if offset >= size["probe_size"]:
                raise Drop("0x%x is past the end of %s (0x%x bytes)" % (offset, c["cls"], size["probe_size"]))
            raise Drop("no padding of %s covers 0x%x..0x%x" % (c["cls"], offset, offset + width))
        k, pad = pads[0]
        lo, hi = size["probe_off_%d" % k], size["probe_off_%d" % k] + size["probe_len_%d" % k]
        indent = re.search(r"[ \t]*$", text[:pad["start"]]).group(0)
        t = Member(key=("member", c["cls"], prefix + name), rel=rel,
                   edits=[(pad["start"], pad["end"], split_pad(pad, lo, hi, offset, width, "%s %s%s" % (typ, name, dims),
                                                              indent))],
                   words=type_words(typ), name=name, new=typ + dims, old=None, rows={row}, cls=c["cls"], retyped=False)
        why = self.layout_same(rel, self.file_text(rel, {t.key: t}, set()), c["cls"],
                               [p["path"] for p in plain if p is not pad], (prefix + name, offset))
        if why:
            raise Drop(why)
        return t

    def type_size(self, header, t):
        base, dims = strip_dims(t)
        probe = '#include <globaldefs.h>\n#include "%s"\ntypedef %s probe_t%s;\nchar probe_size[sizeof(probe_t) + 1];\n' % (
            header, base, dims)
        os.makedirs(f"{self.work}/layout", exist_ok=True)
        sig = mechclean.signature(probe, f"{self.work}/layout/size.cpp", "usa", f"{REPO}/include")
        return next((s[2] - 1 for s in sig[0] if s[0] == "probe_size"), None) if sig else None

    def fn_edit(self, text, rel, t, choice):
        before = text
        text = addrname.retype(text, t.symbol, t.info)
        if rel.endswith((".c", ".cpp")):
            retarget = {(k, i) for s in choice if s[0] == "cast" and s[1] == t.key for k, i in [s[2:]]}
            text = addrname.uncast(text, t.info, retarget)
        if t.newret:
            for m in reversed(self.decls(text, t.qual, t.base)):
                text = text[:m.start(3)] + t.newret + " " + text[m.start(4):]
            if rel.endswith((".c", ".cpp")):
                types = {addrname.unconst(t.newret)} | ({addrname.unconst(t.ret)} if t.ret else set())
                for start, end, args in reversed(mechclean.call_sites(text, t.base)):
                    done = drop_cast_around(text, start, end, types)
                    if done:
                        text = done[0]
        if rel == t.defrel and text != before:
            text = self.definition(text, t, ("alias", t.key) in choice)
        return text

    def definition(self, text, t, alias):
        defs = [m for m in self.decls(text, t.qual, t.base) if m.group(7) == "{"]
        if not defs:
            return text
        m = defs[0]
        open_at = text.index("{", m.end())
        close = self.close_brace(text, open_at)
        params = [p.strip() for p in mechclean.split_args(m.group(5))] if m.group(5).strip() not in ("", "void") else []
        changed = [k for k in range(min(len(params), len(t.real), len(t.olddecl or [])))
                   if mechclean.norm_type(t.params[k]) != mechclean.norm_type(t.real[k])
                   and mechclean.param_name(params[k])]
        body = text[open_at + 1:close]
        if alias and changed:
            indent = (re.search(r"\n([ \t]+)\S", body) or re.search(r"()", "    ")).group(1) or "    "
            taken = set(re.findall(r"[A-Za-z_]\w*", text[m.start(5):close]))
            lines = ""
            for k in changed:
                own, old = mechclean.param_name(params[k]), mechclean.param_type(t.olddecl[k])
                name = t.wanted[k] if k < len(t.wanted) and t.wanted[k] and t.wanted[k] not in taken else "arg%d" % k
                params[k] = "%s %s" % (t.params[k], name)
                lines += "\n%s%s %s = (%s)%s;" % (indent, old, own, old, name)
            return text[:m.start(5)] + ", ".join(params) + text[m.end(5):open_at + 1] + lines + body + text[close:]
        for k in changed:
            own = mechclean.param_name(params[k])
            for cast in reversed(list(re.finditer(r"\b%s\b" % re.escape(own), body))):
                done = drop_cast_around(body, cast.start(), cast.end(), {addrname.unconst(t.params[k])})
                if done:
                    body = done[0]
        if t.newret:
            types = {addrname.unconst(t.newret)} | ({addrname.unconst(t.ret)} if t.ret else set())
            for r in reversed(list(re.finditer(r"\breturn\s+([^;]+);", body))):
                cast = addrname.cast_parts(r.group(1))
                if cast and addrname.unconst(cast[0]) in types:
                    body = body[:r.start(1)] + addrname.bare(cast[1]) + body[r.end(1):]
        return text[:open_at + 1] + body + text[close:]

    def reachable(self, rel):
        if rel not in self.closure:
            headers, edges = mechclean.include_graph(self.original)
            todo, seen = list(edges.get(rel, ())), []
            while todo:
                h = todo.pop()
                if headers[h] not in seen:
                    seen.append(headers[h])
                    todo += edges.get(headers[h], ())
            self.closure[rel] = seen
        return self.closure[rel]

    def declare(self, text, before, words, rel=None):
        simple, headers = [], []
        for w in sorted(set(words)):
            outer = re.compile(r"\b%s\b" % re.escape(w.split("::")[0]))
            if w in plausible.SCALAR_TYPEDEFS or outer.search(before) or \
                    (rel and any(outer.search(self.original[h]) for h in self.reachable(rel))):
                continue
            if "::" in w or w in self.aliases:
                header = mechclean.class_header(w) if "::" in w else self.defined_in.get(w)
                if header and '"%s"' % header not in text and header not in headers:
                    headers.append(header)
            else:
                simple.append(w)
        text = addrname.declare(text, before, simple)
        for header in headers:
            last = list(re.finditer(r"(?m)^#include[^\n]*\n", text))
            at = last[-1].end() if last else 0
            text = text[:at] + '#include "%s"\n' % header + text[at:]
        return text

    def tus_of(self, rel):
        if rel not in self.headers_tus:
            self.headers_tus[rel] = set(mechclean.affected(self.original, {rel: ""})[2])
        return self.headers_tus[rel]

    def file_text(self, rel, targets, choice):
        text = self.original[rel]
        out, words = text, []
        if rel.endswith(addrname.SOURCES):
            here = [t for t in targets.values() if t.kind == "member" and t.rel == rel]
            for a, b, replacement in sorted((e for t in here for e in t.edits), reverse=True):
                out = out[:a] + replacement + out[b:]
            words += [w for t in here for w in t.words]
            for t in targets.values():
                if t.kind == "fn" and (t.symbol in out or t.base in out):
                    edited = self.fn_edit(out, rel, t, choice)
                    if edited != out:
                        words += t.words
                        out = edited
                elif t.kind == "member" and t.retyped and rel.endswith((".c", ".cpp")) and t.name in out \
                        and rel in self.tus_of(t.rel):
                    out = member_uncast(out, t.name, t.new, t.old,
                                        {s[2] for s in choice if s[0] == "assign" and s[1] == t.key})
            if out != text:
                out = self.declare(out, text, words, rel)
        mapping = {t.symbol: t.new for t in targets.values() if t.kind == "fn" and t.new != t.symbol}
        if mapping and any(name in out for name in mapping):
            out = addrname.alternation(mapping).sub(lambda m: mapping[m.group(0)], out)
        return out

    def generate(self, targets):
        names = {n for t in targets.values() for n in ([t.symbol, t.base] if t.kind == "fn" else [t.name, t.rel])}
        new = {}
        for rel, text in self.original.items():
            if rel.endswith(addrname.SOURCES + (".s", "symbols.txt")) and (
                    rel in names or any(n in text for n in names)):
                out = self.file_text(rel, targets, self.choices.get(rel, set()))
                if out != text:
                    new[rel] = out
        mapping = {t.symbol: t.new for t in targets.values() if t.kind == "fn" and t.new != t.symbol}
        return new, mapping

    def tu_same(self, rel, text, mapping, include):
        path = f"{self.work}/tu/{rel.replace('/', '_')}"
        os.makedirs(os.path.dirname(path), exist_ok=True)
        for region in mechclean.text_regions(rel, self.original[rel] + text):
            before = mechclean.original_signature(self.original[rel], path, region, self.state)
            if before is None:
                continue
            after = mechclean.signature(text, path, region, include)
            if after is None or mechclean.remap(before, mapping) != after:
                return False
        return True

    def sites(self, rel, targets):
        out, text = [], self.original[rel]
        for t in targets.values():
            if t.kind == "member":
                if t.retyped and rel.endswith((".c", ".cpp")) and t.name in text:
                    out += [("assign", t.key, k) for k in range(len(member_assigns(text, t.name, t.old)))]
                continue
            if not (t.symbol in text or t.base in text):
                continue
            if rel == t.defrel:
                out.append(("alias", t.key))
            if rel.endswith((".c", ".cpp")):
                retyped = addrname.retype(text, t.symbol, t.info)
                for k, (start, end, args) in enumerate(mechclean.call_sites(retyped, t.base)):
                    for i, arg in enumerate(args[:len(t.params)]):
                        cast = addrname.cast_parts(arg)
                        changed = i >= len(t.real) or mechclean.norm_type(t.params[i]) != mechclean.norm_type(t.real[i])
                        if changed and cast and not addrname.CPP_CAST.fullmatch(arg.strip()) and addrname.unconst(
                                cast[0]) in {addrname.unconst(t.params[i])} | (
                                {addrname.unconst(t.real[i])} if i < len(t.real) else set()):
                            out.append(("cast", t.key, k, i))
        return out

    def repair(self, rel, targets, mapping):
        include = f"{self.work}/include"
        sites = self.sites(rel, targets)
        current = self.choices.get(rel, set())
        aliases = {s for s in sites if s[0] == "alias"}
        casts = {s for s in sites if s[0] in ("cast", "assign")}
        for trial in (aliases, casts, aliases | casts):
            trial = current | trial
            if trial == current or not self.tu_same(rel, self.file_text(rel, targets, trial), mapping, include):
                continue
            for s in sorted(trial - current, key=str):
                less = trial - {s}
                if self.tu_same(rel, self.file_text(rel, targets, less), mapping, include):
                    trial = less
            self.choices[rel] = trial
            return True
        return False

    def texts(self, targets):
        new, mapping = self.generate(targets)
        bad, _ = mechclean.verify(self.original, new, mapping, self.jobs, self.work)
        if any([self.repair(rel, targets, mapping) for rel in bad if rel in new]):
            new, mapping = self.generate(targets)
        return new, mapping

    def suspects(self, bad, seen, targets):
        out = set()
        for rel in bad:
            near = {rel} | set(seen.get(rel, ()))
            out |= {k for k in targets if self.touch.get(k, set()) & near}
        return out


def read_rows(path):
    rows = []
    for ln in open(path, encoding="utf-8"):
        if "\t" in ln:
            addr, text = ln.rstrip("\n").split("\t", 1)
            rows.append((addr.strip().lower(), text.strip()))
    return rows


def main():
    args = sys.argv[1:]
    requests, out_dir, jobs, parse_only = REQUESTS, OUT, 8, False
    while args:
        a = args.pop(0)
        if a == "--requests" and args:
            requests = args.pop(0)
        elif a == "--out" and args:
            out_dir = args.pop(0)
        elif a == "-j" and args:
            jobs = int(args.pop(0))
        elif a == "--parse":
            parse_only = True
        else:
            sys.exit(USAGE)
    rows = read_rows(requests)
    plan = Plan(jobs, WORK)
    mechclean.demangle_all({s for names in plan.every.values() for s in names if s.startswith("_Z")})
    parsed = [(addr, text, parse_request(text, plan.funcs, plan.known)) for addr, text in rows]
    if parse_only:
        for addr, text, groups in parsed:
            for g, got in groups:
                print(addr, "UNPARSED %s: %s" % (got, g[:80]) if isinstance(got, str) else got)
        return 0
    notes = collections.defaultdict(list)
    targets, owner = {}, collections.defaultdict(set)
    for addr, text, groups in parsed:
        for g, got in groups:
            if isinstance(got, str):
                notes[addr].append(("unparsed", "%s (%s)" % (got, g[:60])))
                continue
            for c in got:
                try:
                    t = plan.resolve_fn(c, addr) if c["kind"] == "fn" else plan.resolve_member(c, addr)
                except Drop as e:
                    notes[addr].append(("dropped", str(e)))
                    continue
                overlap = [k for k, o in targets.items() if t.kind == "member" and o.kind == "member" and k != t.key
                           and o.rel == t.rel and any(a < d and c < b for a, b, _ in o.edits for c, d, _ in t.edits)]
                if overlap:
                    notes[addr].append(("dropped", "%s overlaps the change to %s" % (t.name, overlap[0][2])))
                    continue
                if t.key in targets:
                    same = t.info == targets[t.key].info and t.newret == targets[t.key].newret if t.kind == "fn" \
                        else t.edits == targets[t.key].edits
                    if not same:
                        notes[addr].append(("dropped", "conflicts with another request for %s" % (t.key,)))
                        owner[t.key].add(addr)
                        targets[t.key].conflict = True
                        continue
                    targets[t.key].rows.add(addr)
                else:
                    targets[t.key] = t
                owner[t.key].add(addr)
    for key in [k for k, t in targets.items() if getattr(t, "conflict", False)]:
        for addr in owner[key]:
            notes[addr].append(("dropped", "requests disagree on %s" % (key,)))
        del targets[key]
    print(f"targets {len(targets)} from {len(rows)} request(s)", flush=True)
    waiting = mechclean.queued()
    for key, t in list(targets.items()):
        try:
            touched = set(plan.generate({key: t})[0])
        except Drop as e:
            touched, why = None, str(e)
        clash = sorted(touched & waiting) if touched is not None else []
        if touched is None or clash or not touched:
            for addr in owner[key]:
                notes[addr].append(("dropped", why if touched is None else "%s is queued in clean/manifest.tsv" % clash[0]
                                    if clash else "changes nothing"))
            del targets[key]
            continue
        plan.touch[key] = touched
    differs, worse = {}, {}
    while True:
        kept = addrname.settle(plan.original, targets, plan.texts, plan.suspects, jobs, WORK, differs)
        if kept is None:
            return 1
        targets, new, mapping = kept
        gained = {}
        for rel, text in new.items():
            if rel.endswith(addrname.SOURCES) and plausible.gained(plan.original[rel], text):
                for key in targets:
                    alone = plan.file_text(rel, {key: targets[key]}, plan.choices.get(rel, set())) \
                        if rel in plan.touch[key] else None
                    if alone is not None and plausible.gained(plan.original[rel], alone):
                        gained[key] = "%s gains %s" % (rel, ",".join(plausible.gained(plan.original[rel], alone)))
        if not gained:
            break
        worse.update(gained)
        targets = {k: t for k, t in targets.items() if k not in gained}
    label = lambda key: key if isinstance(key, str) else "::".join(key[1:])
    for key, rel in differs.items():
        for addr in owner[key]:
            notes[addr].append(("dropped", "%s: %s compiles differently" % (label(key), rel)))
    for key, why in worse.items():
        for addr in owner[key]:
            notes[addr].append(("dropped", "%s: %s" % (label(key), why)))
    for key, t in targets.items():
        alias = any(("alias", key) in c for c in plan.choices.values())
        if t.kind == "fn":
            done = "%s%s(%s)%s%s" % (t.newret + " " if t.newret else "", t.base, ", ".join(t.params),
                                     " as %s" % t.new if t.new != t.symbol else "",
                                     ", its definition binding the old type" if alias else "")
        else:
            done = "%s::%s %s" % (t.cls, t.name, t.new)
        for addr in owner[key]:
            notes[addr].append(("applied", done))
    os.makedirs(out_dir, exist_ok=True)
    if new and targets and not mechclean.queue(new, plan.blobs, "retype requests", out_dir):
        return 1
    table, unblocked = [], []
    for addr, text, groups in parsed:
        kinds = {k for k, _ in notes[addr]}
        status = next(iter(kinds)) if len(kinds) == 1 else "partial" if "applied" in kinds else "dropped"
        if status == "applied":
            unblocked.append(addr)
        table.append("%s\t%s\t%s\n" % (addr, status, "; ".join("%s: %s" % n for n in notes[addr])))
    with open(f"{out_dir}/results.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(table)
    print("".join(table), end="")
    print(f"applied {len(targets)} change(s) in {len(new) if targets else 0} file(s); unblocked rows: {' '.join(unblocked)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
