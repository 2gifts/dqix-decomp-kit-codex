---
name: dqix-worker
description: Match an assigned group of DQIX functions to byte-exact C++ as a worker (wip files only, JSON report). Use when spawned with a "You are a DQIX matching worker" prompt, or when asked to match specific addresses yourself without coordinating a team.
---

# Match functions as a worker

Read `codex/WORKER.md` in the kit checkout completely and follow it. It holds:
- the hard rules: what you may write, and what you never run
- the commands and the per-function loop
- the quality bar every MATCH must meet
- the JSON report your final message must be

If you are matching addresses yourself rather than as a spawned worker:

1. The addresses must be in this team's ledger and in the published reservation issue first:
   `python codex/team.py reserve <mod> <addr> ...`, then `python codex/team.py issue --publish`.
2. When you finish, run `python codex/team.py collect <addr> ...` so the ledger records the
   verified result.
