import os
import re

CODEGEN_PRAGMA = re.compile(r"(?m)^[ \t]*#[ \t]*pragma[ \t]+(?!(?:define_section|section|once)\b)(\w+)")
HAND_ASM = re.compile(r'(?m)^\s*asm\b|\basm\s+(?:void|int|unsigned|char|long|short)\b')
ASM_ALLOW_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "asm_allow.txt")


def _allow_rows():
    try:
        lines = open(ASM_ALLOW_PATH, encoding="utf-8").read().splitlines()
    except OSError:
        return []
    return [parts for parts in (line.split("#", 1)[0].split() for line in lines) if parts]


def asm_allow():
    return {parts[0].lower() for parts in _allow_rows()}


def asm_allow_symbols():
    out = set()
    for parts in _allow_rows():
        if len(parts) >= 2:
            out.add(parts[1])
            m = re.match(r"_Z(?:N)?(\d+)", parts[1])
            if m:
                out.add(parts[1][m.end():m.end() + int(m.group(1))])
    return out


def refused_asm(addr, path, text):
    return (path.lower().endswith(".s") or bool(HAND_ASM.search(text))) and addr.lower() not in asm_allow()
