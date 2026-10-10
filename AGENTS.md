# DQIX decomp kit, Codex fork

This is `2gifts/dqix-decomp-kit-codex`, a fork of `ZevyaDev/dqix-decomp-kit` tuned for Codex. It is
maintained by its owner, who merges upstream into it. Matched functions still go to
`ZevyaDev/dqix-decomp` (branch `decomp-matching`), and reservations still go to issues on
`ZevyaDev/dqix-decomp-kit`, exactly as for every other contributor.

## Start here (every Codex session)

The job is matched functions landing upstream in large, clean pull requests, and the levers that
closed them going back to the kit. The only kit pull requests this team sends are recipes: one
`worker_src/core.md` rule per PR, citing a landed address whose cause you isolated (`dqix-team`
step 5, `docs/IMPROVEMENT_LOOP.md`). No other kit edits, no tooling projects. If a kit tool is
broken, tell the user the command and the output line, and work on something else.

1. `python kit_update.py`, then `python codex/team.py sync` and `python codex/team.py status`. Run
   anything that talks to GitHub (`kit_update.py`, `team.py sync`/`candidates`/`reserve`/`issue`/
   `integrate`/`pr`/`release`, `gh`, `git fetch`/`push`) with escalated permissions from the first
   attempt: the Windows sandbox cannot see the GitHub login, so sandboxed it fails with
   `FETCH FAILED` or an invalid gh token. If
   `python` cannot import `capstone`, the workspace is not set up (`docs/CODEX_FORK.md`, Setup):
   tell the user instead of installing packages yourself.
2. Load the `dqix-team` skill (`.agents/skills/dqix-team/SKILL.md`) and follow it. It is the whole
   loop:
   - reserve with ONE issue, published before any work
   - match in groups of 4 per worker (`dqix-matcher` subagents, or `team.py dispatch` for a
     `codex exec` fleet)
   - `team.py collect` re-gates every reported MATCH and applies the quality bar
   - integrate and publish every 40-120 matches with `team.py integrate` and `team.py pr --open`,
     which also closes the issue with the PR link
   - `team.py release` when stopping
3. A worker (a spawned subagent, or a session asked to match specific addresses) reads
   `codex/WORKER.md` and follows only that.

What a reviewer accepts, and what this fork enforces:
- byte-exact through `wgate.py` with the default compiler
- no asm, no codegen pragma, no `volatile` except hardware-register defines
- no comments except the `// USA:` tag
- typed structs with named fields, in the style of the neighbouring matched files
- the PR template's two checklist lines ticked
- the reservation issue linked
- `prready.py decomp` READY

Write files with your edit tool (`apply_patch`), never through the shell. On Windows,
PowerShell 5's `>`, `Set-Content` and `Out-File` write ANSI or UTF-16 and corrupt sources.
Kit scripts are Python; run them as `python <script>` from this directory in any shell. Only the
`.sh` scripts need Git Bash, and `team.py` calls those itself.

---

# Reference (shared with the upstream kit)

This directory is `$KIT`, the kit checkout: scripts, docs and skills only. Everything the pipeline
produces (attempts, logs, claims, staging, worker docs, knobs, `OPEN_WORK.md`, `STATE.md`) lives in
`$SP`, the state directory outside the checkout: `$DQIX_STATE`, else the path in `state.path`, else
`../dqix-kit-state`. The decomp checkout is `$DQIX_REPO` (default `../dqix-decomp`, branch
`decomp-matching`). Every script resolves all three through `kitpaths.py`. In Git Bash, from this
directory:

    export KIT="$(pwd -W 2>/dev/null || pwd)"
    export SP="$(python kitpaths.py state)"

A session that runs from another directory sets `DQIX_KIT` to the checkout; skills and workflows
read it.

## Update the kit on every stop

Update at the start of every session and every time you stop:

    python kit_update.py

Claude Code sessions opened in this directory run it automatically (`.claude/settings.json`: a
SessionStart hook, and a Stop hook that keeps you working if an instruction file changed). Every
other agent (Codex, MiniMax, any model) runs it itself; nothing reminds you but this: every kit
script prints `KIT IS N COMMIT(S) BEHIND` while the checkout is stale, and `claim.py` hands out no
new work (exit 3) until you update. Your own unpublished kit commits are kept on top of each update.

| exit | meaning | do |
|---|---|---|
| 0 | up to date, or updated and `kit_init.py` re-run | re-read every file it prints as `RE-READ`, then work |
| 1 | the fetch failed (offline), or `kit_init.py` failed after an update | tell the user the line it printed; fix a `kit_init.py` FAIL before working |
| 2 | the fleet or an integration is running, so nothing was pulled | nothing to do: the fleet drains and updates itself (docs/FLEET.md); never pull under a running script |
| 3 | uncommitted changes, or your unpublished commits conflict with the update | tell the user; never stash, reset or discard them yourself |

## Changing the kit

Codex sessions change this fork never, and upstream only through recipe pull requests to
ZevyaDev/dqix-decomp-kit (`dqix-team` step 5): one `core.md` rule citing a landed address,
`prready.py kit` READY. Push recipe branches from a clean checkout of upstream `main`, not from
this fork. The fork's owner changes the fork and merges upstream into it (`docs/CODEX_FORK.md`). If a script fails, or a
rule here contradicts what the tools do, stop that line of work and report the exact command and
output to the user.

**Pull both before every pull request** (rule 21). A pull request built on a stale kit or a stale
decomp silently reverts what landed since you started, re-adds files for addresses someone else
already owns, records dead ends for functions already matched and cites matches that never landed.
It is sent back, and the review it cost is wasted.

## Where things are

| need | read |
|---|---|
| state of a fresh session's work | `OPEN_WORK.md` first, then `STATE.md` (`python progress.py --print`) |
| one function, start to commit | `docs/WORKFLOW.md` |
| the fleet, knobs, stopping | `docs/FLEET.md` |
| compiler facts and pipeline rules | `docs/LESSONS.md` |
| every codegen lever | `worker_src/core.md` (1400 lines: grep a heading, never read whole) |
| forms already ruled out per address | `worker_src/deadends.md` |
| what each script does | `INVENTORY.md` |
| named residues nobody cracked | `OPEN_RESIDUES.md` (grep an address) |
| closest saved attempt per address | `priors/INDEX.tsv`, `python resumable.py <addr>` |
| naming already-matched functions | `naming/HANDOFF.md` |
| finished experiments cited as evidence | `archive/README.md` (nothing runs them) |

## Skills

`python kit_init.py` copies skills into `.agents/skills/`, where Codex loads them. The sources are
`codex/skills/` for the Codex ones and `.claude/skills/` for the shared ones. Edit the source, never
the copy.

| skill | use |
|---|---|
| `dqix-team` | the coordinator loop: reserve, dispatch, collect, integrate, publish, release |
| `dqix-worker` | match an assigned group as a worker (`codex/WORKER.md`) |
| `dqix-hand-match <addr>` | close one stubborn function in this session |
| `dqix-status` | where things are: coverage, what moved, what needs a decision |
| `dqix-stop` | stop everything now and verify nothing survived |
| `dqix-coordinate` | upstream's native-agent crack/evolve experiments (bounded, only when asked) |

Project config in `.codex/`:
- `agents/dqix-matcher.toml` is the worker subagent.
- `hooks.json` runs `kit_update.py` at session start, and refuses `gh pr create` until
  `prready.py` is READY. Review and trust it once with `/hooks`.

Upstream's Claude fleet (`pull_all.sh`, `claude -p` workers) and the `.claude/workflows/` need the
Claude Code CLI. Codex does not use them; `codex/team.py dispatch` is the Codex fleet.

## Hard rules

1. Source being matched lives in `$SP/wip/<main|ovNNN>/`; scratch in `$SP/handwork/`. Never write
   into `$DQIX_REPO/src/`. Never put scratch in `staging/`, `hold_*/`, `gated/` or `attempts/`:
   those are pipeline inputs.
2. Create and change files with your file-editing tool. Never with a shell heredoc or `sed -i`.
3. Pass absolute source paths to `wgate.py` and `wdiff.py`; they change into the decomp directory.
4. A match lands only through `staging/<main|ovNNN>/` and `finish_wave.sh <main|NNN>` (or
   `integrate_fast.sh`). Only a commit that passes `ninja check` proves a match.
5. One integration at a time: `finish_wave.sh` and `integrate_fast.sh` take `$SP/wave.lock`. Never
   run `integrate.py` (even `--dry`) by hand while it exists. Gating, claiming and sweeping go on
   during an integration.
6. Launch integrations as a background task with a long timeout and the command passed plain: no
   `nohup`, no trailing `&`.
7. Integrations run in their own worktree (`python integ_tree.py path`) from the tip of
   `decomp-matching`, then fast-forward the decomp checkout. Uncommitted edits in the checkout are
   not part of them: commit what a match needs first.
8. No hand assembly except addresses in `asm_allow.txt`. No codegen `#pragma`. Nothing in
   `tools/cc_overrides.txt` or `tools/cc_flag_overrides.txt`. A match that needs one of these is a
   diagnosis: find what the source has that the ROM's did not.
9. Route on the `RESIDUE <CLASS>` line `wgate.py` prints. Run `colorsweep.py` before calling a
   register or scheduling residue stuck. Never write a function off as unmatchable.
10. Key on the address, never the `func_` name. A curated name in `symbols.txt` is binding; define
    exactly that symbol.
11. Preserve before deleting. An untracked worker file is the only copy of that attempt.
12. Never edit a running bash script; edit a copy and `mv` it over. Quiesce the fleet before editing
    any pipeline script (`touch STOP_PULL FLEET_STOPPED`, then `fullstop.sh`).
13. Never kill a DQIX process by hand. `bash fullstop.sh` (`--dry` to look, `--hard` for CPU jobs
    too), `bash killfleet.sh` to escalate. Count jobs with `bash psq.sh`, dispatchers only by
    `pull_all.pid`.
14. Verify the effect, not the patch. After any script edit: `python selfcheck.py` and
    `python regress.py`; `regress.py --slow` after touching `colorsweep.py`, `wdiff.py` or
    `wgate.py`; `python pipetest.py` after touching the gate.
15. Never fork a code path; parameterise it. A new script gets a line in `INVENTORY.md`.
16. After editing `worker_src/core.md`, run `python build_worker_docs.py` and grep the built doc for
    the new text.
17. Record work in progress in `OPEN_WORK.md` while working, not at the end. A fresh session starts
    from it.
18. State stays in `$SP`, outside the checkout. Never write state into `$KIT` and never point
    `state.path` or `DQIX_STATE` inside it. Update the checkout through `kit_update.py` only.
19. A crack is not finished until it is promoted: append the `$SP/wlog/levers.tsv` row, then turn the
    lever into a `colorsweep.py` rule, a repair, or a `core.md` rule citing the address, with its
    `regress.py` proof, or decline it in `levers_declined.txt`. On a miss, record the blocker with
    `blocker.py` and what was ruled out in the function's handoff; only maintainers add `deadends.md`
    rows. The dispatcher stops claiming until this is done. What may go to the kit is listed in
    "Changing the kit" above.
    [docs/IMPROVEMENT_LOOP.md](docs/IMPROVEMENT_LOOP.md) has the whole loop.
20. Reserve work with ONE open issue in the kit repository listing every address you are actively
    working on, never one issue per function. Edit it as the list changes. The moment you open the
    pull request for those addresses, close the issue with a comment naming it
    (`PR: ZevyaDev/dqix-decomp#27`); do not wait for the merge. Then open a new issue for the next
    batch. An address is reserved only while it is listed in an open kit issue or changed by an open
    pull request on ZevyaDev/dqix-decomp. An address in a closed issue with no pull request, and not
    landed in `decomp-matching`, is free: its owner stopped or failed.
    [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) has the steps.
21. Never open a pull request from a stale checkout, on either repository. Immediately before
    `gh pr create`: `python kit_update.py`; in the decomp `git pull --rebase` onto the published
    `decomp-matching`, then `python tools/configure.py usa && ninja check` and re-gate what you
    changed; then `python prready.py kit` or `python prready.py decomp` must print `READY`. Fix every
    line it prints; never work around it. In Claude Code a PreToolUse hook refuses `gh pr create`
    until it passes; every other agent runs it by hand.
