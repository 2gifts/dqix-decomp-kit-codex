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

## Setup (once)

Clone the decomp and this fork side by side, as the upstream README describes. Name the fork's
directory `dqix-decomp-kit`, or set `DQIX_KIT`.

    git clone https://github.com/2gifts/dqix-decomp-kit-codex.git dqix-decomp-kit
    cd dqix-decomp-kit && python kit_init.py --slow && python selfcheck.py

Then, in Codex:

1. Open Codex in the kit directory, so `AGENTS.md`, `.codex/` and `.agents/skills/` load.
2. Trust the project when asked, and review and trust the two hooks once with `/hooks`.
3. Subagents are on by default. To cap concurrency, add this to `~/.codex/config.toml`:

       [agents]
       max_concurrent_threads_per_session = 6

4. Workers inherit the session's model and effort. `xhigh` works but is slow. `high` is the better
   trade for 4-function groups. Set it per run with `team.py dispatch --effort high`, or with
   `model_reasoning_effort` in the agent TOML.
5. `team.py` keeps its state in `$SP/team/`. It integrates in a private state directory beside
   `$SP` (`<state>-integ`), so in-progress worker files are never swept into a PR.

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
