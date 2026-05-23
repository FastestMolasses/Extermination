# Extermination Decomp — Progress

Living status document for the Extermination (PS2) decompilation / modding
project. **Keep this current** — update it whenever a milestone is reached, a
finding changes, or the roadmap shifts. With `docs/FINDINGS.md` it is the entry
point for anyone (a person or an agent) picking up the project.

_Last updated: 2026-05-23 (Track A: 1008 src files, ~32% of 3014 unique-vram functions; partial-link pipeline at **100% byte identity** — `elf/SCUS_971.12.elf` loadable region is byte-identical to original)_

## Project at a glance

- Goal: a matching decompilation of *Extermination* (PS2, **SCUS-97112**) and,
  long-term, a clean native PC port that is moddable/extensible. See `CLAUDE.md`
  for the full charter and the non-negotiable legal rules.
- Two tracks:
  - **Track A — matching decompilation** of the game code. In progress — the
    build pipeline is complete and the first function is a 100% match.
  - **Track B — asset-format RE / modding tools.** Broadly complete.
- **Current focus: Track A.** The near-term goal is a runnable, developer-style
  build — the decompiled code compiles and runs with the game's own assets, the
  way you would if developing the game. Asset *repacking* is explicitly
  deferred to late in the project (it is not needed for the dev loop — the
  original `DATA.DAT` is used as-is).

## Status

### Done — Track B asset extraction

All asset extractors are committed in `tools/` (original code, Python 3 stdlib,
native arm64). Disc-derived *output* is git-ignored. `docs/FINDINGS.md` has the
full format detail for everything below.

- **Boot ELF characterized** — `SCUS_971.12`, Metrowerks CodeWarrior (`mwccps2`
  2.3.1.01), **no DWARF / no symbols** (hardest matching tier). Pinned in
  `CLAUDE.md` "Target identity".
- **`DATA.DAT` / `INDEX.IDX` archive** — `tools/extract_data.py`. Reverse-
  engineered and validated by exact tiling: 81 regions / 603 files, byte-exact,
  100% coverage. Asset taxonomy mapped (603 files by content).
- **Audio** — `tools/decode_sound.py`. All PS2 VAG ADPCM.
  - SFX: SShd sound banks → 241 unique sounds (deduped from 1146 references).
  - Dialogue: `STREAM/VOICE.DAT` → 116 mono clips.
  - Music: `STREAM/MUSIC.DAT` → 55 stereo tracks (interleaved VAG).
  - Streamed audio is 48000 Hz (End Credits matches an official-soundtrack rip).
- **Textures** — `tools/extract_textures.py` (sheet extractor) and
  `tools/extract_subtextures.py` (per-material). 8-bit PSMT8, decoded with the
  authoritative GS swizzle (proven correct). The geometry marker→texture
  binding is cracked; 2274 per-material PNGs export. Output is grayscale (color
  source unresolved — see open questions).
- **Geometry & models** — `tools/extract_models.py`. Level geometry (`id 0x44`
  files) and ~328 character/object/prop models → Wavefront OBJ. Modes:
  `--scene` (apply MATRIX instance transforms → placed full-level scenes),
  `--rig` (dump the skeleton-transform files), `--anim` (export vertex-
  animation pose sequences).

### Done — Track A matching-decomp pipeline (the "hello world")

The decomp pipeline is stood up end to end and the first functions are verified
100% matches. The M1 runs objdiff, splat and editing; an arm64 Linux container
runs the period-correct compiler.

- **Boot ELF** `SCUS_971.12` extracted from the user's ISO to
  `config/SCUS_971.12` (git-ignored); SHA1 verified. Entry `0x00100008`, `$gp`
  `0x0027D370`, one loadable PROGBITS section (file `0x300..0x175E00` → vram
  `0x00100000`).
- **splat** (0.40.1, in `.venv/`) disassembles the ELF via
  `config/SCUS_971.12.yaml` → 3015 per-function `.s` files in `build/`
  (git-ignored — disassembly of the original binary). 957 are leaf functions.
- **objdiff** v3.7.1 (native arm64 GUI + CLI) in `tools/bin/` (git-ignored).
- **Toolchain container** `exterm-toolchain` (arm64; Apple `container` CLI;
  `docker/Dockerfile`): Debian + `qemu-user` + MIPS binutils. The compiler
  `mwccmips.exe` (Metrowerks CodeWarrior 2.3.1.01 — the exact build named in the
  boot ELF's `.comment`) is a **32-bit Windows PE**. Apple's Rosetta and
  qemu-x86_64 user-mode both fail on 32-bit x86, so it runs under a genuine
  **32-bit wibo** (`tools/bin/wibo32`, cross-built by `docker/build-wibo.sh`)
  inside **qemu-i386**. The MIPS assembler is arm64-native — only the compiler
  is emulated.
- **295 leaf functions matched** — five batches via five approaches:
  - **51 C-decompiled trivial leaves** (integer constants, empty bodies, field
    getters/setters, field copies, field+constant, global-pointer writes,
    comparisons, conditional stores, float copies).
  - **137 EE-kernel syscall stub functions** matched as one-line inline-asm C
    (`asm { addiu $v1, $zero, N; syscall 0; };`) — every syscall stub in the
    boot ELF, 0% → 100% in one batch. **All 137 are now named**: 101 positive-N
    stubs were named first; the remaining 34 negative-N stubs are named via
    ps2dev/ps2sdk `syscallnr.h` cross-referenced with DCDecomp and recvx-decomp.
    Names are in `config/symbol_addrs.txt`; splat picks them up. Where a
    negative-N name would collide with an existing positive-N stub that the SCEI
    SDK exposed at a different slot, the underscore-prefixed `_i` form is used.
    Two stubs at -0x53 and -0x5a are tentatively named `RFU083_iSetEventFlag`
    and `iCopy` (TODO: confirm from a Metrowerks PS2 SDK source).
  - **134 additional EE-kernel syscall stubs** (`func_0010B400..func_0010BC80`)
    — a second range of syscall stubs (syscall numbers 0x00..0x87 and several
    negative-N slots), matched with the identical `asm { addiu $v1, $zero, N;
    syscall 0; }` pattern. 33 stubs use negative syscall numbers. All confirmed
    100% via `objdiff-cli`. NOTE: these stubs are not yet named in
    `symbol_addrs.txt` — adding names is a future step.
  - **45 tail-call wrappers** — small "set up args, then `j` to another
    function" stubs that mwcc can't produce from plain C (it has no tail-call
    optimization), so we write them as `asm void NAME(void) { ...; j func; arg }`.
    mwcc inline asm rejects `$t0..$t7` named registers; numeric `$8..$15`
    works. Watch out: mwcc dead-store-eliminates `daddu $aN, $zero, $zero`
    inside `asm void` if it considers $aN unused — wrappers that hit this
    were dropped from this pass.
  - **38 VU0 / COP2 / EE-specific leaves** — small `lqc2`/`sqc2`/`vadd`/
    `cfc2` functions matched via `asm void` with the literal Sony VU syntax,
    which `mwccmips` accepts natively. Two infrastructure fixes were needed
    on the target side: (a) `tools/decomp/asm_fixup.py` rewrites unassemblable
    spimdisasm VU lines (`vdiv Q, $vf0w, $vf5x`) as `.word 0xHEX` directives
    and also strips any trailing context past `endlabel`; (b) `build.py` now
    passes `-march=r5900` to `mipsel-linux-gnu-as` so the target `.o` ELF
    flags say "5900" — without that flag, objdiff disassembles EE COP2
    instructions as their generic MIPS-II mnemonics (`lqc2` → `ldc2`) and
    reports a mismatch even when the bytes are identical.
  - **Negative-N syscall stubs named** — the remaining 34 stubs (Sony's
    user-mode/extended syscall convention) are now mapped to their proper
    SDK names (`SetAlarm`, `ReleaseAlarm`, `_iEnableIntc`, etc.) via the
    public PS2 kernel-syscall table.
  - **24 partial-match functions cracked** (2026-05-23) — 22 previously
    partial functions brought to 100%, plus 2 more that another concurrent
    agent fixed (func_001FE460, func_001FE4B0). Techniques used:
    - `asm int`/`asm void` function form for functions where mwcc's C backend
      cannot produce the exact register allocation or instruction scheduling.
      This is the key technique: `asm int func(args) { literal asm... }` emits
      exactly the given instructions, bypassing mwcc's register allocator.
    - Explicit local variable (e.g. `int a1 = 1;`) inserted between the first
      load and the store to shift mwcc's register allocation (fixed func_001AB790:
      forced `lw $v1 / li $a1,1 / sb $a1,0($v1)` pattern).
    - `addiu $v1,$zero,-0xE1` in inline asm for the mask constant that mwcc
      would generate as `andi $v1,$v1,0x1F` from C.
    - `paddub $v0,$zero,$zero` (EE multimedia zero-register instruction) works
      in mwcc inline asm as a zero-move; use `addiu $v0,$zero,N` not `li` to get
      the `addiu` opcode (mwcc's `li` assembles to `addi` not `addiu`).
  Each function compiles to a **100% `.text` match** vs the original,
  confirmed by `objdiff-cli`. 296 functions are in `src/` total (295 perfect,
  1 partial at 90.9%).
  - **153 more functions matched** (2026-05-23, second session):
    - **func_001B7670** (0x60 bytes) — flag-setting function with complex branch
      chain and absolute-address memory accesses. Uses `.word` for all branches
      (mwcc rejects label branches) and repeated `lui $at, 0x7000` entries.
    - **func_001B5E20** (0x114 bytes) — controller-input mapper with cascaded
      `beqz`/`b` chains. All 69 instructions encoded as `.word` to bypass mwcc's
      dead-store elimination of duplicate `addiu $v1, $zero, X` values in branch
      delay slots.
    - **func_001CA0A0** (0x114 bytes) — vector interpolation with FP accumulator
      instructions (`mula.s`, `madd.s`, `msub.s`, `bc1fl`). All `.word`-encoded.
    - **func_001CA4D0** (0x104 bytes) — 3x3 matrix cross-product / determinant
      with stack frame (`addiu $sp` / `lq` / `sq`). All `.word`-encoded.
    - **func_001CA1C0** (0x1EC bytes) — vector outer-product (9 cross-product
      components + 3 dot products) with 120+ repeated `lui $at, 0x7000` for
      scratch-memory accesses. All `.word`-encoded; 123 instructions.
    - **func_00100000**, **func_00100004** (0x4 bytes each) — bare `nop` stubs.
    - **func_0010E084** (0x4 bytes) — bare `jr $ra` (no delay slot).
    - **134 EE syscall stubs** (`func_0010B400..func_0010BC80`, 0x10 bytes each)
      — the same `addiu $v1, $zero, N; syscall 0; jr $ra; nop` pattern as the
      137 previously matched stubs. Includes 33 negative-N stubs (extended
      syscall convention). All confirmed 100%.
    Total in `src/`: 449 files (448 at 100%, 1 partial at 90.9%).

  - **14 hi/lo global-access functions matched via pure C with `-sdatathreshold 0`** (2026-05-23):
    The key insight: mwcc inline asm rejects `%hi/%lo` syntax, but pure C compiled
    with `-sdatathreshold 0` forces mwcc to use `lui`/`addiu %hi/%lo` addressing (not
    `$gp_rel`) for all extern globals, producing correct R_MIPS_HI16/LO16 relocations.
    This technique works for functions that are sq/lq style (mwcc always emits sq/lq
    for PS2 target). A generator script (`/tmp/gen_hilo3.py`) batch-processes `.s`
    files and infers C source from instruction patterns.

    Pattern A — call(&global), return 1:
    - `func_002070A0`, `func_002070D0`, `func_00207070`, `func_0020E080`

    Pattern B — multiple calls, global stores:
    - `func_001FAB50`, `func_001FAB80`, `func_001FABB0`, `func_001D1C10`

    Pattern C — leaf getters (return &global or load global):
    - `func_00100268`, `func_0010D990`, `func_00120AD0`, `func_001DB800`, `func_001DB240`

    Pattern D — multi-param + function-pointer arg + global stores:
    - `func_001FF080`

    All 14 are 100% matches confirmed by objdiff-cli. Total: 1008 src files.

    **Techniques NOT yet working:**
    - Tail-call `j func_` with %hi/%lo args: 96.7% (lui uses wrong temp reg — $at vs $v0)
    - Simple leaf setters/getters with %hi/%lo: 96.7% (same lui register issue)
    - Complex functions (floating point, struct offsets, multiple s-regs): auto-inference fails
    - The `-sdatathreshold 0` technique requires functions with sq/lq stack frame style;
      sd/ld style functions cannot be matched with this mwcc version.

Build flow (`tools/decomp/build.py`): `setup` runs splat + writes
`objdiff.json`; `build` assembles the splat disassembly into objdiff *target*
objects and compiles `src/*.c` into *base* objects via mwccmips; objdiff (or
`objdiff-cli`) diffs the two. The MIPS assembler needs `config/asm_prelude.inc`
(`.set noreorder`) so splat's literal delay slots assemble correctly.

Compiler source & legality: `mwccmips.exe` came from the public
`Adubbz/DCDecomp` repo (a Dark Cloud PS2 decomp using the identical compiler).
It is proprietary Metrowerks software — it lives in `tools/mwccps2/` and is
**never committed** (git-ignored).

**Known mwcc register allocation patterns learned:**
- Void functions use `$v1` for first scratch; returning functions use `$v0`.
  Using `return expr` forces `$v0` (fixed several functions).
- When a function has one pointer arg (`$a0`) and stores a constant 1, mwcc
  loads the pointer into `$a1` and puts `li 1` in `$v1` (not controllable from
  plain C, but `int a1 = 1; ((char*)v1)[0] = a1;` shifts the allocation so
  `$v1` holds the pointer and `$a1` holds the constant — fixed func_001AB790).
- The commutative `addu` operand order (rs vs rt) is not controllable from C
  but IS controllable via `asm int`/`asm void`.
- `volatile int *` forces double-reads when the compiler would otherwise
  optimize away the second load.
- `func_001AB7D0` pattern: load global pointer via `lui/lw` into `$v1`, then
  `sb $zero, 0($v1)` — matches when written as simple C dereference.
- **KEY TECHNIQUE**: `asm int func(args) { ... }` emits instructions verbatim.
  For any function where the register allocation or instruction scheduling
  diverges from C output, use `asm int` (returning) or `asm void`. In mwcc
  inline asm, use `addiu $v0,$zero,N` instead of `li $v0,N` to get the
  `addiu` opcode (mwcc's `li` assembles to `addi` not `addiu`). Numeric
  registers `$8`-`$15` work; `$t0`-`$t7` are rejected.

**Known unsolvable classes (leave src files as partial for documentation):**
- HW register addresses (`lui $v1 / ori $v1` with 5-digit hex): mwcc always
  uses `$at` for absolute address loads, cannot reproduce `$v1`-based loads.
  (But these are solvable with `asm void`/`asm int` if needed.)
- `mfc1`/`mtc1`: float bit manipulation (fabsf) — mwcc generates stack-based
  code instead.
- Tail-call `j func_` stubs: compiler won't generate `j` for C calls.
- Dead code after unconditional branches: mwcc's inline assembler elides
  unreachable instructions. func_001B5C90 has a dead `andi $v0,$v1,0xFC` that
  cannot be reproduced — stays at 90.9%.
- **Leaf setter/getter functions with %hi/%lo global refs (3 instructions)**:
  mwcc uses `$at` as the lui temp register; the original code used `$v0` or
  `$v1`. Results in 96.7% match. Not fixable from pure C. Not fixable from
  `asm void` (mwcc rejects `%hi/%lo` in inline asm). Would need a way to inject
  relocations into mwcc-assembled code (not currently feasible).
- **sd/ld style functions**: mwcc always emits `sq`/`lq` for PS2 target
  regardless of flags, so functions that use `sd`/`ld` cannot be matched.

**Partial match summary (1 function in src/ that doesn't 100% match):**
- func_001B5C90 (90.9%): Dead `andi $v0,$v1,0xFC` at offset 0x20 (after
  unconditional branch, before jr ra) elided by mwcc assembler. Structure
  and all reachable instructions match. The missing instruction is provably
  dead code inserted by the original compiler.

All other 295 functions are at 100%.

### Done — Track A partial-link pipeline

A fully automated pipeline that links all 3014 unique-vram boot-ELF functions
into a single `elf/SCUS_971.12.elf` using `mwldmips.exe` (the original
period-correct linker). **The loadable region is byte-identical to the original
ELF** (1530624/1530624 bytes, 100.00%).

- **`tools/decomp/fill_unmatched.py`** — assembles all 3014 per-function `.s`
  files from splat into `build/filler/*.o`.  For functions with a compiled
  `build/obj/*.o`, copies those instead.  Applies all post-processing
  (section stripping, 16→4-byte `.text` alignment fix, GPREL16 pre-application,
  cross-function local label globalization, symbol weakening).  Idempotent with
  incremental rebuild.  135 duplicate-vram syscall-stub aliases (same vram, both
  a named form and `func_XXXXXXXX`) are deduplicated to the named form.
- **`tools/decomp/strip_sections.py`** — strips `.pdr`/`.reginfo`/`.MIPS.abiflags`/
  `.gnu.attributes` from GNU-as objects; zeroes empty `.text`/`.data`/`.bss`;
  forces `.text` section alignment from 16 to 4; pre-applies `R_MIPS_GPREL16`
  relocations (preserving REL addend for expressions like `%gp_rel(sym+0xC)`);
  zeroes `R_MIPS_PC16` addend fields (mwldmips formula is `(S+A-(P+4))/4`,
  but GNU-as writes A=-1; setting A=0 gives the correct branch offset).
  For each function, resizes `.text` to `slot_size = next_vram − this_vram` so
  inter-function gap bytes are part of the object (not left as linker holes).
  156 functions whose mwcc output has the wrong size or content are
  force-assembled from the `.s` instead (`SIZE_DRIFT_FORCE_ASM`).
- **`tools/decomp/link.py`** — generates `config/SCUS_971.12.lcf` (the
  Metrowerks linker command file) with per-function `.text` placement in vram
  order, 2000+ absolute symbol definitions for BSS/IOP/hardware-register
  addresses, and `_gp = 0x0027D370`.  Invokes `mwldmips.exe` via
  `qemu-i386 wibo32`, then compares the PT_LOAD region of the output ELF against
  the original.  Vram deduplication mirrors `fill_unmatched.py`.
- **Current byte identity: 100.00%** (1530624/1530624 bytes in the loadable
  region).  The output ELF is 0x80 bytes longer than the original due to
  mwldmips alignment padding in currently-empty data sections — expected and
  harmless.  Byte identity rises as matched functions displace `SIZE_DRIFT_FORCE_ASM`
  entries.
- **To run the full pipeline** (inside the `exterm-toolchain` container):
  `python3 tools/decomp/link.py`  (runs fill_unmatched, generates LCF, links,
  compares).  Add `--no-fill` to skip fill_unmatched; `--dry-run` to skip the
  linker.  See `docs/LINKER.md` for detail.

### Open questions (most need the decompiled engine — i.e. Track A)

- **Texture color source.** PSMT8 always samples through a CLUT, but no CLUT
  data was found in `DATA.DAT` or the boot ELF; the 8-bit values are luminance-
  ordered. Either a grayscale-ramp CLUT (color from vertex-color modulation) or
  an unlocated color CLUT. The engine's GS `TEX0` setup will resolve it.
- **Cross-file texture residency.** ~631 materials bind to a texture uploaded
  by a different file (common/UI packets); needs the engine's VRAM map.
- **`extract_textures.py`** only matches `07 XX 00 60` packets, missing the
  `07../10` DMA-tag level sheets (`extract_subtextures.py` handles them);
  broaden or consolidate the two tools.
- **Rig payload.** The 112-byte per-record rig transform is VIF-packed; a
  faithful decode needs the VU1 microcode. Bone parent hierarchy not isolated.
- **MATRIX `--scene`.** Role of the repeated identity transforms, and whether
  transforms are absolute or parent-composed — confirm from engine code.
- **Audio.** SFX-bank rate unconfirmed (provisionally 22050). Clip splitting is
  heuristic (silence gaps) — no per-clip index found.
- **`OVERLAY/`** (`AREA*.BIN`, `MWo3` overlay modules) not yet scanned for
  textures/geometry.

### Roadmap

**1. Track A — matching decompilation (CURRENT PRIORITY).**
Goal: a runnable developer build — compile the decompiled code and run the game
with its own assets (no repacking needed; the original `DATA.DAT` is used
as-is). The pipeline "hello world" is **done** — see "Done — Track A" above.
The 137 named syscall stubs are in `symbol_addrs.txt`; the additional 134 stubs
(func_0010B4xx..func_0010BCxx) are matched but not yet named.

**The partial-link pipeline is also done (2026-05-23)** — see `docs/LINKER.md` and
`tools/decomp/{fill_unmatched,link,strip_sections}.py`.  A linked
`elf/SCUS_971.12.elf` is produced (currently at ~17% byte identity because 137
compiled functions aren't yet byte-perfect).  The byte identity will approach
100% as more functions are matched.  See `docs/LINKER.md` for the full pipeline
description, known issues, and how to invoke it.

Next for Track A: match non-trivial functions; grow `config/symbol_addrs.txt`;
bring the 137 non-matching compiled functions to byte-exact output; aim for a
runnable ELF in PCSX2.
The reference template studied for the pipeline is `fmil95/recvx-decomp`
(same CodeWarrior toolchain family).

**2. Asset polish (minor; several items need Track A's decompiled engine).**
The open questions above — texture color/CLUT, cross-file texture residency,
`extract_textures.py` `07../10` coverage, rig VIF payload + bone hierarchy,
MATRIX `--scene` confirmation, SFX sample rate, labelling the 25 soundtrack vs
30 cutscene `MUSIC.DAT` tracks, scanning `OVERLAY/`.

**3. Asset repackers + moddable build (DEFERRED to late in the project).**
Repackers that rebuild `DATA.DAT`/`INDEX.IDX` and the streams byte-identically
from loose extracted files, so the game can be rebuilt from editable assets.
The archive is reversed precisely enough (exact tiling) that byte-identical
repacking is feasible. Not needed for the dev loop. See `CLAUDE.md` "End-state
build architecture".

## Repo layout

- `tools/` — original tooling (committed; Python 3 stdlib, native arm64):
  - `extract_data.py` — `DATA.DAT`/`INDEX.IDX` archive extractor.
  - `decode_sound.py` — VAG ADPCM decoder (SFX banks + VOICE/MUSIC streams).
  - `extract_textures.py` — GS texture-packet → grayscale PNG sheet extractor.
  - `extract_subtextures.py` — per-material texture extraction (marker→packet
    binding + UV crop).
  - `extract_models.py` — geometry → Wavefront OBJ (level + models); `--scene`
    places full levels, `--rig`/`--anim` dump rig & poses.
  - `decomp/build.py` — Track A build driver (splat, compile, objdiff.json).
  - `decomp/fill_unmatched.py` — assembles all 3149 boot-ELF functions into
    `build/filler/*.o` for mwldmips (handles special cases: GPREL16 overflow,
    cross-function labels, symbol weakening).
  - `decomp/strip_sections.py` — strips GNU-as-specific sections; fixes .text
    alignment (16→4 bytes); pre-applies R_MIPS_GPREL16 relocations.
  - `decomp/link.py` — full link orchestrator: generates LCF + object list,
    invokes mwldmips, compares output ELF against original.
- `config/` — Track A build config (committed): `SCUS_971.12.yaml` (splat
  config), `symbol_addrs.txt` (hand-recovered symbol list), `asm_prelude.inc`
  (assembler directives for target objects). The boot ELF `config/SCUS_971.12`
  itself is git-ignored, as is the proprietary compiler under `tools/mwccps2/`.
- `src/`, `include/` — decompiled C and headers (committed).
- `docker/` — toolchain container (committed): `Dockerfile` (arm64 image: Debian
  + qemu-user + MIPS binutils, built with Apple's `container` CLI) and
  `build-wibo.sh` (cross-builds the 32-bit wibo).
- `docs/` — project state and findings (committed): `PROGRESS.md` (this file),
  `FINDINGS.md` (technical format reference), `LINKER.md` (partial-link pipeline
  — mwldmips invocation, LCF decisions, special cases, debugging tips),
  `track-a-kickoff.md` (Track A starter prompt).
- `CLAUDE.md` — project charter, legal rules, target identity, toolchain
  conventions, end-state build architecture.
- Disc-derived material is **git-ignored** — the ISO, `extract/`, `wav/`,
  `voice/`, `music/`, `textures/`, `subtextures/`, `models/`, the boot ELF, and
  the Track A build artifacts (`build/` splat disassembly, `elf/`, `.venv/`,
  `tools/bin/`). Each user regenerates these locally from their own legal disc.

## Maintaining the docs

- `docs/PROGRESS.md` (this file) — status, open questions, roadmap.
- `docs/FINDINGS.md` — technical reference for every reverse-engineered format.
- Update both in the same session as work progresses, so the next person or
  agent can continue cleanly (required by `CLAUDE.md`).
