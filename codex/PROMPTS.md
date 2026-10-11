# Base prompts for Codex

Two prompts:
- **Setup** runs once, in an empty folder, to build the workspace.
- **Work** runs every session, opened in the kit folder that setup creates.

Fill in the `<...>` values in the setup prompt before pasting it.

You need, before starting:
- Windows with Git for Windows
- Python 3.10+
- the GitHub CLI (`gh auth login`)
- your own dump of the USA ROM

---

## 1. Setup prompt (once, in an empty folder)

```
Set up a Dragon Quest IX decomp workspace in this folder. Do only setup: no matching, no issues, no PRs.

My GitHub user: <your-github-user>
My USA base ROM: <full path to the .nds file>

Run every command that talks to GitHub (gh, git clone/fetch/push) with escalated permissions from the first attempt; the Windows sandbox cannot see my GitHub login. If any step fails, show me the exact output and stop.

1. Check `git --version`, `python --version` (3.10+) and `gh auth status`.
2. Fork and clone the decomp:
   - If I have no fork of ZevyaDev/dqix-decomp, run `gh repo fork ZevyaDev/dqix-decomp --clone=false`.
   - Clone my fork's `decomp-matching` branch into `dqix-decomp`.
   - Add `https://github.com/ZevyaDev/dqix-decomp.git` as the remote `zevya`.
3. Copy my ROM to `dqix-decomp/extract/baserom_dqix_usa.nds`.
4. Create a Python virtual environment at `C:\dqix-venv`. The path must have no spaces. Install `ninja capstone pyelftools frida` and `dqix-decomp/tools/requirements.txt` into it.
5. In `dqix-decomp`, with that environment's python:
   - run `python tools/configure.py usa`
   - run `ninja check rom`
   - it must finish green.
6. Clone https://github.com/2gifts/dqix-decomp-kit-codex.git into `dqix-decomp-kit`. In it, with that environment's python, run:
   - `python kit_init.py --slow`
   - `python selfcheck.py`
   - `python codex\setup_workspace.py`
7. Tell me to:
   - open Codex in the `dqix-decomp-kit` folder and accept the trust prompt
   - optionally review its hooks with /hooks
   - paste the work prompt.
```

---

## 2. Work prompt (every session, in `dqix-decomp-kit`)

```
You are contributing matched functions to the Dragon Quest IX decompilation (ZevyaDev/dqix-decomp, branch decomp-matching) with the Codex-tuned agent kit in this folder. The goal is the most landed progress: byte-exact, clean, reviewer-ready functions delivered in large pull requests, plus the matching lessons fed back to the kit as small recipe PRs. Spend my usage on that and nothing else.

START
1. Run `python kit_update.py`, `python codex/team.py sync` and `python codex/team.py status`.
   - The Windows sandbox cannot see my GitHub login. Run every command that talks to GitHub with escalated permissions on the first attempt, never sandboxed first:
     - `kit_update.py`
     - `team.py sync`/`candidates`/`reserve`/`issue`/`integrate`/`pr`/`release`/`reclaim`
     - `gh`
     - `git fetch`/`push`
   - Also run escalated: `team.py integrate` (it runs bash, which cannot start sandboxed) and `frida/colorforce.py` and `frida/schedforce.py` (sandboxed they hang forever; give them a 10-minute timeout).
2. If `python -c "import capstone"` fails or a start command errors, show me the exact output and stop.
3. Read AGENTS.md. Load the dqix-team skill (.agents/skills/dqix-team/SKILL.md) and follow it as the procedure. Read codex/WORKER.md: it is what every worker is held to.

STUDY THE STANDARD FIRST
These are the established format. `codex/team.py` generates exactly this format, so never hand-write issues or PRs.
- Batch PRs on ZevyaDev/dqix-decomp: #77 and #78 (100+ functions each), #85 and #86, and #92. Each has a one-line description per function, a validation paragraph, the reservation link, and both contributor-checklist boxes ticked.
- Reservation issues on ZevyaDev/dqix-decomp-kit: #169 and #205.
- Recipe PRs on ZevyaDev/dqix-decomp-kit: #195, #196, #197 and #198.
Tag everything you create [Codex]. Never touch another contributor's issue, PR or reserved addresses. `team.py candidates` excludes every address in an open issue or open PR; do not override it.

THE LOOP
1. Reserve:
   - Run `python codex/team.py candidates`, `reserve --take 24`, then `issue --publish`.
   - Keep exactly ONE open issue, published before any matching starts. Re-publish it as you reserve more.
2. Dispatch:
   - Run 5 dqix-matcher subagents in parallel. Each takes one group from `python codex/team.py next --label <name>`; paste the printed prompt exactly.
   - When a worker finishes, run `collect` on its addresses and give that slot the next group. Never leave a slot idle while reserved work remains, and reserve more before the queue runs dry.
3. Verify:
   - Trust nothing a worker reports. `python codex/team.py collect` re-gates every file and enforces the quality bar.
   - Skim each match before it goes into a PR. Send it back if it has raw pointer arithmetic where a struct member would do, meaningless names, a gratuitous cast, or any comment other than the `// USA:` tag.
   - Fix vague descriptions with `team.py describe`.
4. Residues: a RESIDUE gets at most one more worker, given its residue line and notes. Then leave it. Never sink the budget into one function.
5. Publish every 40-120 matches, and always before stopping:
   - Run `python codex/team.py integrate --archive` as a background task with a long timeout. `RED: sha1 mismatch` at the end of each module is expected without an ARM7 BIOS; the commit is already made.
   - If the integration tree is left with an uncommitted config change (for example from countfix), discard it rather than ship an unexplained edit.
   - Run `python codex/team.py pr` as a dry run.
   - FOR MY FIRST PR ONLY: stop, show me the title, the body file and the integration summary, and wait for my OK. After that, open each PR yourself with `python codex/team.py pr --open`. It pushes, opens the PR, closes the issue with the PR link and opens the successor issue.
   - Once a PR is open, leave its branch alone: no rebasing, no merging decomp-matching into it, never closing it to fold it into a bigger batch. The maintainers resolve delinks conflicts at landing. Send the next batch as a new PR.
6. Never run two integrations at once.

FEED THE IMPROVEMENT LOOP (recipe PRs only)
Workers report the source change that closed each function, and `collect` records it in $SP/wlog/levers.tsv. After one of your batches lands on decomp-matching, run `python levercheck.py`. For each landed lever it lists:
1. Check worker_src/core.md. If it already teaches the lever, decline it with one line in $SP/wlog/levers_declined.txt.
2. Isolate the cause. Gate the landed source with only that construct removed (`WGATE_ALLOW_COMMITTED=1 python wgate.py ...`). It must stop matching; if it still matches, the lever is not the cause, so decline it.
3. Send it as one small PR to ZevyaDev/dqix-decomp-kit main, built from a clean checkout of upstream main (not this fork):
   - one rule of a few lines in worker_src/core.md, under the section for its symptom, citing the address
   - titled like "Record the <lever> (<addr>)"
   - the body states the isolation result
   - run `python build_worker_docs.py`, then `python prready.py kit` must say READY
   - follow docs/CONTRIBUTING.md and docs/IMPROVEMENT_LOOP.md
Recipes are the only kit contributions. Never spend effort on kit re-engineering: no workflow, script, skill, doc or dead-end PRs, and no edits to this kit. If a kit tool fails, tell me the command and the output line, and work around it with other functions.

HARD RULES
- No inline asm, no codegen #pragma, no compiler overrides, no `volatile` except hardware-register defines.
- Write files only with apply_patch, never through PowerShell redirection or Set-Content.
- Never one issue or one PR per function.
- Keep ../dqix-kit-state/OPEN_WORK.md current, so a session that hits a usage limit can resume. After a limit, restart the loop with `team.py status`, then `collect`. Workers' files survive.

WHEN I SAY STOP, OR THE BUDGET IS NEARLY GONE
1. Publish finished matches (step 5), then run `python codex/team.py release`, so the issue closes and unfinished functions are free for others.
2. If integration fails, do NOT release the matched functions. Keep the issue open, and report the failing command and log to me.
3. Report:
   - functions matched, PR'd and merged
   - open PRs
   - recipe PRs sent
   - what was released
   - anything that needs my attention

Start now.
```
