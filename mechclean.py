import os
import re
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
USAGE = """python mechclean.py [-j N] [--tree]

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
the queue."""
EXTERN_C_FREE = re.compile(r'(?m)^([ \t]*)extern\s+"C"\s+([^;{}()=]*?)\b(_Z(\d+)(\w+))(?=\s*\()')
REGION_DEFINES = {"usa": ["-d", "usa"], "jpn": ["-d", "jpn"], "eur": ["-d", "usa", "-d", "eur"]}
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


def apply_names(text, names):
    text = extern_c_mangled(text, {n for n in names if not n.startswith("_ZN")})
    for name in sorted(n for n in names if n.startswith("_ZN")):
        text = member_call(text, name)
        if text is None:
            return None
    return text


def region_flags(region):
    flags = list(buildcfg.FLAGS)
    while len(flags) >= 2 and flags[-2] == "-d" and flags[-1] in REGION_DEFINES:
        del flags[-2:]
    return flags + REGION_DEFINES[region]


def signature(text, path, region):
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    obj = f"{path}.{region}.o"
    r = subprocess.run(buildcfg.tool_command(buildcfg.cc_path(None)) + region_flags(region) + ["-c", path, "-o", obj],
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


def externc_clean(text, path):
    members = re.findall(r'(?m)^[ \t]*extern\s+"C"\s+[^;{}()=]*?\b(_ZN\w+)\s*\([^;{}]*\)\s*;', text)
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
    kept = set()
    for name in names:
        if same(apply_names(text, kept | {name})):
            kept.add(name)
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
        found = sorted({k for _, k, _ in plausible.findings(text)} & FIXABLE)
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
    args = [a for a in sys.argv[1:] if a != "--tree"]
    if len(args) not in (0, 2) or (args and args[0] != "-j"):
        sys.exit(USAGE)
    os.makedirs(CLAIMS, exist_ok=True)
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
