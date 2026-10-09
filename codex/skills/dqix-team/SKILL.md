---
name: dqix-team
description: Run the DQIX decomp as a coordinator with a team of matching workers, from reservation through the published pull request and release. Use when asked to match functions, make progress on the decomp, use up a usage budget, open a decomp PR, continue after a limit, or wrap up and release reservations.
---

# Coordinate a DQIX matching team

You are the coordinator. Workers match functions; you keep the queue full and own everything shared:
- the ledger
- the one reservation issue
- integration
- the pull request
- releasing work

`codex/team.py` does each of these steps deterministically. Use it instead of hand-built `gh` or
`git` sequences.

What this process produced for the Claude team, which this fork copies:
- About 600 matched functions in two days, against about 15 parked.
- Functions were 200-700 bytes, next to already-matched siblings, in groups of 4 per worker, with
  8-12 workers running.
- They landed in PRs of 40-120 functions, all merged.

A one-function PR costs the maintainer about as much review as a 100-function one.

Run everything from the kit checkout. In Git Bash: `export DQIX_KIT="$(pwd -W)"`. In PowerShell:
`$env:DQIX_KIT = (Get-Location).Path`.

## 0. Start of every session

    python kit_update.py                    re-read every RE-READ file it prints
    python codex/team.py sync               fast-forward the decomp checkout to decomp-matching
    python codex/team.py status
    gh pr list -R ZevyaDev/dqix-decomp --author @me --state open

If one of your open PRs has review comments or requested changes, deal with them before anything
else. A PR waiting on you blocks every later PR that touches the same modules.

## 1. Reserve: one issue, published before any work starts

    python codex/team.py candidates --limit 40           ranked free functions (size 200-700 by default)
    python codex/team.py reserve --take 24 [--module 017]
    python codex/team.py issue --publish                 creates or edits the ONE [Codex] issue

`candidates` already excludes:
- anything delinked on `decomp-matching`
- anything listed in another open kit issue or changed by an open decomp PR (live `gh` scan)
- skiplisted and OPEN_RESIDUES addresses
- your own ledger

It ranks modules with many matched siblings first, and functions next to matched code, so workers
have neighbours to copy from.

Reserve about two groups of 4 per worker you will run. Never work on an address before the issue
listing it is published. Add rows and re-publish the same issue as the list changes; never open a
second issue.

## 2. Dispatch: groups of 4, one worker per group

Pick one mode.

**Subagents (interactive session).** For each free slot:

    python codex/team.py next --label <worker-name>      prints the worker prompt for 4 addresses

Spawn the `dqix-matcher` agent (`.codex/agents/dqix-matcher.toml`) with that prompt verbatim. Paste
the real printed prompt, never a placeholder. Keep 4-6 workers running. When one returns, refill
its slot straight away.

**Headless fleet (long unattended runs).**

    python codex/team.py dispatch --workers 4 [--effort high]

Each group runs as a `codex exec` process. Its schema-checked JSON report lands in
`$SP/team/reports/`, and the group is collected automatically as soon as it exits. Run it as a
background task with a long timeout.

A worker that dies on a usage limit leaves its wip files. Its rows go back to ACTIVE on collect,
and the next worker resumes from the file.

## 3. Collect: never trust a report, re-gate it

    python codex/team.py collect [ADDR ...]

For each address, `collect` re-runs `wgate.py` on the wip file and applies the quality bar. The
row becomes MATCH only when both pass:
- only the `// USA:` comment
- no asm or codegen pragma
- `volatile` only in MMIO register defines
- `#include <globaldefs.h>` first

`collect` copies each worker's one-line description into the ledger. Fix a vague one with
`team.py describe ADDR "Battle menu confirm handler"`.

A RESIDUE row stays reserved. Give it at most one more worker, with the residue line and the
worker's notes in the prompt. After that, leave it for release; do not sink the budget into it.

Read every MATCH before it goes in a PR, at least skimming it. The reviewer will reject:
- unnamed pointer arithmetic where a struct member would do
- a struct that duplicates an existing header type without need
- leftover debugging names

Send any of these back to a worker.

## 4. Integrate and publish (every 40-120 matches, and always before stopping)

Before you start:
- Check `gh pr list -R ZevyaDev/dqix-decomp --author @me`. If an open PR of yours touches the same
  modules, wait for it to merge, or integrate only other modules: two PRs appending to the same
  `delinks.txt` conflict.
- Make sure no other integration is running. Two at once destroy each other's staging and branch.

Then, as a background task with a long timeout:

    python codex/team.py integrate --archive
    python codex/team.py pr                  dry run: title, body file, prready verdict
    python codex/team.py pr --open           push to the fork, gh pr create, close the issue with
                                              "PR: ZevyaDev/dqix-decomp#N", open the successor issue

`integrate` builds `codex/overlay-batch-N` from the fresh `decomp-matching` tip in the integration
worktree:
1. re-gates and stages every MATCH
2. runs `finish_wave.sh` per module (wiring, `ninja check`), then `ninja report` and `countfix.py`

Without an ARM7 BIOS dump, `finish_wave` ends with `RED: sha1 mismatch`. That is expected locally:
the commit is already made, and the ROM differs only in header bytes `0x6c-0x6d` and
`0x15e-0x15f`.

A function `finish_wave` defers stays MATCH in the ledger and goes into the next batch. Typical
causes are a reloc that needs a `_dup` symbol alias, or a symbol owned by another unit.

`pr` refuses unless `prready.py decomp` prints READY. The PR body carries:
- the table of every function with its description
- the validation paragraph
- the reservation link
- the two ticked contributor-checklist lines the CI check requires

Do not edit those lines out.

When `decomp-matching` moves while a PR is open and it conflicts: rebase the branch. In
`delinks.txt`, keep both sides and keep the two blank lines between entries, or `dsd` fails. Then
`ninja check`, then force-push.

## 5. Stop or wrap up

    python codex/team.py release            closes the issue with a release comment; unfinished rows -> RELEASED

1. Publish finished matches first (step 4), so `release` only frees what is unfinished.
2. Before `release`, wait for, or stop, running workers and any `dispatch`.
3. Record anything a later session needs in `$SP/OPEN_WORK.md`: open PRs, parked residues and why.

## Never

- Open a kit pull request, or edit kit docs or scripts, as part of a matching session. This fork is
  maintained by its owner. Report a broken tool to the user with the command and output.
- Open one issue or PR per function, or work on an unpublished reservation.
- Run two integrations at once, or start one while another session's is running.
- Comment on, push to or close anything that is not this team's own.
