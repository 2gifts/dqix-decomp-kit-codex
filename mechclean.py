import collections
import functools
import glob
import os
import re
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

import buildcfg
import cleancheck
import kitpaths
import mangle
import plausible

KIT, SP, REPO = kitpaths.KIT, kitpaths.SP, kitpaths.REPO
QUEUE = f"{SP}/wlog/clean_queue.tsv"
LOG = f"{SP}/wlog/clean.log"
CLAIMS = f"{SP}/clean/claims"
MANIFEST = f"{SP}/clean/manifest.tsv"
WORK = f"{SP}/handwork/mechclean"
USAGE = """python mechclean.py [-j N] [--tree | --headers]

Rewrites, without a worker, the plausible.py kinds that have one natural form, for every row of
$SP/wlog/clean_queue.tsv no worker has finished or claimed: NOOP-ADDR-CAST,
TERNARY-SELF, SELF-ASSIGN, INLINE-LOCAL-RETURN, EXTERN-C-MANGLED. An inline whose local the ROM needs
gets the compiled-out assert instead (`{ SDK_NULL_ASSERT(p); return e; }`: any statement before the
return makes mwcc expand the body where the call statement starts). A rewrite is kept only when the
function still gates MATCH and cleancheck.py accepts it; it goes to $SP/clean/manifest.tsv, a staging
row's to $SP/staging/<module>/, and clean.log records CLEAN.

EXTERN-C-MANGLED drops `extern "C"` from a free function's `_Z<len><name>` declaration or definition and
calls it by <name>, one function at a time; a rename is kept only when every region the source names
compiles to the same object (sections, relocations, symbols), so a signature the mangling contradicts
stays for a worker. --tree runs that rewrite over every landed source on decomp-matching instead of
the queue. --headers declares, in the class's header, each member function a landed source still
reaches through an `extern "C"` mangled name, kept only when every unit that includes it compiles to the
same object."""
EXTERN_C_FREE = re.compile(r'(?m)^([ \t]*)extern\s+"C"\s+([^;{}()=]*?)\b(_Z(\d+)(\w+))(?=\s*\()')
REGION_DEFINES = {"usa": ["-d", "usa"], "jpn": ["-d", "jpn"], "eur": ["-d", "usa", "-d", "eur"]}
BUILTIN_WORDS = set("const volatile unsigned signed char short int long float double bool void wchar_t".split())
DEMANGLED = {}
SKIPPED_SECTIONS = re.compile(r"debug|^\.line|^\.comment|^\.mwcats|^\.rel\.|^\.rela\.|^\.symtab$|^\.strtab$|^\.shstrtab$")

LVAL = r"[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*|\[[^\]\[]*\])*"


def noop_addr_cast(line):
    return re.sub(r"\*\s*\(\s*[A-Za-z_][\w\s]*\*\s*\)\s*&\s*(%s)" % LVAL, r"\1", line)


def ternary_self(line):
    m = re.match(r"^(\s*)(%s)\s*=\s*(.+?)\s*\?\s*(.+?)\s*:\s*(.+?)\s*;\s*$" % LVAL, line)
    if not m:
        return line
    ind, lhs, cond, yes, no = m.groups()
    if no == lhs:
        return "%sif (%s) %s = %s;" % (ind, cond, lhs, yes)
    if yes == lhs:
        return "%sif (!(%s)) %s = %s;" % (ind, cond, lhs, no)
    return line


def self_assign(line):
    return None if re.match(r"^\s*(%s)\s*=\s*\1\s*;\s*$" % LVAL, line) else line


LINE_FIX = {"NOOP-ADDR-CAST": noop_addr_cast,
            "TERNARY-SELF": ternary_self, "SELF-ASSIGN": self_assign}
FIXABLE = set(LINE_FIX) | {"INLINE-LOCAL-RETURN", "EXTERN-C-MANGLED"}
TREE_FIXABLE = {"EXTERN-C-MANGLED"}


LOCAL_RETURN = re.compile(r"(?:\(([^()]*)\)(\s*const)?\s*)?\{\s*(?!static\b)(?:const\s+)?[A-Za-z_][\w\s\*]*?\b"
                          r"([A-Za-z_]\w*)\s*=\s*([^;{}]+);\s*return\s+\3\s*;\s*\}")


def null_assert(params, member):
    for param in params.split(","):
        m = re.search(r"[*&]\s*([A-Za-z_]\w*)\s*$", param)
        if m:
            return "SDK_NULL_ASSERT(%s); " % m.group(1)
    return "SDK_NULL_ASSERT(this); " if member else None


def inline_local_return(text, asserted=False):
    def fix(m):
        whole = m.group(0)
        head = whole[:whole.index("{")]
        stmt = null_assert(m.group(1) or "", bool(m.group(2))) if asserted else ""
        if stmt is None:
            return whole
        return "%s{ %sreturn %s; }" % (head, stmt, m.group(4))
    new = LOCAL_RETURN.sub(fix, text)
    if not plausible.gained(text, new):
        return new
    for m in reversed(list(LOCAL_RETURN.finditer(text))):
        trial = text[:m.start()] + fix(m) + text[m.end():]
        if not plausible.gained(text, trial):
            text = trial
    return text


def rewrite(text, kinds, asserted=False):
    lines = text.split("\n")
    out = []
    for ln in lines:
        new = ln
        for k in kinds:
            if k in LINE_FIX and new is not None:
                new = LINE_FIX[k](new)
        if new is not None:
            out.append(new)
    text = "\n".join(out)
    if "INLINE-LOCAL-RETURN" in kinds:
        text = inline_local_return(text, asserted)
    return text


def free_mangled(text):
    out = {}
    for m in EXTERN_C_FREE.finditer(text):
        n, rest = int(m.group(4)), m.group(5)
        if len(rest) > n and re.match(r"[A-Za-z_]\w*$", rest[:n]) and rest[n] != "I":
            out[m.group(3)] = rest[:n]
    return out


def extern_c_mangled(text, only=None):
    names = {k: v for k, v in free_mangled(text).items() if only is None or k in only}
    if not names:
        return text
    new = EXTERN_C_FREE.sub(lambda m: m.group(1) + m.group(2) + names[m.group(3)] if m.group(3) in names
                            else m.group(0), text)
    for mangled, plain in names.items():
        new = re.sub(r"\b%s\b" % re.escape(mangled), plain, new)
    return new


def member_parts(name):
    m = re.match(r"_ZN(K?)(?=\d)", name)
    if not m:
        return None
    i, parts = m.end(), []
    while i < len(name) and name[i].isdigit():
        j = re.match(r"\d+", name[i:]).end() + i
        parts.append(name[j:j + int(name[i:j])])
        i = j + int(name[i:j])
    if name[i:i + 1] != "E" or len(parts) < 2 or name[i + 1:i + 2] == "I":
        return None
    return "::".join(parts[:-1]), parts[-1], mangle.mangled_arity(name)


def call_sites(text, name):
    out = []
    for m in re.finditer(r"\b%s\s*\(" % re.escape(name), text):
        depth, i, cur, args = 1, m.end(), m.end(), []
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
        out.append((m.start(), i, [] if args == [""] else args))
    return out


def receiver(expr):
    postfix = r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*|\[[^\[\]]*\]|\([^()]*\))*"
    m = re.fullmatch(r"&\s*(%s)" % postfix, expr)
    if m:
        return m.group(1) + "."
    return expr + "->" if re.fullmatch(postfix, expr) else "(%s)->" % expr


def member_call(text, name):
    parts = member_parts(name)
    decls = list(re.finditer(r'(?m)^[ \t]*extern\s+"C"\s+[^;{}()=]*?\b%s\s*\(([^;{}]*)\)\s*;[ \t]*\n?' % re.escape(name),
                             text))
    if not parts or parts[2] is None or not decls:
        return text
    qual, method, arity = parts
    params = decls[0].group(1).strip()
    declared = 0 if params in ("", "void") else len(split_args(params))
    if declared not in (arity, arity + 1):
        return text
    for d in reversed(decls):
        text = text[:d.start()] + text[d.end():]
    for start, end, args in reversed(call_sites(text, name)):
        if declared == arity + 1:
            if not args:
                return None
            call = "%s%s(%s)" % (receiver(args[0]), method, ", ".join(args[1:]))
        else:
            call = "%s::%s(%s)" % (qual, method, ", ".join(args))
        text = text[:start] + call + text[end:]
    return None if re.search(r"\b%s\b" % re.escape(name), text) else text


def split_args(params):
    out, depth, cur = [], 0, ""
    for ch in params:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    return out + [cur]


def demangle(name):
    if name not in DEMANGLED:
        tool = shutil.which("c++filt")
        DEMANGLED[name] = subprocess.run([tool, name], capture_output=True, text=True).stdout.strip() if tool else name
    m = re.match(r"^(.*?)\((.*)\)( const)?$", DEMANGLED[name])
    if not m or DEMANGLED[name] == name:
        return None
    params = [p.strip() for p in split_args(m.group(2))] if m.group(2).strip() not in ("", "void") else []
    return m.group(1), params, bool(m.group(3))


def param_type(param):
    t = re.sub(r"\s*\[\s*\w*\s*\]\s*$", "*", param.strip())
    t = re.sub(r"\(\s*\*\s*\w*\s*\)", "(*)", t)
    toks = re.findall(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*|\*|&|\(\*\)|\(|\)|,|\.\.\.", t)
    if len(toks) >= 2 and re.match(r"[A-Za-z_]", toks[-1]) and toks[-1] not in BUILTIN_WORDS and "(*)" not in toks \
            and any(x not in ("const", "volatile", "struct", "class", "enum", "union") for x in toks[:-1]):
        toks = toks[:-1]
    return " ".join(toks).replace(" *", "*").replace(" &", "&")


def norm_type(t):
    s = re.sub(r"\b(?:struct|class|enum|union)\s+", "", param_type(t))
    s = re.sub(r"\bsigned\s+(?!char\b)", "", s)
    s = re.sub(r"\bunsigned\b(?!\s+(?:char|short|int|long))", "unsigned int", s)
    s = re.sub(r"\b(short|long) int\b", r"\1", s)
    m = re.match(r"^const (.+?)(\**&?)$", s)
    return (m.group(1) + " const" + m.group(2) if m else s).replace(" ", "")


def plain_expr(arg):
    return re.fullmatch(r"&?\s*[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*|\[[^\[\]]*\]|\([^()]*\))*"
                        r"|-?(?:0[xX][0-9a-fA-F]+|\d+)[uUlL]*", arg) is not None


def wrapped(expr):
    if not expr.startswith("("):
        return False
    depth = 0
    for i, ch in enumerate(expr):
        depth += {"(": 1, ")": -1}.get(ch, 0)
        if depth == 0:
            return i == len(expr) - 1
    return False


def convert(arg, declared, real, scalar_cast):
    d, r = norm_type(declared), norm_type(real)
    if d == r or (d.replace("const", "") == r.replace("const", "") and "const" in r) or \
            (r in ("void*", "voidconst*") and "*" in d):
        return arg
    if "*" in d or "*" in r or scalar_cast:
        inner = re.fullmatch(r"\(\s*(?:const\s+)?(?:struct\s+)?[A-Za-z_][\w\s]*\*+\s*\)\s*(.+)", arg)
        if inner and (plain_expr(inner.group(1)) or wrapped(inner.group(1))) and ("*" in d or "*" in r):
            arg = inner.group(1)
            if wrapped(arg):
                return "(%s)%s" % (real, arg)
        return "(%s)%s" % (real if "*" in d or "*" in r else param_type(declared), arg if plain_expr(arg) else "(%s)" % arg)
    return arg


def class_body(text, qual):
    m = re.search(r"(?m)^\s*(?:struct|class)\s+%s\b[^;{]*\{" % re.escape(qual.split("::")[-1]), text)
    if not m:
        return None
    depth, i = 1, m.end()
    while depth and i < len(text):
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
    return m.end(), i - 1


def class_header(qual):
    for path in glob.glob(f"{REPO}/include/**/*.h", recursive=True):
        if re.search(r"(?m)^\s*(?:struct|class)\s+%s\b[^;]*\{" % re.escape(qual), open(path, encoding="utf-8",
                                                                                     errors="replace").read()):
            return os.path.relpath(path, f"{REPO}/include").replace("\\", "/")
    return None


def method_return(qual, method):
    header = class_header(qual)
    if not header:
        return None
    text = open(f"{REPO}/include/{header}", encoding="utf-8", errors="replace").read()
    body = class_body(text, qual)
    m = body and re.search(r"(?m)^\s*(?:static\s+|virtual\s+|inline\s+)*([A-Za-z_][\w\s\*&:]*?)\s*\b%s\s*\(" %
                           re.escape(method), text[body[0]:body[1]])
    return m.group(1).strip() if m else None


@functools.lru_cache(maxsize=None)
def header_functions(header):
    return _header_functions(header, set())


def _header_functions(header, seen):
    path = f"{REPO}/include/{header}"
    if header in seen or not os.path.exists(path):
        return {}
    seen.add(header)
    text, depth, flat = open(path, encoding="utf-8", errors="replace").read(), 0, []
    for ch in text:
        depth += {"{": 1, "}": -1}.get(ch, 0)
        flat.append(ch if depth == 0 or ch in "{}" else " ")
    names = {m.group(2): m.group(1).strip() for m in re.finditer(
        r"(?m)^[ \t]*(?:extern\s+\"C\"\s+)?([A-Za-z_][^;{}()=\n]*?)\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*;", "".join(flat))}
    for inc in includes(text):
        names = {**_header_functions(inc, seen), **names}
    return names


def included_functions(text):
    out = {}
    for inc in includes(text):
        out = {**header_functions(inc), **out}
    return out


def param_name(param):
    m = re.search(r"([A-Za-z_]\w*)\s*(?:\[\s*\w*\s*\])?\s*$", param)
    return m.group(1) if m and param_type(param) != re.sub(r"\s+", " ", param.strip()) else None


def typed_def(text, name, with_header, real=None):
    parsed = demangle(name)
    m = re.search(r'(?m)^([ \t]*)extern\s+"C"\s+([^;{}()=]*?)\b%s\s*\(([^;{}]*)\)\s*\{' % re.escape(name), text)
    if not parsed or not m:
        return text
    full, demangled, const = parsed
    real = demangled if real is None else real
    qual, ident = full.rsplit("::", 1) if "::" in full else (None, full)
    declared = [p.strip() for p in split_args(m.group(3))] if m.group(3).strip() not in ("", "void") else []
    member = qual is not None and len(declared) == len(real) + 1
    if len(declared) - member > len(real):
        return None
    params, aliases = [], []
    if member:
        own, alias = param_name(declared[0]), param_type(declared[0])
        if own is None:
            return None
        same = norm_type(alias) == norm_type(qual + "*") and not const
        aliases.append("%s %s = %s;" % (alias, own, "this" if same else "(%s)this" % alias))
    for i, rt in enumerate(real):
        j = i + member
        if j >= len(declared):
            params.append(rt)
            continue
        own = param_name(declared[j])
        if own is None or norm_type(declared[j]) == norm_type(rt):
            params.append(declared[j] if own else rt)
            continue
        params.append("%s arg%d" % (rt, i))
        aliases.append("%s %s = (%s)arg%d;" % (param_type(declared[j]), own, param_type(declared[j]), i))
    head = "%s%s%s%s(%s)%s {" % (m.group(1), m.group(2), qual + "::" if qual else "", ident, ", ".join(params),
                                 " const" if const else "")
    body = "".join("\n%s    %s" % (m.group(1), a) for a in aliases)
    text = text[:m.start()] + head + body + text[m.end():]
    if qual is not None:
        cls = class_body(text, qual)
        if cls and cls[1] < m.start() and not re.search(r"\b%s\s*\(" % re.escape(ident), text[cls[0]:cls[1]]):
            line = "    %s%s %s(%s)%s;\n" % ("" if member else "static ", re.sub(r"\b(?:ARM|THUMB)\b", "",
                                                                                m.group(2)).strip(),
                                            ident, ", ".join(real), " const" if const else "")
            text = text[:cls[1]] + line + text[cls[1]:]
        elif not cls and with_header:
            header = class_header(qual)
            if not header:
                return None
            if '"%s"' % header not in text:
                last = list(re.finditer(r"(?m)^#include[^\n]*\n", text))
                at = last[-1].end() if last else 0
                text = text[:at] + '#include "%s"\n' % header + text[at:]
    return text


def typed_call(text, name, scalar_cast, with_header, real=None):
    parsed = demangle(name)
    decls = list(re.finditer(r'(?m)^([ \t]*)extern\s+"C"\s+([^;{}()=]*?)\b%s\s*\(([^;{}]*)\)\s*;[ \t]*\n?'
                             % re.escape(name), text))
    if parsed and not decls:
        return typed_def(text, name, with_header, real)
    if not parsed:
        return None
    full, demangled, const = parsed
    real = demangled if real is None else real
    qual, ident = full.rsplit("::", 1) if "::" in full else (None, full)
    ret = decls[0].group(2).strip()
    declared = [p.strip() for p in split_args(decls[0].group(3))] if decls[0].group(3).strip() not in ("", "void") else []
    member = qual is not None and len(declared) == len(real) + 1
    if len(declared) != len(real) + member or (with_header and qual is None):
        return None
    known = included_functions(text) if qual is None else {}
    for d in reversed(decls[1:]):
        text = text[:d.start()] + text[d.end():]
    first, at = decls[0], decls[0].start()
    rest = text[:first.start()] + text[first.end():]
    words = {w for t in real for w in re.findall(r"[A-Za-z_]\w*(?:::\w+)*", t)} - BUILTIN_WORDS
    if any("::" in w for w in words):
        return None
    insert = "".join("struct %s;\n" % w for w in sorted(words) if not re.search(r"\b%s\b" % w, rest))
    if qual is None and ident not in known:
        insert += "%s %s(%s);\n" % (ret, ident, ", ".join(real))
    elif qual is not None:
        body = class_body(rest, qual)
        if body and not re.search(r"\b%s\s*\(" % re.escape(ident), rest[body[0]:body[1]]):
            line = "    %s%s %s(%s)%s;\n" % ("" if member else "static ", ret, ident, ", ".join(real),
                                            " const" if const else "")
            rest = rest[:body[1]] + line + rest[body[1]:]
            at += len(line) if body[1] <= at else 0
    text = rest[:at] + insert + rest[at:]
    if with_header:
        header = class_header(qual)
        if not header or '"%s"' % header in text:
            return None
        known = header_functions(header)
        decl = re.compile(r"^(?:extern\s+\"C\"\s+)?[A-Za-z_][^;{}()=]*?\b(%s)\s*\([^;{}]*\)\s*;\s*$" %
                          "|".join(map(re.escape, sorted(known)))) if known else None
        kept, depth = [], 0
        for line in text.split("\n"):
            if not (depth == 0 and decl and decl.match(line)):
                kept.append(line)
            depth += line.count("{") - line.count("}")
        text = "\n".join(kept)
        last = list(re.finditer(r"(?m)^#include[^\n]*\n", text))
        at = last[-1].end() if last else 0
        text = text[:at] + '#include "%s"\n' % header + text[at:]
    header_ret = method_return(qual, ident) if qual else known.get(ident)
    wrap = header_ret is not None and "*" in ret and norm_type(header_ret + " x") != norm_type(ret + " x")
    for start, end, args in reversed(call_sites(text, name)):
        if 'extern "C"' in text[text.rfind("\n", 0, start) + 1:start]:
            continue
        if len(args) != len(declared):
            return None
        conv = [convert(a, d, r, scalar_cast) for a, d, r in zip(args[member:], declared[member:], real)]
        if ident == "operator=" and member and len(real) == 1 and real[0].endswith("&"):
            to = lambda a, t: a[1:].strip() if a.startswith("&") and plain_expr(a[1:].strip()) and not scalar_cast \
                else "*(%s)%s" % (t, a if plain_expr(a) else "(%s)" % a)
            call = "%s = %s" % (to(args[0], qual + "*"), to(args[1], real[0][:-1].strip() + "*"))
        elif qual is None:
            call = "%s(%s)" % (ident, ", ".join(conv))
        elif member:
            recv = receiver(args[0]) if norm_type(declared[0]) == norm_type(qual + "*") else \
                "((%s*)%s)->" % (qual, args[0] if plain_expr(args[0]) else "(%s)" % args[0])
            call = "%s%s(%s)" % (recv, ident, ", ".join(conv))
        else:
            call = "%s::%s(%s)" % (qual, ident, ", ".join(conv))
        text = text[:start] + ("(%s)%s" % (re.sub(r"\bstruct\s+", "", ret), call) if wrap else call) + text[end:]
    text = typed_def(text, name, with_header, real)
    return None if text is None or re.search(r"\b%s\b" % re.escape(name), text) else text


def strategies(name):
    plain = (lambda t: member_call(t, name)) if name.startswith("_ZN") else \
        (lambda t: extern_c_mangled(t, {name}))
    typed = [lambda t, c=c, h=h: typed_call(t, name, c, h) for h in (False, True) for c in (False, True)]
    return [plain] + typed


def apply_names(text, names):
    plan = names if isinstance(names, dict) else {n: 0 for n in names}
    for name in sorted(plan):
        text = strategies(name)[plan[name]](text)
        if text is None or re.search(r"\b%s\b" % re.escape(name), text):
            return None
    return text


def region_flags(region):
    flags = list(buildcfg.FLAGS)
    while len(flags) >= 2 and flags[-2] == "-d" and flags[-1] in REGION_DEFINES:
        del flags[-2:]
    return flags + REGION_DEFINES[region]


def signature(text, path, region, include=None):
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    obj = f"{path}.{region}.o"
    flags = region_flags(region)
    if include:
        flags = [include if f == "include" and flags[i - 1] == "-i" else f for i, f in enumerate(flags)]
    r = subprocess.run(buildcfg.tool_command(buildcfg.cc_path(None)) + flags + ["-c", path, "-o", obj],
                       cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if r.returncode:
        return None
    with open(obj, "rb") as fh:
        elf = ELFFile(fh)
        names = [s.name for s in elf.iter_sections()]
        kept = lambda i: isinstance(i, int) and not SKIPPED_SECTIONS.search(names[i])
        syms = list(elf.get_section_by_name(".symtab").iter_symbols())
        sig = [tuple(sorted((s.name, s["st_value"], s["st_size"], s["st_info"]["type"], s["st_info"]["bind"],
                             names[s["st_shndx"]] if isinstance(s["st_shndx"], int) else s["st_shndx"])
                            for s in syms if not isinstance(s["st_shndx"], int) or kept(s["st_shndx"])))]
        for sec in elf.iter_sections():
            if isinstance(sec, RelocationSection):
                if kept(sec["sh_info"]):
                    sig.append((sec.name, tuple((r["r_offset"], r["r_info_type"], syms[r["r_info_sym"]].name)
                                                for r in sec.iter_relocations())))
            elif not SKIPPED_SECTIONS.search(sec.name):
                sig.append((sec.name, sec.data()))
    os.remove(obj)
    return sig


def tree_texts(roots=("src", "include", "config"), suffixes=(".c", ".cpp", ".h", ".hpp", ".inc", ".s", "symbols.txt")):
    blobs = {}
    for ln in subprocess.run(["git", "-C", REPO, "ls-tree", "-r", "HEAD", "--", *roots], capture_output=True,
                             text=True, encoding="utf-8").stdout.splitlines():
        meta, rel = ln.split("\t", 1)
        if rel.endswith(suffixes):
            blobs[rel] = meta.split()[2]
    return blobs, {rel: open(f"{REPO}/{rel}", encoding="utf-8", errors="replace").read() for rel in blobs}


def includes(text):
    return re.findall(r'(?m)^\s*#\s*include\s*[<"]([^>"]+)[>"]', text)


def remap(sig, mapping):
    syms = tuple(sorted((mapping.get(s[0], s[0]),) + tuple(s[1:]) for s in sig[0]))
    rest = [(n, tuple((o, t, mapping.get(s, s)) for o, t, s in d)) if isinstance(d, tuple) else (n, d)
            for n, d in sig[1:]]
    return [syms] + rest


def verify(original, new, mapping, jobs, work):
    headers = {rel[len("include/"):]: rel for rel in original if rel.startswith("include/")}
    closure = {h for h, rel in headers.items() if rel in new}
    while True:
        more = {h for h, rel in headers.items() if h not in closure and set(includes(original[rel])) & closure}
        if not more:
            break
        closure |= more
    tus = sorted(rel for rel in original if rel.endswith((".c", ".cpp"))
                 and (rel in new or set(includes(original[rel])) & closure))
    print(f"files {len(new)}; translation units to check {len(tus)}", flush=True)
    os.makedirs(f"{work}/tu", exist_ok=True)
    shutil.rmtree(f"{work}/include", ignore_errors=True)
    shutil.copytree(f"{REPO}/include", f"{work}/include")
    for rel, text in new.items():
        if rel.startswith("include/"):
            with open(f"{work}/{rel}", "w", encoding="utf-8", newline="\n") as fh:
                fh.write(text)

    def check(rel):
        path = f"{work}/tu/{rel.replace('/', '_')}"
        before = signature(original[rel], path, "usa", f"{REPO}/include")
        if before is None:
            return rel, rel not in new
        after = signature(new.get(rel, original[rel]), path, "usa", f"{work}/include")
        return rel, after is not None and remap(before, mapping) == after

    with ThreadPoolExecutor(jobs) as pool:
        bad = [rel for rel, ok in pool.map(check, tus) if not ok]
    return bad, {rel: [headers[h] for h in set(includes(original[rel])) & closure] for rel in bad}


def queue(new, blobs, why):
    queued = {ln.split("\t")[0] for ln in open(MANIFEST, encoding="utf-8")} if os.path.exists(MANIFEST) else set()
    clash = sorted(set(new) & queued)
    if clash:
        print(f"REFUSED: {len(clash)} file(s) already queued in the manifest, e.g. {clash[0]}; land them first")
        return False
    for rel, text in sorted(new.items()):
        os.makedirs(os.path.dirname(f"{SP}/clean/{rel}"), exist_ok=True)
        with open(f"{SP}/clean/{rel}", "w", encoding="utf-8", newline="\n") as fh:
            fh.write(text)
    with open(MANIFEST, "a", encoding="utf-8", newline="\n") as fh:
        fh.writelines(f"{rel}\t{blobs[rel]}\t{why}\n" for rel in sorted(new))
    return True


def method_decls(original):
    groups = collections.defaultdict(collections.Counter)
    for rel, text in original.items():
        if not rel.startswith("src/") or not rel.endswith((".c", ".cpp")):
            continue
        for m in re.finditer(r'(?m)^[ \t]*extern\s+"C"\s+([^;{}()=]*?)\b(_ZN\w+)\s*\(([^;{}]*)\)\s*[;{]', text):
            parsed, parts = demangle(m.group(2)), member_parts(m.group(2))
            if not parsed or not parts or parts[2] is None:
                continue
            declared = len(split_args(m.group(3))) if m.group(3).strip() not in ("", "void") else 0
            if declared in (parts[2], parts[2] + 1):
                groups[(parts[0], parts[1], tuple(parsed[1]), parsed[2], declared == parts[2] + 1)][
                    re.sub(r"\bARM\b|\bTHUMB\b", "", m.group(1)).strip()] += 1
    new = {}
    for (qual, method, real, const, member), rets in sorted(groups.items()):
        header = class_header(qual)
        if not header:
            continue
        rel = f"include/{header}"
        text = new.get(rel, original[rel])
        ret = rets.most_common(1)[0][0]
        words = {w for t in list(real) + [ret] for w in re.findall(r"[A-Za-z_]\w*(?:::\w+)*", t)} - BUILTIN_WORDS
        body = class_body(text, qual)
        if not body or re.search(r"\b%s\s*\(" % re.escape(method), text[body[0]:body[1]]) or \
                any("::" in w or not re.search(r"\b%s\b" % w, text) for w in words - {qual}):
            continue
        line = "    %s%s %s(%s)%s;\n" % ("" if member else "static ", ret, method, ", ".join(real),
                                        " const" if const else "")
        new[rel] = text[:body[1]] + line + text[body[1]:]
    return new


def header_pass(jobs):
    blobs, original = tree_texts()
    new = method_decls(original)
    while new:
        bad, seen = verify(original, new, {}, jobs, f"{SP}/handwork/headerpass")
        if not bad:
            break
        drop = {h for rel in bad for h in seen[rel]}
        print(f"{len(bad)} unit(s) differ, e.g. {bad[0]}; dropping {len(drop)} header(s)", flush=True)
        if not drop:
            return 1
        new = {rel: text for rel, text in new.items() if rel not in drop}
    if new and not queue(new, blobs, "mechanical: method declarations"):
        return 1
    print(f"declared methods in {len(new)} header(s)")
    return 0


def externc_clean(text, path):
    members = re.findall(r'(?m)^[ \t]*extern\s+"C"\s+[^;{}()=]*?\b(_ZN\w+)\s*\([^;{}]*\)\s*[;{]', text)
    members = [n for n in members if not plausible.structor_variant(n)]
    names = list(free_mangled(text)) + sorted(set(members), key=members.index)
    if not names:
        return text
    regions = ["usa"] + [r for r in ("jpn", "eur") if re.search(r"\b%s\b" % r, text)]
    base = {r: signature(text, path, r) for r in regions}
    if None in base.values():
        return text
    same = lambda t: t is not None and all(signature(t, path, r) == base[r] for r in regions)
    if same(apply_names(text, set(names))):
        return apply_names(text, set(names))
    kept = {}
    for name in names:
        for strategy in range(len(strategies(name))):
            if same(apply_names(text, {**kept, name: strategy})):
                kept[name] = strategy
                break
    return apply_names(text, kept)


def gate(mod, addr, path):
    r = subprocess.run([sys.executable, f"{KIT}/wgate.py", mod, addr, path], capture_output=True, text=True,
                       encoding="utf-8", errors="replace", env=dict(os.environ, WGATE_ALLOW_COMMITTED="1"))
    return any(ln.startswith("MATCH") for ln in (r.stdout + r.stderr).splitlines())


def finished(addr):
    try:
        return re.search(r" (CLEAN|KEEP|BLOCKED|SKIP) %s " % addr, open(LOG, encoding="utf-8").read()) is not None
    except OSError:
        return False


def candidates(kinds):
    for subset in [kinds] + ([[k] for k in kinds] if len(kinds) > 1 else []):
        for asserted in (False, True) if "INLINE-LOCAL-RETURN" in subset else (False,):
            yield subset, asserted


def accepted(mod, addr, path, text, new, staged):
    return (gate(mod, addr, path) and cleancheck.accepted(text.split("\n"), new.split("\n"), set())[0]
            and not (staged and plausible.blocking(new)))


def job(row, force=False):
    mod, addr, rel = row[:3]
    staged = row[5:6] == ["staging"]
    if not force and finished(addr):
        return None
    try:
        os.mkdir(f"{CLAIMS}/{addr}")
    except OSError:
        return None
    try:
        if staged:
            base = ""
            text = open(rel, encoding="utf-8", errors="replace").read()
        else:
            base = subprocess.run(["git", "-C", REPO, "rev-parse", f"origin/decomp-matching:{rel}"],
                                  capture_output=True, text=True).stdout.strip()
            text = subprocess.run(["git", "-C", REPO, "cat-file", "blob", base], capture_output=True, text=True,
                                  encoding="utf-8", errors="replace").stdout
        found = sorted({k for _, k, _ in plausible.findings(text)} & (TREE_FIXABLE if mod == "tree" else FIXABLE))
        if not found:
            return None
        path = f"{WORK}/{mod}/{rel.replace('/', '_') if mod == 'tree' else os.path.basename(rel)}"
        os.makedirs(os.path.dirname(path), exist_ok=True)
        renamed = externc_clean(text, path) if "EXTERN-C-MANGLED" in found else text
        new, kinds = renamed, ["EXTERN-C-MANGLED"] if renamed != text else []
        tried = bool(kinds)
        for subset, asserted in candidates([k for k in found if k != "EXTERN-C-MANGLED"]):
            trial = rewrite(renamed, subset, asserted)
            if trial == renamed:
                continue
            tried = True
            with open(path, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(trial)
            if accepted(mod, addr, path, renamed, trial, staged):
                new, kinds = trial, kinds + subset
                break
        if new == text or (staged and plausible.blocking(new)):
            if new != text:
                with open(rel, "w", encoding="utf-8", newline="\n") as fh:
                    fh.write(new)
            return f"{addr} kept for a worker ({','.join(found)})" if tried else None
        if staged:
            dest = f"{SP}/staging/{'main' if mod == 'main' else 'ov' + mod}/{os.path.basename(rel)}"
        else:
            dest = f"{SP}/clean/{rel}"
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        with open(dest, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(new)
        if staged:
            os.remove(rel)
        else:
            with open(MANIFEST, "a", encoding="utf-8", newline="\n") as fh:
                fh.write(f"{rel}\t{base}\tmechanical: {','.join(kinds)}\n")
        if mod != "tree":
            with open(LOG, "a", encoding="utf-8", newline="\n") as fh:
                fh.write(f"{time.strftime('%H:%M')} CLEAN {addr} $0 mechanical {','.join(kinds)}\n")
        return f"{addr} CLEAN ({','.join(kinds)})"
    finally:
        os.rmdir(f"{CLAIMS}/{addr}")


def tree_rows():
    def git(*args):
        return subprocess.run(["git", "-C", REPO, *args], capture_output=True, text=True, encoding="utf-8",
                              errors="replace").stdout
    queued = {ln.split("\t")[0] for ln in open(MANIFEST, encoding="utf-8")} if os.path.exists(MANIFEST) else set()
    for rel in git("grep", "-l", "-E", r'extern "C" .*_Z[0-9N]', "origin/decomp-matching", "--", "src").split():
        rel = rel.split(":", 1)[1]
        if rel.endswith(".cpp") and rel not in queued:
            yield ["tree", rel.replace("/", "_"), rel, "", "EXTERN-C-MANGLED"]


def main():
    args = [a for a in sys.argv[1:] if a not in ("--tree", "--headers")]
    if len(args) not in (0, 2) or (args and args[0] != "-j"):
        sys.exit(USAGE)
    os.makedirs(CLAIMS, exist_ok=True)
    if "--headers" in sys.argv:
        return header_pass(int(args[1]) if args else 8)
    if "--tree" in sys.argv:
        rows, run = list(tree_rows()), lambda row: job(row, True)
    else:
        rows, run = [ln.rstrip("\n").split("\t") for ln in open(QUEUE, encoding="utf-8") if ln.count("\t") >= 2], job
    with ThreadPoolExecutor(int(args[1]) if args else 3) as pool:
        for result in pool.map(run, rows):
            if result:
                print(result, flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
