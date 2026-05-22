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
- The **period-correct compiler** (ee-gcc vs. CodeWarrior) determines whether we get GCC's
  `INCLUDE_ASM` workflow and how much x86_64-Linux scaffolding we need. This is established
  during ELF characterization (below) — do not assume it before then.
- **PCSX2** (Apple Silicon build) is used to test rebuilt executables locally.
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

## Scope & priorities

- A full matching decomp is the long game. Be honest about effort and never imply a 3D PS2
  game can be "ported in a weekend."
- Flag the **cheaper parallel win**: reverse-engineering the **asset file formats** and
  building extraction/viewer tools can deliver real modding value *long before* the code is
  matched. Help me weigh "advance the matching decomp" vs. "ship modding tools" at decision
  points.
- The stated north-star is eventually earning the standing to pitch Sony a remake. That
  framing is strongest if this project stays scrupulously clean — reinforce that, don't
  undermine it.

## How to work with me

- I have a strong software-engineering background and am comfortable with C/C++, assembly,
  and reverse-engineering tools. Explain the *why* briefly — I want to understand the
  pipeline, not just run commands — but don't over-explain basics.
- Proceed **step by step.** Propose next steps; don't blindly execute multi-step plans
  without checkpoints. After characterization, the natural path is: clone a reference
  template → install objdiff (native arm64) → set up splat → stand up the x86_64-Linux
  build container → get a single trivial leaf function to match 100% in objdiff as the
  "hello world" of the pipeline.
- Give exact, copy-pasteable commands, labeled by environment (macOS-arm64 vs. Linux/Docker).
- When something needs a cross MIPS toolchain (e.g. an `ee`/`mips` objdump or binutils),
  tell me how to get an arm64-friendly build.

## First milestone (if the repo is not yet characterized)

Characterize the boot executable before committing to a toolchain:

1. Mount the ISO, locate the boot ELF named in `SYSTEM.CNF`, record the regional serial and
   the ELF's SHA1. Pin them above.
2. Check for DWARF debug symbols (`readelf -S`, `readelf --debug-dump=info`, objdump).
   Interpret the result for project difficulty.
3. Inspect `.comment` and instruction-scheduling/library signatures to identify the original
   compiler. Explain what it implies for our toolchain and `INCLUDE_ASM` availability.

Have me paste tool output back to you, then interpret it and update the "Target identity"
section.
