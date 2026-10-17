# Reading the compiler

When a residue survives every source form, stop generating forms and find the compiler decision
behind it. Locate the routine that makes the decision, read what it reads, and turn that input into
a source property. This is how plateaus close: `ov026:021d8ba0` (8896 bytes) sat at 11 bytes through
nine evolve runs and about 17,000 compiled spellings. One afternoon in the backend found the cause,
a store-to-store alias edge, and the source form that removes it.

Go here when:

- an evolve run plateaus or `evocap.py` prints `EVOCAP STOP` (rule: never a second evolve run);
- a `REGPERM` or `SCHED` residue survives `colorsweep.py`, `frida/colorforce.py` (with `CF_PAIRS=1`)
  and `frida/schedforce.py`;
- an `UNDERGEN`/`OVERGEN` residue where the source looks right: the compiler spilled, skipped or
  merged code and you need to know why.

Before you start, audit the file for constructs no developer would write (pragmas, `volatile` to
force order, invented macros or templates, fake arguments; `python plausible.py <file>`) and work the
cleaned source, even if it scores worse. Invented code moves residues around and makes any reading
of the compiler wrong. `main:02061c04` stayed at 27 bytes for ~1000 forms under a file-wide
`#pragma opt_propagation off` and closed only after the pragma was removed.

## What you need

| what | where |
|---|---|
| the compiler | `$DQIX_REPO/tools/mwccarm/2.0/sp2p2/mwccarm.exe`, x86 PE32. Every address below belongs to this file. |
| Frida | [SETUP.md](SETUP.md#frida-optional); on Linux, `frida/proton.py` |
| Ghidra | 11.1.2 or later, with JDK 21 (`JAVA_HOME`) |
| a reference | github `inspiredrobot/mwcc`, cloned to `$SP/refs/mwcc-gc125`: the PowerPC CodeWarrior backend, decompiled. Same structure and pass names; `docs/PASS_PIPELINE.md` and `docs/SCHEDULER.md` |

Import and analyse the compiler once (about two minutes):

    analyzeHeadless <proj-dir> mwccarm -import $DQIX_REPO/tools/mwccarm/2.0/sp2p2/mwccarm.exe

Then decompile the functions that contain any addresses, one `.c` file per address, in a few seconds:

    analyzeHeadless <proj-dir> mwccarm -process mwccarm.exe -noanalysis -readOnly \
        -scriptPath $KIT/ghidra -postScript DecompAt.py <out-dir> 4ff700 513a50

`analyzeHeadless` is `support/analyzeHeadless.bat` on Windows. The pyghidra MCP server
(`decompile_function`, `list_xrefs`, `read_bytes`) does the same interactively.

Asserts give away the source file: `FUN_0044d0e0("IrOptimizer.c", 0xaf)`. Search the strings for
`.c` and `.cpp` names (`SchedulerARM9.c`, `Scheduler.c`, `Coloring.c`, `InterferenceGraph.c`,
`IroLifetime.c`, `Alias.c`, `CFunc.cpp`) and follow their xrefs to put a name to an unknown routine. Then read the
routine with the same name in `refs/mwcc-gc125`.

## The backend, in order

| stage | address | hook / tool |
|---|---|---|
| IR optimiser driver (`IrOptimizer.c`) | `0x4f4110` | |
| skip every IR pass when the function has inline asm or an alignment-qualified type | `0x545a20` | |
| IR common subexpressions | `0x57a360` | |
| function-scope `static` marks its object (byte `+0x42`) | `0x4a5e00` | |
| pcode generation, end of statement: the block ends once it has more than 100 pcodes | `0x4ffe20` | `pad/renum/genct.py` |
| if-conversion: per function / per candidate / limit | `0x5031e0` / `0x5025f0` / `0x502400` | `pad/renum/ifcvtrace.py` |
| scheduler, runs twice: pass 1 on virtual registers, final pass on physical | `0x4ffa30` | `frida/schedtrace.py` |
| scheduler, one block | `0x4ff8c0` | |
| scheduler pick | `0x4ff700` | `frida/schedforce.py` |
| add a dependency edge (kind = return address, see `schedforce.py`) | `0x4ff030` | `frida/schedforce.py` |
| memory dependence, load / store | `0x4ff250` / `0x4ff2b0` | `frida/forcenoalias.py`, `pad/renum/aliasdump.py` |
| pass 1 pressure mode on (live >= limit; limit `[0x61667c]`, live `[0x616680]`) | `0x4feb70` | `pad/renum/pressforce.py` |
| pressure-mode score | `0x4feba0` | `pad/renum/schedwhy.py` |
| may-alias | `0x559c40` | `pad/renum/aliasdump.py` |
| make alias record; returns the worst-case set `[0x63bb8c]` when the lookup overruns the object | `0x559670` | |
| alias propagation (ARM), adds the access offset to the record | `0x592200` -> `0x559a00` | |
| coalescing | `0x50b790` | `pad/renum/forcemerge.py` |
| colouring driver | `0x513dc0` | |
| colour the graph | `0x513a50` | `frida/colorforce.py` replaces it |
| simplify: scan vregs in ascending order, push nodes below K | `0x5136b0` | |
| K: allocatable registers in a class | `0x4fe540` | |
| stage dump stub, `(char* func, char* label)`; gate byte `0x63d59c` | `0x505330` | |

The labels the stage dump reports, in order: pass-1 scheduling, `AFTER EXPAND STACK RELATIVE LOADS
AND STORES`, colouring, `AFTER REGISTER COLORING`, `BEFORE`/`AFTER PEEPHOLE`, conditional, branch,
head-tail merge and branch-to-epilogue optimisations, the final scheduling pass, `FINAL CODE`,
`AFTER CONSTANT POOLING`. Initialisation sets the gate byte to 0, so a hook has to set it back.
Frida cannot `Interceptor.attach` the stub (it is a single `ret`), so patch a `jmp` to a
`NativeCallback` into the padding after it.

Globals: block list head `[0x63a828]`, current block `[0x63a224]`, vreg count `[0x63a364]`, colouring
nodes `[0x63ba40]`, register class `[0x64669d]`.

**Blocks:** next block at `+0`, first pcode at `+0x14`, pcode count at `+0x28`.

**Pcodes:** next at `+0`, flags at `+0xc` (`0x4` store, `0x2` load, `0x208` call barrier), alias
record at `+0x1c`, opcode at `+0x28` (`0x80` pool or address load, `0xa5` ldr, `0xee` str, `0xb2`
mov#, `0xbd` mvn#, `0x76` bl, `0x7b` mov reg, `0x7a`/`0x71` branches), condition at `+0x2a`,
addressing mode at `+0x2b`, operand count at `+0x2c`. Operands start at `+0x2e`, 14 bytes each:
`{kind u8, class s8, access u16 (1 use, 2 def), reg s16, ...}`.

## The procedure

1. **Shrink the residue to one decision.** Use `wdiff.py`, `pad/casediff.py` or
   `pad/shiftdiff.py` to get the exact window, ROM against ours. Say what differs: emission
   order, register choice, an instruction present or absent. Check that allocation and store order
   agree, and that only one thing is left.
2. **Build a lab.** Write a small function holding the window's context, compiled alone
   (`pad/probe_cc.py`, below a second each). Before you trust the lab, show it emits the real
   function's window: a reduced case in a different regime gives false negatives. 11,520 lab
   spellings for `021d8ba0` all missed because the lab hoisted the constant one slot less than the
   real function.
3. **Name the decision.** Find which compiler decision, forced the other way, gives the ROM's
   bytes. The forcing tools do this for colouring, scheduler picks, dependency edges, pressure mode,
   vreg numbering and coalescing. Where no tool hooks the stage, write a hook (below). The proof is
   causal: forcing that one decision makes the real function exact.
4. **Read the deciding routine.** Decompile it and follow its inputs back until one of them is a
   property of the source: an object's storage class, a struct's size, a type qualifier, the order
   in which values are created, a block boundary. Trace the routine live with Frida to confirm the
   reading.
5. **Write the source form.** Change only that property, using a form the developers could have
   written, and gate the real function. Check every condition the routine reads. Properties that
   are required together give a search no gradient, because each one alone scores the same as
   neither.
6. **Promote it.** Write a `core.md` rule that cites the address (rules 16 and 19), put the tool in
   `frida/` or `pad/renum/` with an `INVENTORY.md` line, and add a row to the table above.

### Writing a hook

Start from `frida/schedforce.py`. It spawns the build's compiler for one source with
`buildcfg.py`'s flags, attaches before the backend runs, and checks that the hooked compile
reproduces the native one. Then:

- `Interceptor.attach(ptr('0x...'), {onEnter, onLeave})` to observe a routine, and `send()` what it
  reads;
- `Interceptor.replace` with a `NativeCallback` that calls the original `NativeFunction`, to change
  one decision (`colorforce.py` replaces colouring with a JavaScript replay of it);
- write a forced value straight into a global (`pressforce.py` adjusts the pressure limit for each
  block);
- calling convention `'mscdecl'`.

Each compile takes about 2 seconds, and independent compiles run in parallel. Inside a sandbox Frida
cannot attach and hangs while holding a core ([NATIVE_AGENTS.md](NATIVE_AGENTS.md)).

## Worked example: `ov026:021d8ba0`

**Residue.** 11 bytes at `+0x2bc`, in a state transition that stores into a global, then into
`self`:

    ROM   mov r1,#3 ; ldr r0,[pc] ; mvn r2,#0 ; str r2,[r0,#0x18] ; str r1,[sl,#0xeac]
    ours  ldr r0,[pc] ; mvn r2,#0 ; mov r1,#3 ; str r2,[r0,#0x18] ; str r1,[sl,#0xeac]

Registers, instructions and store order all agree. The only difference is how far `mov r1,#3` is
hoisted (three slots in the ROM, one in ours), so this is the in-block scheduler. Everything blind
search could try was spent first, and all of it was inert: ~2000 permuted spellings, 3675
declaration permutations, all 25 compiler builds, ~50 flag axes, all 29 `-proc` models, an
11,520-spelling lab search, and removing every invented construct in the file.

**Decision.** `frida/schedtrace.py` logged the final scheduling pass at that block. When the
decision is made, the pool load, `mvn` and `mov #3` are all overdue with release count 0, so height
decides and puts the pool load first. `mov #3` cannot win because the store it feeds,
`str r1,[sl,#0xeac]`, waits on a store-to-store edge from `str r2,[r0,#0x18]`.
`frida/forcereal.py ov026 pass2` drops store-to-store alias edges in the final pass, and with that
the whole 0x22c0-byte function is code-exact. That is the causal proof.

**Routine.** The edge comes from `0x4ff2b0` asking may-alias `0x559c40`, and both stores carry the
same alias record: the per-function worst-case set, which holds every non-const global. The global
store lands in that set for two reasons:

- alias propagation `0x592200` adds the store's `#0x18` to a record that already holds the member
  offset, asks for offset 0x30 in a 0x1c-byte `extern`, and `0x559670` falls back to worst-case;
- even a precise record stays in the worst-case set. `0x559670` exempts only locals, `const`
  objects, a type flag, the unknown-object type, and objects with byte `+0x42` set. `CFunc` sets
  that byte (`0x4a5e00`) only for a function-scope `static`.

**Source.** The object is a function-scope static, sized to the gap before the next symbol (0x3c),
so the doubled offset stays inside it:
`inline RowLabels& GetRowLabels() { static RowLabels s = {...}; return s; }`. It sits in an inline
accessor because a static in the function body is the local symbol `s$N`, which the other functions
that read it cannot link to. Scope and size were each necessary. Either one alone scores exactly
11, the same as neither, so no search operator could have found this. Landed `2424ebd7`; the rule
is `core.md` "A CONSTANT HOISTED ABOVE A GLOBAL'S POOL LOAD".

The same readings give the rest of [LESSONS.md](LESSONS.md):

- vreg numbering decides callee-saved colouring (simplify `0x5136b0`, `pad/renum/renum.py`);
- the 100-pcode block split (`0x4ffe20`, `genct.py`);
- the IR skip on alignment-qualified types (`0x545a20`);
- pass-1 tie-breaks and pressure mode (`0x4ff700`, `0x4feb70`).
