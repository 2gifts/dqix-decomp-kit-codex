#!/usr/bin/env python3
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
# The compiler, flags and include paths the BUILD uses, read from tools/configure.py itself.
# wgate, classify and the repair sweeps each carried their own copy of the flag list, so the
# rebase onto sp2p2 left every one of them measuring on a compiler the ROM is no longer built
# with. Importing the build's own definitions is the only arrangement they cannot drift from.
import ast
import os
import re
import sys

REPO = _kp.REPO
_TOOLS = os.path.join(REPO, "tools")
# usa, jpn, or eur. Unset stays usa, which is every current config/ and extract/ path.
REGION = os.environ.get("DQIX_REGION", "usa")


def _load():
    cwd, argv = os.getcwd(), sys.argv[:]
    os.chdir(REPO)
    sys.argv = ["configure.py", REGION]
    sys.path.insert(0, _TOOLS)
    try:
        import configure
        return configure
    finally:
        os.chdir(cwd)
        sys.argv = argv
        if _TOOLS in sys.path:
            sys.path.remove(_TOOLS)


def _mwcc_defines(configure):
    '''The -d flags on configure.py's mwcc rule, for the region it was loaded with.

    $game_version is the ninja variable that rule uses for usa and jpn. The gate
    does not run ninja, so that variable is expanded to the loaded region.
    '''
    path = configure.__file__
    source = open(path, encoding="utf-8").read()
    tree = ast.parse(source)
    for node in ast.walk(tree):
        if not isinstance(node, ast.Assign):
            continue
        for target in node.targets:
            if isinstance(target, ast.Name) and target.id == "region_defines":
                value = eval(compile(ast.Expression(node.value), path, "eval"), configure.__dict__)
                return value.replace("$game_version", REGION)
    return f"-d {REGION}"


_cfg = _load()

EXE = ".exe" if os.name == "nt" else ""


def native(tool):
    """An argv[0] that runs a Windows toolchain binary here. On Linux the build runs them through
    wibo, so callers get a wrapper script that does the same."""
    if os.name == "nt":
        return tool
    import hashlib
    import shutil
    wibo, d = None, os.path.dirname(tool)
    while not wibo and os.path.dirname(d) != d:
        wibo = os.path.join(d, "wibo") if os.path.isfile(os.path.join(d, "wibo")) else None
        d = os.path.dirname(d)
    wibo = wibo or shutil.which("wibo")
    if not wibo:
        sys.exit(f"wibo not found next to {tool} or on PATH; run ninja in the decomp checkout first")
    wrap = os.path.join(_kp.SP, "bin", hashlib.sha1(tool.encode()).hexdigest()[:12] + "-" + os.path.basename(tool))
    body = f'#!/bin/sh\nexec "{wibo}" "{tool}" "$@"\n'
    if not os.path.exists(wrap) or open(wrap).read() != body:
        os.makedirs(os.path.dirname(wrap), exist_ok=True)
        with open(wrap, "w") as fh:
            fh.write(body)
        os.chmod(wrap, 0o755)
    return wrap


MWCC_VERSION = _cfg.MWCC_VERSION
DECOMP_ME_COMPILER = _cfg.DECOMP_ME_COMPILER
CC = native(f"{REPO}/tools/mwccarm/{MWCC_VERSION}/mwccarm.exe")
AS = native(f"{REPO}/tools/mwccarm/{MWCC_VERSION}/mwasmarm.exe")
AS_FLAGS = _cfg.AS_FLAGS.split()
FLAGS = (_cfg.CC_FLAGS + " " + _cfg.CC_INCLUDES + " " + _mwcc_defines(_cfg)).split()
CODEGEN_PRAGMA = re.compile(r"(?m)^[ \t]*#[ \t]*pragma[ \t]+(?!(?:define_section|section|once)\b)(\w+)")


def config_dir(mod):
    root = f"config/{REGION}/arm9"
    return root if mod == "main" else f"{root}/overlays/ov{mod}"


def pristine(mod):
    if mod == "main":
        return f"extract/{REGION}/arm9/arm9.bin"
    return f"extract/{REGION}/arm9_overlays/ov{mod}.bin"


SDK_LCF_SYMBOLS = {"SDK_IRQ_STACKSIZE": 0x400, "SDK_SYS_STACKSIZE": 0}


def lcf_symbols():
    overlays = [d for d in os.listdir(f"{REPO}/{config_dir('main')}/overlays") if re.fullmatch(r"ov\d+", d)]
    return {**SDK_LCF_SYMBOLS, **{f"OVERLAY_{int(d[2:])}_ID": int(d[2:]) for d in overlays}}


def cc_path(version):
    return native(f"{REPO}/tools/mwccarm/{version}/mwccarm.exe") if version else CC


if __name__ == "__main__":
    if "--cc" in sys.argv:
        print(CC)
    elif "--flags" in sys.argv:
        print(" ".join(FLAGS))
    else:
        print("MWCC", MWCC_VERSION)
        print("CC  ", CC)
        print("FLAGS", " ".join(FLAGS))
