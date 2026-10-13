# Cloud matching session

A Claude Code cloud session matches one batch of DQIX functions and pushes the results to a branch
of this repository. The local coordinator has already reserved the batch in a kit issue, and it
does the rest:
- re-gating
- integration
- the pull request
- releasing the reservation

So this session never touches GitHub issues, pull requests or the decomp repository's remote.

Start the session with this prompt, replacing `<id>`:

```
Run the DQIX cloud batch <id> by following cloud/SESSION.md exactly.
```

## Steps

1. **Bootstrap.**
   - Run `bash cloud/bootstrap.sh` as a background command; it takes several minutes.
   - It must end with `BOOTSTRAP OK`. If it fails, stop and report the last 30 lines.
   - From here on, run every kit command as `/opt/dqix-venv/bin/python <script>` from the repository
     root, with `DQIX_NO_FRESHNESS=1` set.
2. **Sanity gate.** Prove the toolchain on a function that is already landed:

       WGATE_ALLOW_COMMITTED=1 /opt/dqix-venv/bin/python wgate.py 017 021d27e4 "$(cd ../dqix-decomp && pwd)/src/Combat/Overlay_17/func_ov017_021d27e4.cpp"

   It must print `MATCH`. If it does not, stop and report the full output: the Linux toolchain is
   broken, and matching would waste the budget.
3. **Load and prepare the batch.**
   - Run `python cloud/batch.py load cloud/batches/<id>.tsv`, then `python codex/team.py prep --n <rows> --prefix c`.
   - Each line printed is `<label> <addr> <task file>`. A line ending `PREWORK-MATCH` needs no worker.
4. **Match.**
   - Spawn one background subagent per task file, all in one message, up to 15 at once.
   - A fourth column in the batch file names the model for that row; it wins over everything.
   - If `cloud/batches/<id>.model` exists, use the model it names for every worker. Otherwise
     choose the model by function size (the task file says it):
     - 1100 bytes or more: `opus`
     - 600 to 1099 bytes: `sonnet`
     - smaller: `haiku`
   - Give each worker this prompt:

     ```
     Read <absolute task file> and do exactly what it says. Up to 50 gate runs (opus) or 40
     (sonnet/haiku); stop early after 10 runs in a row with no reduction. Do not write permutation
     or search scripts that compile hundreds of variants; one colorsweep --apply pass is enough. Print the full target
     with `"<runner>" wlist.py <mod> <addr>` and build section by section: size first, then shape,
     then registers. Keep temp files only in /tmp/<label>/. Quality bar for MATCH:
     - the first line is `#include <globaldefs.h>`
     - typed structs with named fields, with at most 2 raw `*(T*)((char*)p+off)` reads
     - no comments except the USA tag
     - volatile only via existing hardware-register defines (e.g. DISPCNT in System/Graphics.h)
     - no self-assignment or cast tricks to force loads or stores
     - copy a named data symbol rather than a local initialized array
     On WRONG-SYMBOL, rename the definition to the tagged symbol. If the listing is hand-written asm
     or speed-built library code, stop early and report RESIDUE with that note.
     ```

   - When more than 15 tasks are ready, start the rest as workers finish, so 15 run at a time.
   - A task whose source was seeded from an earlier attempt starts from that near-miss: tell the
     worker to improve the existing source rather than rewrite it, unless it is far off.
   - Never spawn a second worker on the same address.
5. **Collect.**
   - When a worker finishes, run `python codex/team.py collect <addr>`. It re-gates and applies the
     quality bar.
   - A `RESIDUE QUALITY` line names something you can fix yourself in a minute (include order, a
     known define). Fix it and collect again. Otherwise leave it.
6. **Export and push.** Do this when every worker has finished, and also if the session must stop
   early:

       python cloud/batch.py export <id>
       git checkout -b cloud/<id>
       git add cloud/results/<id>
       git commit -m "Cloud batch <id> results"
       git push -u origin cloud/<id>

7. **Report.**
   - For each address: its verdict, and the last gate line for a residue.
   - The branch you pushed.
   - Anything that broke in the toolchain.

## Rules

- Never write into `../dqix-decomp/src`, never commit the ROM or anything under `extract/`, and
  never print the `DQIX_ROM_URL` value.
- No inline asm, no codegen `#pragma`, no compiler flag overrides.
- Do not open issues or pull requests, and do not push anywhere except `cloud/<id>` here.
