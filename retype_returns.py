import collections
import re
import sys

import addrname
import kitpaths
import mechclean
import plausible

USAGE = """python retype_returns.py [-j N] [--dry]

Retypes a free function's return value to the class every call casts its result to (`(Entry*)Lookup(i)`,
`static_cast<Entry*>(Lookup(i))`), when the function returns a placeholder (void*, char*, unsigned char*,
int, unsigned int): its definition and every declaration, a file that never names the class given
`struct <class>;`, and the casts dropped. The return type is not part of a free function's symbol, so no
rename. Kept per function only when every translation unit it touches compiles to the same object in every
region; rewrites go to $SP/clean/manifest.tsv for landclean.sh. --dry prints the candidates."""
PLACEHOLDER = {"void*", "char*", "unsignedchar*", "int", "unsignedint"}
CAST_CALL = re.compile(r"(\(\s*(?:const\s+)?(?:struct\s+|class\s+)?([A-Za-z_]\w*(?:::\w+)*)\s*\*\s*\)\s*|"
                       r"static_cast\s*<\s*(?:const\s+)?([A-Za-z_]\w*(?:::\w+)*)\s*\*\s*>\s*\(\s*)"
                       r"([A-Za-z_]\w*)\s*\(")
DEFINITION = r"(?m)^((?:extern\s+\"C\"\s+)?(?:ARM\s+|THUMB\s+|static\s+|inline\s+)*)([^;{}()=#\n]*?)\b%s\s*(\([^;{}]*\))\s*\{"
DECLARATION = r"(?m)^([ \t]*(?:extern\s+\"C\"\s+)?)([^;{}()=#\n]*?)\b%s\s*(\([^;{}]*\))\s*;"


def call_end(text, at):
    depth, i = 1, at
    while depth and i < len(text):
        depth += {"(": 1, ")": -1}.get(text[i], 0)
        i += 1
    return i


def candidates(original):
    casts = collections.defaultdict(set)
    for rel, text in original.items():
        if rel.endswith((".c", ".cpp")):
            for m in CAST_CALL.finditer(plausible.strip_code(text)):
                target = m.group(2) or m.group(3)
                if target not in mechclean.BUILTIN_WORDS:
                    casts[m.group(4)].add(target)
    defined = {}
    for rel, text in original.items():
        if rel.endswith((".c", ".cpp")):
            for name in [n for n in casts if n in text]:
                d = re.search(DEFINITION % re.escape(name), text)
                if d and "::" not in d.group(2):
                    defined.setdefault(name, (rel, d.group(2).strip()))
    out = {}
    for name, targets in casts.items():
        if len(targets) == 1 and name in defined:
            ret = re.sub(r"\b(?:struct|class)\s+", "", defined[name][1]).strip()
            if mechclean.norm_type(ret + " x") in PLACEHOLDER:
                out[name] = (next(iter(targets)), ret)
    return out


def rewrite(text, name, target, ret):
    before = text
    for pattern in (DEFINITION, DECLARATION):
        def fix(m):
            if mechclean.norm_type(re.sub(r"\b(?:struct|class)\s+", "", m.group(2)).strip() + " x") != \
                    mechclean.norm_type(ret + " x"):
                return m.group(0)
            return m.group(0).replace(m.group(2), "%s* " % target, 1)
        text = re.sub(pattern % re.escape(name), fix, text)
    pieces, last = [], 0
    for m in CAST_CALL.finditer(text):
        if m.group(4) != name or (m.group(2) or m.group(3)) != target:
            continue
        end = call_end(text, m.end())
        call = text[m.start(4):end]
        if m.group(3):
            close = text.find(")", end)
            if close < 0 or text[end:close].strip():
                continue
            pieces.append(text[last:m.start()] + call)
            last = close + 1
        else:
            pieces.append(text[last:m.start()] + call)
            last = end
    text = "".join(pieces) + text[last:]
    text = re.sub(r"\(\s*(%s\s*\([^()]*\))\s*\)(?=\s*(?:->|\.|\[|\)|;|,))" % re.escape(name), r"\1", text)
    return addrname.declare(text, before, ["%s*" % target]) if text != before else text


def main():
    args = sys.argv[1:]
    jobs = int(args[args.index("-j") + 1]) if "-j" in args else 8
    if any(a.startswith("-") and a not in ("-j", "--dry") for a in args):
        sys.exit(USAGE)
    blobs, original = mechclean.tree_texts()
    found = candidates(original)
    print(f"functions whose calls all cast the result to one class: {len(found)}", flush=True)
    if "--dry" in args:
        for name, (target, ret) in sorted(found.items()):
            print(f"{name}\t{ret} -> {target}*")
        return 0

    def build(names):
        new = {}
        for rel, text in original.items():
            if not rel.endswith(addrname.SOURCES):
                continue
            out = text
            for name in names:
                if name in out:
                    out = rewrite(out, name, *found[name])
            if out != text:
                new[rel] = out
        return new

    names = set(found)
    while names:
        new = build(names)
        bad, seen = mechclean.verify(original, new, {}, jobs, f"{kitpaths.SP}/handwork/retype_returns")
        if not bad:
            break
        suspects = {n for rel in bad for n in names
                    if n in original[rel] + "".join(original[h] for h in seen[rel])}
        drop = {n for n in suspects if len(suspects) == 1 or
                mechclean.verify(original, build({n}), {}, jobs, f"{kitpaths.SP}/handwork/retype_returns")[0]} \
            or suspects
        print(f"{len(bad)} unit(s) differ; dropping {len(drop)} of {len(suspects)} function(s) there", flush=True)
        names -= drop
    if not names:
        print("retyped 0 function(s)")
        return 0
    if not mechclean.queue(new, blobs, "retype"):
        return 1
    print(f"retyped {len(names)} function(s) in {len(new)} file(s): {', '.join(sorted(names))}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
