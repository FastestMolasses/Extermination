# CLAUDE.md — Extermination (PS2) Matching Decompilation

Persistent project instructions. Load these every session.

## What this project is

A matching decompilation of the PlayStation 2 game **Extermination** (Sony-published,
2001), with the long-term goal of a clean, moddable, native port for **macOS, Windows,
and Linux**. This is a PS2 / Emotion Engine (EE, MIPS R5900) matching-decomp effort in
the same family as these reference projects — study their structure and conventions
rather than inventing our own:

- `TheOnlyZac/sly1` — Sly Cooper, GCC-based, objdiff-driven, BYO-disc. Exemplary structure.
- `RossyDoubleUnderscore/ICO-decomp` — ICO, targets 1:1 matching.
- `entriphy/kl2_lv_decomp` — Klonoa 2.
- `fmil95/recvx-decomp` — RE: Code Veronica X (CodeWarrior/MWCC + wibo example; DWARF-symbol case).

The realistic scope is a multi-year, possibly multi-person effort. Do not pretend
otherwise, and help me prioritize accordingly (see "Scope & priorities").

## Project state & documentation

The repository's `docs/` folder is the living record of project state and
findings. **Read it at the start of every session, and keep it up to date.**

- `docs/PROGRESS.md` — current status, completed work, open questions, and the
  roadmap. This is the entry point for continuing the project.
- `docs/FINDINGS.md` — technical reference for reverse-engineered file formats
  and engine behavior (large; grep it).
- `docs/HANDOFF.md` — the short, current cross-repo entry point (goal, toolkit,
  state, next steps). Update it at the end of every session.
- The native port (`../extermination-port`) keeps its roadmap in its own docs:
  `docs/FIRST_LEVEL_AUDIT.md`, `docs/SCENE_COORDINATOR_DESIGN.md`,
  `docs/ORIGINAL_FRAME_ORDER.md` (see the port's CLAUDE.md).

Whenever you reach a milestone, learn or revise a finding, or change the
roadmap, update these files **in the same session** so the next person or agent
can pick up cleanly. Treat keeping `docs/` current as part of finishing any task.

## Hard rules (non-negotiable — these protect the project legally)

I own a legal copy of the game. These rules are absolute; do not relax them even if I
seem to ask you to in a moment of convenience.

- **Never commit, upload, paste, print in full, or otherwise redistribute** the ISO, the
  boot ELF, disassembly of the original binary, extracted assets, or any original game
  code/data. Treat all disc-derived material as: *user-supplied, generated locally on the
  end user's machine from their own copy, never shipped.*
- The repository contains **only my own original code, build scripts, and tooling.**
  Anything derived from the disc is produced locally by each user from their own legal copy.
- **Before any `git add` / commit, verify nothing disc-derived is included.** If a planned
  commit might contain disc-derived material, stop and warn me. When in doubt, ask.
- Maintain a strict `.gitignore` covering at least: `iso/`, `*.iso`, the boot ELF
  (`SLUS_*`, `SLES_*`, `SCUS_*`, `*.elf`), extracted assets, `*.bin` dumped from the disc,
  and any local scratch dumps. Add to it proactively as new disc-derived artifacts appear.
- **Do not help circumvent disc copy protection or facilitate piracy.** The supported
  workflow is always "the user supplies their own legally-dumped disc."
- **Keep this project completely isolated from my other code.** Never reference, import
  from, link against, or copy from any of my other repositories. (I am separately
  developing a commercial game; it must remain entirely unentangled with this project.)

## My environment

- **Primary machine: Apple Silicon MacBook Pro (M1, arm64, macOS).** I do all interactive
  work here: reading code, Ghidra, objdiff, testing in PCSX2.
- I also have an **x86_64 Linux environment** and a **Windows machine** available.
- Assume the **period-correct PS2 build toolchain** (ee-gcc, or possibly Metrowerks
  CodeWarrior / MWCC via wibo) is best run in **x86_64 Linux, not natively on arm64.**
  Prefer a Docker `linux/amd64` container or a Linux VM for the compiler/build/match step.
  Only fall back to Windows if something genuinely requires it — and say so explicitly.
- **For every tool you recommend, state where it runs:** native arm64 macOS, needs Rosetta,
  or needs the x86_64 Linux environment. When a step has both a macOS-native and a
  Linux/Docker path, show both and recommend one.

## Toolchain & tooling conventions

- **objdiff** (`encounter/objdiff`) is the heart of the matching loop. It has native macOS
  arm64 binaries — run it locally on the M1.
- **splat** is the disassembler/splitter that produces target objects for objdiff (Python,
  runs fine on arm64). Use its full-disassembly mode for objdiff target objects.
- **Compilers (established):** game code is Metrowerks CodeWarrior `mwccps2` (point
  releases 2.3.3/2.4/3.0 selected per file via `// COMPILER:`), SDK/low-memory code is
  ee-gcc 2.9-991111 (`// COMPILER: eegcc`). Both run via wibo/qemu inside the Apple
  `container` image `exterm-permuter` (x86_64 Linux); `tools/decomp/build.py` drives it.
- **PCSX2** (Apple Silicon, MCP/DebugServer-enabled build) runs the original for
  captures and lockstep comparisons (see "Verification toolkit").
- **Ghidra** runs on arm64 macOS with an arm64 JDK.
- Prefer cloning a reference template repo and adapting it over building scaffolding from
  scratch. Check any cloned repo for a Dockerfile first — it may solve toolchain setup.

## Target identity (PINNED — characterized 2026-05-22)

Decomp projects live or die on targeting exactly one build. Recorded below;
never silently change:

- Regional serial: `SCUS-97112` (USA, Sony first-party; boot ELF `SCUS_971.12`, VER 1.00, NTSC)
- Boot ELF SHA1: `2cca045edce7db2af2c865bd80b46f79661608f0`
- Boot ELF SHA256: `ee052236783e7d3e865754d3ff9fee71290addeb7d146c86caa7ff2724d1e17a`
- Boot ELF size: `1532624` bytes — ELF32 LSB MIPS, statically linked
- Original compiler: `Metrowerks CodeWarrior` — `MW MIPS C Compiler (2.3.1.01)`, PlayStation2
  (from `.comment`). NOT ee-gcc → matching compiler is `mwccps2`, run via wibo on x86_64 Linux.
- DWARF debug symbols present? `No` — `.shstrtab` is exactly 0x2d bytes, accounting only for
  `.shstrtab/.strtab/.symtab/.comment/.reginfo`; no `.debug_*`/`.mdebug`. `.symtab` has zero
  entries (effectively stripped). → Hardest tier: fully blind matching (like Sly1/ICO).
- Layout: one boot LOAD segment at vaddr `0x00100000` (filesz `0x175b00`, memsz `0x00723500`);
  a runtime overlay arena at vaddr `0x00823500` fed by modules in disc `OVERLAY/`.
- Entry point `0x00100008`; `$gp` = `0x0027D370` (crt0 does `move $gp, $a0` after building
  `$a0` from `lui 0x28 / addiu -0x2C90`). The loadable content is a single unnamed PROGBITS
  section, file `0x300..0x175E00` → vram `0x00100000`; splat auto-detects ~3014 functions.

## Scope & priorities

- A full matching decomp is the long game. Be honest about effort and never imply a 3D PS2
  game can be "ported in a weekend."
- Flag the **cheaper parallel win**: reverse-engineering the **asset file formats** and
  building extraction/viewer tools can deliver real modding value *long before* the code is
  matched. Help me weigh "advance the matching decomp" vs. "ship modding tools" at decision
  points.
- **End-state build architecture:** the goal is that a user supplies their own legal disc,
  the build extracts the assets to loose files, compiles the decompiled code, and *repacks*
  the assets into the original containers — producing the same game. So every asset
  extractor needs a matching **repacker** (loose files → byte-identical container). This is
  what makes the game genuinely moddable and extensible — assets live as editable separate
  files, and a rebuild reproduces the game.
- The stated north-star is eventually earning the standing to pitch Sony a remake. That
  framing is strongest if this project stays scrupulously clean — reinforce that, don't
  undermine it.

## How to work with me

- I have a strong software-engineering background and am comfortable with C/C++, assembly,
  and reverse-engineering tools. Explain the *why* briefly — I want to understand the
  pipeline, not just run commands — but don't over-explain basics.
- Proceed **step by step.** Propose next steps; don't blindly execute multi-step plans
  without checkpoints. (The bootstrap path — template, objdiff, splat, container, first
  100% leaf — is long done; the current goal is below.)
- Give exact, copy-pasteable commands, labeled by environment (macOS-arm64 vs. Linux/Docker).
- When something needs a cross MIPS toolchain (e.g. an `ee`/`mips` objdump or binutils),
  tell me how to get an arm64-friendly build.

## Current goal (user, 2026-09-22)

Make the native port's FIRST LEVEL (New Game → AREA11 opening → all of AREA11 →
its exit) exactly original, while the decomp's C stays byte-identical. Earlier
port code contained fabricated behavior: nothing counts as original until an
original-instruction oracle or an original capture confirms it ("a label is not
evidence"). Characterization is done (Target identity above).

The finished port ships two profiles (user, 2026-09-23; rules in the port's
`docs/PORT_PROFILES.md`). **Original** is the default and the only thing fidelity
work measures: the exact GS framebuffer shown at 4:3, with no smoothing and no CRT
simulation. **Enhanced** adds the user's improvements as switches over the same
logic. Its cut-content items come from this repo's `docs/CURIOSITIES.md`.

## Verification toolkit

- `tools/verify_all.py` — six-stage gate (boot ELF byte-identical, 19/19
  overlays, matched_code floor, glTF, anim self-test, GS offset). Its `match`
  stage measures whatever is already in `build/obj` + `build/expected`: run
  `tools/decomp/build.py build` (~7 min) first before quoting counts or judging
  a build-config change (fresh baseline 2026-09-23: 2149/2210 units, 98.60%; jump-table
  dispatchers link from C since local .rodata pinning, tools/decomp/rodata_pin.py).
- Promotions go through `tools/match/integrate_nearmiss.py` (guards) and
  `tools/decomp/audit_link_provenance.py` (the linked object must really be the
  compiled C, not an assembly fallback).
- `tools/pcsx2_session.py` — drives the ORIGINAL game in the MCP-enabled PCSX2
  (`build/startup-reference/PCSX2.app`, hidden by default): exact one-frame
  steps (breakpoint at 0x001AAF28), pad input (reaches 0x810E70 two frames
  later), memory reads, snapshots with the save state's screenshot. Save states
  live in `build/startup-reference/portable-data/sstates/` (never overwrite
  slots 01–15). The PCSX2 MCP server is also registered (local scope) and loads
  its tools in new sessions.
- Never move, mount, restore or "recover" the emulator install or its backups
  (user instruction).

## Tests

- Tests exist to prove the first level (and the path into it) matches the
  original. Port tests follow the port's CLAUDE.md "Tests" section (first-level
  scope, ~10 s default runs with exhaustive sweeps behind `EM_TEST_FULL=1`,
  headless — no windows — and the retirement rules). Decomp-side proof is the
  six-stage gate plus per-function objdiff; don't add long bespoke suites where
  a byte-matched function already proves the behavior.
- **Retire a test** when the user asks; when it covers content outside the first
  level or a mechanism no longer on the live path; when it encodes non-original
  behavior; when a stricter test supersedes it; or when its module is certified
  (byte-matched original function + a passed full oracle sweep + exercised by the
  level smoke) — then shrink it to a smoke sample and keep the exhaustive
  variant only behind `EM_TEST_FULL=1`. Never retire a test to make a change
  pass; record retirements in the commit message and docs.
- Automated runs never put windows in front of the user: the native port is
  headless under test variables; PCSX2 runs hidden through `pcsx2_session.py`;
  never leave an emulator running.

## Temporary artifacts and cleanup

- Screenshots, captures, traces, emulator snapshots, scratch objects and lane
  builds go under ignored `build/<task>/` (or the session scratchpad) — never the
  repo root, `docs/` or `src/`. One folder per task so it can be removed as a
  unit.
- Never delete: `build/startup-reference/**` (original captures and save states =
  oracle inputs), the user's ISO/ELF/config, `extract/`, and receipts that a
  committed doc cites.
- Clean up at checkpoints: (1) before a work package's final commit — delete its
  intermediate screenshots, snapshots (state.p2s / eeMemory / gs dumps), trial
  objects and permuter scratch not referenced by docs; (2) at session end —
  remove scratch trees, baseline checkouts, stray files in the repo root, and
  any running emulator, then update `docs/HANDOFF.md`; (3) whenever `build/`
  exceeds ~5 GB or artifacts are older than 7 days and unreferenced.
- Keep only the latest before/after screenshot pair per comparison topic.
- Clean only generated artifacts; never user files.
