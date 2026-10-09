# The Codex fork

`2gifts/dqix-decomp-kit-codex` is `ZevyaDev/dqix-decomp-kit` plus a coordinator/worker layer tuned
for Codex. Everything upstream (the gate, colorsweep, integration, prready) is unchanged and still
does the real work. This fork changes how Codex drives it.

## What is different from upstream

| file | change |
|---|---|
| `AGENTS.md` | a Codex "start here" on top: the batch loop, the quality bar, Windows file-writing rules; the skills table; kit changes are off-limits to sessions |
| `codex/team.py` | the coordinator: ledger, candidates, the one reservation issue, groups of 4, `codex exec` fleet, re-gate and quality bar on collect, batch integration, PR body and publish, issue close and successor, release |
| `codex/WORKER.md` | the worker brief, given to every worker verbatim |
| `codex/report.schema.json` | the JSON report every worker returns (`codex exec --output-schema`) |
| `codex/skills/dqix-team`, `codex/skills/dqix-worker` | Codex skills; `kit_init.py` copies them into `.agents/skills/` |
| `.codex/agents/dqix-matcher.toml` | the worker as a Codex custom subagent |
| `.codex/hooks.json` | `kit_update.py` on session start; `prready.py --hook` refuses `gh pr create` until READY |
| `kitpaths.py`, `kit_update.py` | the kit updates from this fork (`DQIX_KIT_URL` overrides) instead of upstream |
| `kit_init.py` | also copies `codex/skills/*` into `.agents/skills/` |
| `codex/setup_workspace.py` | writes the machine-local `.codex/config.toml` (environment, sandbox, subagent cap) |

## Setup (once per machine)

Put the workspace in one folder. Use the names below, or set `DQIX_KIT`, `DQIX_REPO` and
`DQIX_STATE`.

    <workspace>\dqix-decomp        your clone of decomp-matching (origin = your fork, zevya = ZevyaDev), built
    <workspace>\dqix-decomp-kit    this fork
    <workspace>\dqix-kit-state     created by kit_init.py

1. Create a Python 3.10+ environment at a path without spaces, and install the kit's dependencies
   and the decomp's `tools/requirements.txt`: `capstone pyelftools frida ninja`.
2. Build the decomp: `python tools/configure.py usa`, then `ninja check rom`.
3. From PowerShell, in the kit:

       <venv>\Scripts\python.exe kit_init.py --slow
       <venv>\Scripts\python.exe selfcheck.py
       <venv>\Scripts\python.exe codex\setup_workspace.py

   `setup_workspace.py` writes `.codex/config.toml` (untracked). It puts the environment first on
   PATH for every Codex command and subagent, trusts the workspace for git, makes the workspace
   writable and allows network for git and gh, and caps subagents at 6.
4. Mark the kit folder trusted in Codex: accept the prompt, or add
   `[projects.'<kit path, lower-case>'] trust_level = "trusted"` to `~/.codex/config.toml`.
   Project config and hooks load only for trusted projects.
5. Optionally, review the two hooks with `/hooks`. They are a convenience: `AGENTS.md` already
   runs `kit_update.py`, and `team.py pr` already runs `prready.py`.

Open Codex in the kit folder, so `AGENTS.md`, `.codex/` and `.agents/skills/` all load. Do not open
it in the workspace root.

## Running it

Interactive, as the coordinator with subagents. Tell Codex:

> Use the dqix-team skill. Reserve about 24 functions, keep 5 dqix-matcher workers busy, and open a
> PR once about 60 are matched. Stop and release when I say so.

Headless, as a fleet. Run this as a long background job and look at it occasionally:

    python codex/team.py reserve --take 32 && python codex/team.py issue --publish
    python codex/team.py dispatch --workers 6 --effort high
    python codex/team.py status

Publishing and stopping:

    python codex/team.py integrate --archive && python codex/team.py pr --open
    python codex/team.py release

## Keeping the fork current with upstream

    git fetch upstream
    git merge upstream/main

Conflicts can only come from the files in the table above. Keep the fork's lines:
- the `KIT_URL` default in `kitpaths.py`
- the codex-skills loop in `kit_init.py`
- the Codex sections of `AGENTS.md`

Take upstream's version of everything else. Then:

    python selfcheck.py && python regress.py
    git push origin main

Sessions update from this fork through `kit_update.py`, so they pick up the merge on their next
start.

If upstream changes `wgate.py`'s output lines, `finish_wave.sh`'s knobs (`DQIX_STATE`,
`DQIX_BRANCH`, `DQIX_PUBLISH=local`) or `prready.py`, rerun the `team.py` smoke test before
pushing: reserve, `next`, and `collect` on a known MATCH in a scratch `DQIX_STATE`.
