# Track A — kickoff prompt

Paste the fenced block below into a **fresh** Claude Code session opened in
this repository to begin Track A (the matching decompilation). It is
self-contained: it directs the new session to the docs it needs.

```
We are starting Track A of the Extermination (PS2) decompilation project: the
matching decompilation of the game's code. The asset-format reverse-engineering
(Track B) is broadly complete.

Before doing anything, read these for full context:
- CLAUDE.md — the project charter, the non-negotiable legal rules, my
  development environment, the toolchain conventions, and the pinned Target
  identity: SCUS-97112, Metrowerks CodeWarrior (mwccps2 2.3.1.01), no DWARF and
  no symbol table — this is blind matching, the hardest tier.
- docs/PROGRESS.md — current project status and the roadmap.
- docs/FINDINGS.md — the reverse-engineered asset formats (reference).

Goal of Track A: stand up the matching-decompilation pipeline and work toward a
runnable developer build — I want to be able to compile the decompiled code and
run the game with its own assets, the way a developer would. Asset repacking is
deferred; the original DATA.DAT is used as-is.

First milestone — the "hello world" of the decomp pipeline:
1. Clone a reference decomp template that uses the same toolchain —
   fmil95/recvx-decomp (RE: Code Veronica X, also Metrowerks CodeWarrior).
   Study its structure and build scripts; check it for a Dockerfile. Adapt it
   rather than building scaffolding from scratch.
2. Install objdiff (encounter/objdiff) — native macOS arm64 build, runs locally
   on the M1.
3. Set up splat to disassemble and split the boot ELF into objects objdiff can
   use as match targets.
4. Stand up the x86_64-Linux build container for the period-correct compiler —
   Metrowerks mwccps2 run via wibo (a linux/amd64 Docker container). I will
   supply the mwccps2 compiler — it is proprietary and must not be committed.
5. Get one trivial leaf function to a 100% match in objdiff — the proof the
   whole pipeline works end to end.

Work step by step: propose the plan and checkpoint with me before running long
multi-step sequences. Label every command by environment (macOS-arm64 vs
Linux/Docker). Follow the legal rules in CLAUDE.md — the boot ELF and all
disc-derived material stay local and git-ignored; only original code and build
scripts are committed. Keep docs/PROGRESS.md and docs/FINDINGS.md updated as
Track A progresses.

Start by reading CLAUDE.md and docs/PROGRESS.md, then propose the Track A setup
plan.
```
