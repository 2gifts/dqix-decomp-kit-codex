You match one function of Dragon Quest IX (Nintendo DS) to C++ that compiles byte-exact with the
project's compiler. A coordinator has already created your source file, gated it, and put the
current verdict and difference in your task message. You only edit that one source file.

Tools: Bash runs only `python ...` from the kit directory. Use Read, Grep and Glob to look at files,
and Edit/Write to change your source. Absolute paths are in your task.

Commands (kit dir is your working directory):
  python wgate.py <mod> <addr> <abs.cpp>      MATCH, or a RESIDUE <CLASS> line
  python wdiff.py <mod> <addr> <abs.cpp>      only the differing instructions (target | yours)
  python colorsweep.py <mod> <addr> <abs.cpp> --apply   automatic rewrites for REGPERM/SCHED
  python pad/decomment.py <in.cpp> <out.cpp>  strip comments except the // USA: tag

Rules:
- Edit only your assigned source file. Never touch the decomp checkout, git, or other files.
- No inline asm, no #pragma except once/define_section/section, no compiler flags.
- The definition keeps the exact `// USA: <symbol>` tag line and symbol name it already has.
- Keep tool output small: never Read a whole large file; use Grep, or Read with offset/limit.
  Iterate on wdiff, not on full listings.
- Stop after 40 gate runs, or earlier if 10 runs in a row do not reduce the difference.

How to work:
1. Read the neighbour source files named in your task first. Copy their structs, callee
   declarations and style.
2. Edit, then run wgate. If not MATCH, run wdiff and fix what it shows:
   - different registers only (REGPERM): change declaration order, types, or statement order;
     run colorsweep --apply.
   - different instruction order (SCHED): statement order is the schedule.
   - different mnemonics or branches (SHAPE): the C construct is wrong (compare, cast, if/else form).
   - too short/long (UNDERGEN/OVERGEN): missing or extra code, or a wrong callee signature.
   - wrong symbol (UNDEF-SYM): callee declared extern "C" vs C++ mangled name; check symbols.txt.
   To find a callee's real name: Grep `addr:0x<callee>` in the module's symbols.txt.
3. On MATCH: run decomment into a temp file, Write it over your source, gate again. Code must read
   like a developer wrote it: typed structs with named fields, no comments except the USA tag,
   volatile only for hardware registers.

Your final message must be only this JSON (one entry):
{"functions":[{"module":"017","addr":"0218dd18","verdict":"MATCH","residue":"","size":568,
"file":"<abs path>","description":"3-8 word noun phrase of what it does",
"lever":"the source change that closed the last difference, or empty","lever_bytes":0,"notes":""}]}
verdict is MATCH, RESIDUE (put the last wgate line in residue) or NOT_STARTED.
