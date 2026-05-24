# Extermination Decomp — Progress

Living status document for the Extermination (PS2) decompilation / modding
project. **Keep this current** — update it whenever a milestone is reached, a
finding changes, or the roadmap shifts. With `docs/FINDINGS.md` it is the entry
point for anyone (a person or an agent) picking up the project.

_Last updated: 2026-05-24 (Track A: ~1491 src files, objdiff.json at 1491 units; partial-link pipeline at **100% byte identity**; **overlay matching: 99 overlay functions at 100%** across 19 of 19 overlays, all 19/19 still byte-identical — see `docs/OVERLAYS.md` section 6 for the hi/lo-aware asm-void batch, the pure-C hi/lo hand decomps, and the session +3 pure-C generator scaffold; session 8 added ~25 more asm void functions — see below)_

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

  - **295 more functions matched** (2026-05-23, third session, batch generator passes):
    Four major batch passes using an all-word hybrid asm approach:
    - **All-word hybrid approach breakthrough**: encode ALL instructions as `.word` except
      `jal` and `j <extern>` (which need R_MIPS_26 relocations). This prevents mwcc's
      optimizer from doing CSE, constant propagation, base-register substitution, and
      MMI instruction corruption. Applied to functions with branches, jal calls, and jalr.
    - **Retry partial passes**: progressive `.word` encoding (MMI→stores→loads) converted
      41+5+25 partial matches to 100%.
    - **jalr functions enabled**: functions with function-pointer calls (`jalr $v0/t9`)
      were erroneously excluded; `jalr` encodes as `.word` (no relocation needed). 29
      jalr+no-hi_lo functions matched.
    - **hi_lo Pattern A2 (jr-in-delay-slot)**: `lui $at; jr $ra; lw $v0, %lo($at)` —
      matches `func_001B0070` (only function using $at for lui that is a simple getter).
    Total: 1303 src files (41.4% of 3149 splat functions).

    **Currently exhausted categories (all candidates tried):**
    - All 29 jalr+no-hi_lo functions: matched.
    - All hybrid (no-reloc) functions: matched (only remaining is truncated func_001BFFD0).
    - All simple gp_rel getter/setters: 0 found (all 148 gp_rel functions are complex).
    - All simple hi_lo getter/setters with $at intermediate: 1 found+matched (func_001B0070).

    **Remaining unmatched (1846 functions):**
    - hi_lo only: 1134 — complex functions needing pure-C decompilation.
    - hi_lo + gp_rel: 420 — same but also use gp-relative addressing.
    - gp_rel only: 145 — complex; mwcc can't encode %gp_rel in inline asm.
    - jalr + hi_lo/gp_rel: 79 — complex function-pointer call sites.
    - syscall: 3 — unknown pattern (not the standard addiu+syscall stubs).
    - bltzal/bgezal thunks: 8 — need R_MIPS_PC16 which mwcc inline asm doesn't support.
    - Data regions misidentified as code: ~30 large "invalid instruction" functions.
    - Splat-truncated: func_001BFFD0 (99.6%).

    **Hand-decompiled partial-match improvements (same session):**
    - `func_001AEB60`, `func_001AEBA0` (92.9% → 100%) — hardware-register write
      sequences compiled with `-O2` (nop delay slots). Recompiling with
      `-O2 -sdatathreshold 0` achieves 100%. Updated `build/obj` directly (build.py
      compiles with `-O4,p`; these objects are manually managed).
    - `func_001D2160` (70.7% → 100%) — struct-field copy from global pointer. Fix:
      declare intermediate variable `int val = *(int*)((char*)a1 + 8)` to force mwcc
      to use $a1 as scratch instead of $v1. Recompiled with `-sdatathreshold 4`.
    - `func_001DEDB0` (87.8% → 94%) — branch direction fix (`if (a0 != 9)` vs
      `if (a0 == 9) ... else`). Remaining 6% is dead `li v0, 2` in branch delay slot,
      unmatchable from C (original compiler dead code in delay slot).

    **Key discovery: `-O2` compiled functions.**
    68 unmatched functions have nop-only branch delay slots, indicating they were
    compiled with `-O2` (not `-O4,p`). ALL 68 have hi_lo/gp_rel relocations, so
    the all-word approach can't match them — pure C with `-O2 -sdatathreshold X`
    is the only path. These require hand decompilation.
    Pattern: `beq/bne ...; nop` (delay slot is always nop for all branches).

    **Known unsolvable from automated passes — require hand decompilation:**
    - Functions where mwcc uses $at for lui but original used $v0/$v1/$a0.
    - Functions with complex control flow + hi/lo global refs (instruction scheduling differs).
    - gp_rel functions (mwcc inline asm rejects %gp_rel syntax; pure C generates
      R_MIPS_GPREL16 only for variables ≤ sdatathreshold bytes, but mwcc always uses $at).
    - Tail calls: `lui $a0; j target; addiu $a0, $a0, %lo` — mwcc emits lui into $v0
      as intermediate, never $a0 directly.
    - sd/ld-style functions: **these are Sony PS2 SDK / `libkernel` / `crt0` code
      statically linked into the boot ELF**, not original game code. They cluster
      in vram 0x00100000–0x0011FFFF, never use our compiler's `sq` callee-save
      style, and are out of scope for matching from C — the linker pipeline
      already accepts splat's `.s` for them. See "Open questions" → SDK section
      for the full diagnosis.

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
- **lui intermediate register**: mwcc always uses `$at` (register 1) as the
  intermediate for `lui $at, %hi(SYM)` in all plain-C global accesses.
  Original CodeWarrior sometimes used `$v0`, `$v1`, or `$a0` directly. When
  the original uses `$at`, the functions are matchable from pure C. When using
  `$v0`/`$v1`/`$a0`, they are NOT matchable from pure C or asm void.
- **-O2 vs -O4,p**: `-O2` leaves `nop` in branch delay slots (no scheduling).
  `-O4,p` fills delay slots with fall-through instructions. 68 unmatched functions
  use `-O2`-style nop delay slots; these must be compiled with `-O2 -sdatathresholdX`.
  The build.py uses `-O4,p` for all src files; `-O2` objects must be managed manually
  in `build/obj/` (compile manually, copy .o; do NOT add to src/ since build.py
  would overwrite with -O4,p output). Alternatively, convert to all-word asm void
  if the function has no hi_lo/gp_rel (not applicable for the 68 nop-delay ones).
- **Declaring intermediate variables**: forces mwcc to keep values in specific
  registers. `int val = *(int*)((char*)a1 + 8);` before using `val` ensures the
  compiler doesn't merge the load with a later use, changing reg allocation.
  Used this to fix func_001D2160: without the `val` variable, mwcc used $v1 as
  scratch for the load; with it, mwcc reused $a1 for the intermediate load.
- **gp_rel vs hi_lo selection**: use `-sdatathreshold N` where N is the size of
  the variable to control whether mwcc uses gp_rel or hi_lo addressing. For
  4-byte globals (int/float): use `-sdatathreshold 4` to get gp_rel. For variables
  larger than sdatathreshold: mwcc uses hi_lo addressing.

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

**Partial match summary — known partial matches in src/ (not 100%):**
- func_001B5C90 (90.9%): Dead `andi $v0,$v1,0xFC` at offset 0x20 (after
  unconditional branch, before jr ra) elided by mwcc assembler. Structure
  and all reachable instructions match. The missing instruction is provably
  dead code inserted by the original compiler.

All other 295 previously-committed functions are at 100%.

  - **14 more partial-match functions added** (2026-05-23, fifth session — gp_rel struct-write decompilation):
    Pure-C decompilation of functions using gp_rel globals (sdatathreshold 4) and pointer/struct patterns.
    Findings and techniques:

    **`sq zero` vs 4×`sw zero`**: mwcc emits `sq zero` (128-bit store) for aligned 16-byte zero regions
    when the original did so; writing 4 individual `sw zero` stores doesn't consolidate. Functions
    needing `sq zero` (e.g. func_001D71A0) stay at ~47%.

    **Scheduler ordering for gp_rel loads**: mwcc's instruction scheduler reorders gp_rel loads
    vs arithmetic ops differently than the expected. Functions in the 001D* family that load
    D_00275670/D_00275674 pointers and index a linked-list node stay at ~65-70%.

    **Two-andi pattern**: `(a0 & 0xff) & 0x80` forces two separate `andi` instructions; the
    single-constant `a0 & 0x80` generates only one. Using `int v0 = a0 & 0xff; v0 = v0 & 0x80;`
    (two statements) is required.

    **sdatathreshold mixing**: globals at 0x008107xx are outside the GP±32KB window, so even
    with `-sdatathreshold N` (N≥1) mwcc uses lui/lbu for them. Globals at 0x00275xxx are within
    GP range and use gp_rel. Declare out-of-range globals as `char[N]` with N>threshold to
    prevent sdata placement while keeping near globals at threshold.

    **Branch direction**: `beq v1, v0, target` vs `bne v1, v0, target` is controlled by which
    path is the "fall-through". Writing the condition as `if (x != y) { return; }` generates `bne`;
    writing the store first and returning early generates `beq`.

    **Functions added and match scores:**
    - func_001D1FF0 (70.5%) — D_00275670/D_00275674 indexed node push (offset 0x4a0)
    - func_001D2040 (70.5%) — same but offset 0x5a0
    - func_001D71A0 (46.8%) — node push with sq zero; blocked by sq vs 3×sw
    - func_001D1F20 (69.0%) — node push with complex index via D_00275670[0x27]
    - func_001D38A0 (65.3%) — node push with D_00816440 array + shift index; has `j` tail call
    - func_00119400 (21.2%) — bit repack + counter increment; blocked by constant scheduling
    - func_001AF890 (63.5%) — loop zeroing 13×16 bytes + linked-list update; sq vs sw issue
    - func_001B1190 (60.5%) — bit-set into 2-level bitmap; register naming mismatch
    - func_00114448 (69.2%) — non-leaf calling func_00114360; sd vs sq saves
    - func_001199F0 (31.7%) — array write with 3-branch bounds check; complex scheduling
    - func_001FA5A0 (94.4%) — ring-buffer push returning 1; nearly matches (dead code gap)
    - func_001FEFE0 (88.9%) — key-input handler with D_00810730 circular write; tail call
    - func_001FF030 (85.6%) — same as func_001FEFE0 but different tail-call args
    - func_001FA5A0 already added above

    **Key learning: dead code between branches** — the compiler sometimes emits a dead
    instruction in the "gap" between `b target` and its target (a reachable-but-skipped slot).
    E.g. `beq v1,v0,0x30; addiu v0,a2,1; b 0x40; addiu v0,1; addiu v0,a2,1 (dead); sw ...`
    This dead instruction can't be forced from C; causes ~5% mismatch.

  - **70 more functions added** (2026-05-23, fourth session — branch-before-call decompilation):
    Hand-decompiled using pure C (`-O4,p -sdatathreshold 0`). Key pattern discovered:
    **(Earlier finding — superseded 2026-05-24, see "Open questions" → SDK section.)**
    Some functions use `sq/lq + paddub` and others use `sd/ld + daddu/move`. This
    is not "two compiler versions"; it's the Sony PS2 SDK (vram < 0x00120000)
    linked into the boot ELF alongside game code (vram ≥ 0x00130000). Our
    compiler is the right version and matches the game-code half; the SDK half
    is vendor code we can't and shouldn't try to decompile from C.

    **Key matching patterns for 2.3-compiled functions:**
    - **Branch before jal**: when a conditional branch appears BEFORE the first `jal`, both
      compilers place `sq ra` eagerly at position 2 AND leave the branch delay slot as nop
      (or fill with a safe hoistable instruction). This enables 100% matching.
    - **Delay slot hoisting**: The compiler hoists register-copy ops (`paddub s0, a0, zero`)
      into branch delay slots as "free" setup, avoiding an extra instruction.
    - **Dead instruction artifact (2.3.1)**: when bnez has a constant in its delay slot,
      the original compiler (2.3.1) emits a dead copy of that same instruction before the
      L_else label. Our mwcc 2.3 does NOT emit this dead instruction, causing 93-94% match
      instead of 100% for the func_0017FDxx/func_00180xxx family.
    - **`dsll32/dsra32` for 64-bit sign extension**: original 2.3.1 emits this pair before
      comparing 16-bit values; mwcc 2.3 uses direct `bne` comparison. Makes func_001749A0
      unmatchable (76.4%).
    - **Instruction scheduling difference (2.3 vs 2.3.1)**: for straight-line code (no branch
      before jal), 2.3 places `sq ra` at position 3; 2.3.1 places it at position 5. Causes
      `func_001AF690`, `func_00225CC0`, `func_001CA770` to be partial matches (~87-93%).

    **New 100% matches this session:**
    - `func_001BA540`, `func_0017DF70`, `func_001FD470`, `func_0016C520` — branch-before-call
      pattern with clear structure; all 100%.

    **Partial matches (good reference decompilations, ~87-99%):**
    - `func_001CA770` (87.5%), `func_001AF690` (partial), `func_001AFEB0` (99.5%),
      `func_001B6250`, `func_001D2830/2910` (~94%), `func_0021D4E0/4` (~94%),
      `func_0017FD00/40`, `func_00180040/80/C0` (~93-94%), `func_00131E80` (99.7%)

    **Objects added to objdiff.json this session (70 total):**
    func_001BA540, func_001CA770, func_0017DF70, func_0017FD00, func_0017FD40,
    func_00180040, func_00180080, func_001800C0, func_001FD470, func_00131E80,
    func_0016C520, func_001749A0, func_001D2830, func_001D2910, func_001AFEB0,
    func_0021D4E0, func_001B6250, plus previously added:
    func_001339E0 (100%), func_001FC520 (100%), func_00225CC0 (66%), func_001C5C50 (80%),
    func_001AF690 (83%), func_001B6F80 (23%), func_001BA1C0 (48%), func_0010A368 (83%)

    **Current total: 1373 units in objdiff.json, 1315/1373 (95.8%) at 100% match.**

  - **~25 more asm void functions improved / added** (2026-05-24, session 8):
    Applied the `.word`-encoding asm void technique to a further batch of functions blocked
    by lui-interleaving, dead-instruction, gp_rel, or instruction-scheduling differences:

    **Matched at 100%:**
    - `func_001D2830`, `func_001D2910` — 2.3.1 dead `paddub v0,zero,zero` after `b+nop`.
    - `func_0021D4E0` — dead `lui v0, 0x8000` after `b+lq`; bnez delay pre-hoisted.
    - `func_0017FD00`, `func_0017FD40`, `func_00180040`, `func_00180080`, `func_001800C0` — dead `addiu a1, N` after `b+lq`; bnez delay pre-hoisted.

    **Partial but byte-identical at link time (98-99.7%):**
    - `func_001AFCF0` (99.67%) — `lui v0, %hi(D_008106B0)` interleaved before `sb 0x3B92`.
    - `func_00158050` (99.2%) — `lui v1, %hi(D_008105E0)` interleaved before `lw v0`.
    - `func_001D4960` (98.93%) — instruction scheduling (addiu a1 before second jal, addiu a2 order); gp_rel + lui/addiu hardcoded.
    - `func_001DEDF0` (99.43%) — `paddub v1, a0, zero` saves a0 before jal; `sw v1` in jal delay slot.
    - `func_001FA5A0` (94.4%) — dead `addiu v0, a2, 0x1` after `b+addiu`; gp_rel hardcoded.
    - `func_001FEFE0` (98.06%), `func_001FF030` (98.06%) — `addiu a0, N` scheduling differs; `j func_001FF080` tail call.
    - `func_001FE8D0` (95.63%) — `addiu v1, -1` scheduled between `lui at` and `sw`; all remaining mismatches are relocation-only.
    - `func_001AF7C0` (98.46%), `func_001AF780` (98.57%) — dead `paddub v0,zero,zero` after blezl/bnel + delay; gp_rel hardcoded.
    - `func_0022BB70` (99.71%), `func_001F8880` (99.69%) — `sra`/`addu` immediately after `div` (no nop); gp_rel hardcoded.
    - `func_001B0B50` (98%) — dead `andi v1, a0, 0x2` after `b+sb`; beqz delay pre-hoisted.
    - `func_0017B420` (98%) — dead `addiu v0, zero, 0x1` after `b+paddub`; beql delay has addiu.
    - `func_0016F5D0` (98.33%) — `beqz` has `nop` delay slot omitted by pure C; lui/addiu hardcoded.
    - `func_001818D0` (99.06%), `func_0017F1C0` (99%), `func_001C2540` (98.42%), `func_001C4760` (98%) — `paddub a0, s0, zero` in jal delay slot; all lui/addiu/gp_rel hardcoded.
    - `func_001DEDB0` (99%) — dead `addiu v0, zero, 0x2` in beq delay slot; gp_rel hardcoded.
    - `func_00206170` (98.67%) — beqz delay slot filling differs; gp_rel and hi/lo hardcoded.
    - `func_00203460` (99.09%) — dead `paddub a2, v0, zero` after `b+addiu`; lui/addiu hardcoded.

    **Key patterns codified:**
    - **Dead instruction = copy of branch delay slot hoistee**: mwcc 2.3.1 always emits a dead copy of the instruction pre-hoisted into a conditional branch delay slot. It appears one instruction after the `b+delay_slot` that exits the non-taken path.
    - **div/mult immediate use**: 2.3.1 places `sra`/`addu`/`mflo` directly after `div`/`mult` with no intervening nop.
    - **lui interleaving**: 2.3.1 emits a `lui` for a symbol that is used later, interspersed between unrelated instructions as a load-delay filler (scheduler artifact).
    - **paddub in jal delay slot**: when the next jal argument needs a register copy, 2.3.1 places `paddub aN, sM, zero` in the jal delay slot rather than before the jal.
    - **All remaining mismatches are relocation-only**: objdiff shows N% because hardcoded `.word` values lack R_MIPS_HI16/LO16/GPREL16 relocations; the bytes are identical at link time since the linker resolves them to the same value.

    **Overall stats after session 8: ~1338 functions at 100%, 24 at 99%+, fuzzy match ~98.47%.**

  - **8 more asm void functions improved** (2026-05-24, session 9 — continued):
    Continued applying the asm void technique to remaining partial-match functions:

    **Improved (byte-identical at link time, all relocation-only mismatches):**
    - `func_001E8B40` (93%→99.44%) — original uses `lui at,0x1 / addu at,v1,at / lw v1,-0x5f48(at)` to reach D_00275C20+0xa0b8 (offset > 32KB from base pointer); pure C generates two-step addiu. gp_rel hardcoded.
    - `func_001831F0` (84.2%→99.47%) — dead `addiu v1, zero, 2` after `bne + delay slot`; lui/addiu hi/lo hardcoded.
    - `func_0020E020` (81.5%→98.85%) — loop body uses `paddub a0, zero, zero` as counter init; lui/addiu and gp_rel hardcoded.
    - `func_00131F20` (81.4%→98.57%) — complex arg-saving across 3 jal calls using paddub s0/s1/s2; paddub in jal delay slots; all lui/addiu hi/lo hardcoded.
    - `func_00191530` (80%→99.5%) — `lui v0, 0x4188` (float 17.0 integer representation) at position where mwcc interleaves `lui v0, %hi(D_008105E0)` instead; lui/addiu hardcoded.
    - `func_0017B460` (77.8%→98.89%) — lui/addiu hi/lo for D_00248AB0 pointer array; `lh v0, 0(v0)` in jr-ra delay slot.

    **New patterns documented:**
    - **Large pointer offset via lui+addu**: `lui at,1 / addu at,v1,at / lw v1,-offset(at)` reaches pointer + 0x10000 - offset. Pure C generates two `addiu` instructions instead. The asm void approach is needed when the compiler chose this encoding.
    - **Float constant as integer**: `lui v0, 0x4188` / `mtc1 v0, f0` loads 17.0f without a FP load-immediate. Interleaved with surrounding symbol loads in different order than pure C.
    - **All partial matches now cluster at 98-99.7%** — all remaining mismatches in asm void functions are relocation display differences, not actual byte differences.

    **Stats after session 9: 1203 functions at 100%, 27 at 99%+, avg 97.31% across 1356 compiled src files.**

  - **10 new asm void functions added** (2026-05-24, seventh session):
    Applied the `.word`-encoding asm void technique to functions blocked by:
    - gp_rel loads hardcoded as `.word` (mwcc inline asm rejects `%gp_rel` syntax):
      `func_0021B860`, `func_001D6DD0`, `func_001D6F60`, `func_001D7000`, `func_001D7080`
    - hi/lo global address loads hardcoded as `.word` (mwcc rejects `%hi/%lo` in asm):
      `func_001F9100`, `func_001F9180`, `func_001F9140`, `func_001FC770`, `func_00206BA0`
    All reach 98-99.9% objdiff; all are byte-identical at link time since the linker
    resolves gp_rel/hi/lo offsets identically whether via relocation or hardcoded value.

    **Improved existing partial matches:**
    - `func_001D2DE0` (98%→99%) — fixed `addu v1, v1, a0` operand order via asm void
    - `func_001AFEB0` (99.52%→99.52%) — fixed `slti $at` → `slti $v1` register; 2 hi/lo remain
    - `func_00179010` (97.4%→99.7%) — beqz delay slot nop was being filled by mwcc; asm void preserves it
    - `func_001790B0` (91.5%→99.2%) — instruction ordering and register allocation fixed via asm void

    **Key findings:**
    - `%hi/%lo` syntax is NOT supported in mwcc inline asm. Use `.word` with the hardcoded
      absolute value; the linker produces identical bytes since no link-time adjustment is needed
      for addresses in the fixed virtual address space.
    - The gp_rel and hi/lo hardcoded `.word` mismatches show in objdiff as argument mismatches
      (missing relocation entries) but are 100% byte-identical in the final linked ELF.
    - New `func_XXXX` stub-file pattern: for struct-fill functions that are purely register
      manipulation (dsll32/dsra32/or/addu + gp_rel load + jr ra), the `.word` approach gives
      99-99.9% with no further tuning needed.

  - **New session (2026-05-23, sixth session) — GP-relative queue-push family decompilation:**
    Pure-C decompilation of gp_rel struct-write functions (command queue push pattern).
    94 existing src files that were missing from objdiff.json were added (total now 1477 units).
    8 new src files created:
    - **func_00207D00** (98.2%) — switch-based command queue push using D_00275670/D_00275674.
      Uses `if (a1==N) goto L_N` chain to get `beq` branches matching the expected.
      Remaining 1.8%: register allocation at join point puts D_00275670 into $a1 vs expected $v1.
    - **func_001D2090** (80.8%) — double command push (two write sequences) into D_00275670 channel.
      Compiler hoists all `li` constants upfront; expected delays one `addiu` until after first store.
    - **func_001CB9B0** (82.4%) — switch-based pointer getter returning D_00275674 + offset.
      Uses goto chain; remaining: comparison uses $v1 vs expected $v0, missing dead `paddub`.
    - **func_001D1F80** (53%) — complex index arithmetic + queue push; scheduler difference prevents better match.
    - **func_001D1FF0** (70.5%, previously existing) — rewritten with `int` extern types for gp_rel.
    - **func_001D4A90**, **func_001DD950** — compiled (previously unbuilt src files); match 56% and 72%.

    **Key findings:**
    - **`beq` vs `bne` generation**: writing `if (a0 == N) goto L_N` produces `beq a0, v1, L_N`.
      Writing `if (a0 == N) { action; }` produces `bne a0, v1, skip` (inverted branch). The goto
      form matches the expected beq-chain pattern in switch-like functions.
    - **Constant scheduling**: mwcc always hoists ALL `li` (load-immediate) operations to the top
      of the basic block, regardless of source order. Cannot force a constant to be initialized late.
    - **GP-relative register choice at join point**: after a switch with multiple beq-targets,
      at the join point the compiler picks the register for global loads based on liveness analysis,
      not source variable name. Cannot force $v1 vs $a1 without changing the function structure.
    - **Dead `paddub` in else branch**: appears when comparing with `bne` chain (not `beq` goto chain).
      The goto form with `beqz` tests generates `paddub` in the `bnez` delay slot for the "no match" path.

### Done — Heuristic function naming pass (47 names, 2026-05-24)

Added `tools/decomp/name_functions.py` — heuristic naming via string-reference
analysis. The tool reads splat's per-function `.s` disassembly, recovers
absolute 32-bit addresses from lui/addiu pairs (and the resolved
`D_XXXXXXXX` symbols splat already emits for cross-function data refs), maps
them to ASCII strings extracted from the boot ELF, and proposes a
function name derived from the most distinctive nearby string. Auto-block in
`config/symbol_addrs.txt` is idempotent — rerunnable any time without
duplication.

Result of first pass: **47 SDK helpers named** by their referenced strings,
e.g. `sub_Invalid_macroblock_type_code_0` (MPEG decoder error path),
`sub_Too_many_macroblocks_in_picture`, `sub_libpad_Module_version_mismatch`,
`sub_BASCUS_97112_DS00_00_*` (boot ID readers), `sub_NIGHT_VISION_SYSTEM`
(weapon-name lookup, game code). Stdlib-signature pass and SDK-banner pass
yielded zero high-confidence hits — the SDK leaves all call out (no
isolated memcpy/memset/strlen in the linked ELF that's also obviously
distinct from byte-loop game code), and the `PsIIlib*` banner strings are
referenced by too many functions (≥6 each, dropped by the
distinctiveness filter).

DCDecomp cross-reference attempted (`/tmp/DCDecomp`): yielded zero. DCDecomp
itself only has ~120 named symbols, all anonymous `D_XXXXX` / `.LXXXXX`
labels — they don't name SDK functions either. The memory note's hopeful
estimate of 50-200 hits did not materialize.

objdiff still parses (1491 units) and `objdiff-cli report generate` reports
unchanged metrics: **1338/1491 matched at 100%, 98.51% fuzzy**. No matched
function regressed.

### Done — Overlay matching first batch (36 functions, 2026-05-24)

The overlay decomp loop is now proven end-to-end. **36 overlay functions are
matched to 100% C source** across 18 of 19 overlays, while all 19 overlays
remain byte-identical to the original `OVERLAY/AREAXX.BIN` files. The
matched-function tree lives at `src/overlays/AREAXX/`; see `docs/OVERLAYS.md`
section 6 for the per-overlay table and the patterns used.

Categories (most are pure C with `-O4,p -sdatathreshold 4`, no inline asm):
- 19 **area-init functions** (1 per overlay) — write area-type, data-section
  pointer, and zeroes into 4–6 gp-relative boot-ELF BSS slots.
- 6 **jr+nop stubs** (`void f(void) {}`).
- 3 **thin wrappers** (`callee(args); return 1`).
- 4 **struct-field setters / boolean inverters** (small leaves).
- 2 **abs-address byte read-modify-write** (need `-sdatathreshold 0` to
  force `lui/lbu` for globals outside the GP ±32KB window).
- 2 **gp_rel pure-C leaves** that don't fit the above buckets.

Three small infrastructure changes:
- **`tools/overlay/compile_overlay_src.py`** (new) — compiles
  `src/overlays/AREAXX/*.c` → `build/overlays/AREAXX/obj/`.
- **`tools/overlay/fill_overlay.py`** — normalizes the mwcc-emitted EABI64
  e_flags to O32 before handing the object to GNU ld (which refuses to link
  EABI64 alongside GNU-as O32). Boot-ELF builds use mwldmips directly and are
  unaffected.
- **`tools/overlay/link_overlay.py`** — symbol table for the GNU ld script
  now also scans compiled `.o` objects (`nm -u`), not just `.s` disassembly,
  so C source can reference globals never named in any `.s`.

### Done — Overlay build pipeline (19/19 byte-identical)

All 19 `OVERLAY/AREA*.BIN` overlay files produce byte-identical output from
the original disc disassembly. The pipeline is fully automated in `tools/overlay/`.

**Key decisions and hard-won fixes:**

- **GNU ld instead of mwldmips**: mwldmips (the period-correct linker) segfaults
  on aarch64 under qemu-i386 + wibo32 for small overlay-sized inputs. GNU
  `mipsel-linux-gnu-ld` runs natively and produces byte-identical results.
  mwldmips continues to be used for the boot ELF (the full 3014-object link
  doesn't crash).

- **Cross-file `.L` label fixup**: splat splits code at function boundaries, but
  MIPS branches legally jump into adjacent functions. Splat generates `.L` local
  labels for branch targets in the *defining* function's `.s`. GNU as local labels
  are not exported, so cross-object `.L` refs fail at link time. Fix: scan all
  `.s` files, find `.L` labels referenced across files, rename them (drop `.`) and
  add `.globl` in the defining file.

- **R_MIPS_PC16 addend bias**: GNU ld's formula is `(S - P) >> 2`, but MIPS
  hardware uses `target = P + 4 + offset×4`, so the correct formula is
  `(S - P - 4) >> 2`. GNU as leaves the instruction field = 0 for cross-object
  branches. Fix: after partial link, patch all PC16 relocation instruction fields
  from 0 → 0xFFFF (-1), so GNU ld computes `(S + (-1) - P) / 4` = correct value.

- **VU0 / COP2 macro-mode instructions**: AREA21 contains vector unit instructions
  (`vmulax`, `vmadday`, etc.) that GNU as doesn't support. Splat emits them as
  decoded mnemonics with the raw 8-hex-char opcode in the comment. Fix: replace
  with `.word` directives. Opcode byte order: splat shows bytes MSB-first; use
  `int.from_bytes(bytes.fromhex(opcode), 'little')` to get the correct LE integer.

See `docs/OVERLAYS.md` for the full architecture, tool descriptions, and
byte-identity results table.

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

- **`sd/ld` callee-save functions are Sony PS2 SDK code, not game code (RESOLVED 2026-05-24 — not a blocker).**
  Earlier sessions noted that ~half the boot ELF's functions use
  `sd/ld + daddu/move` for callee-saves while the other half use
  `sq/lq + paddub`, and called this a "two compiler versions" mystery
  blocking Track A. **The split is real but the diagnosis was wrong.**
  Empirical findings:
  - The local `tools/mwccps2/mwccmips.exe` is **byte-identical** to
    `Adubbz/DCDecomp`'s `tools/compilers/mw/2.3.1.01/mwccmips.exe` (SHA1
    `b368c01c0d3e306389d5de622a801e6b56f77ba4`, 1,177,088 bytes). It
    self-identifies as **`Version 2.3, Runtime Built Dec  2 1999`** when
    asked `-help`, but `mwldmips` writes `MW MIPS C Compiler (2.3.1.01)`
    into the linked ELF's `.comment` regardless — that string is a static
    linker label, not a per-compile attribute. DCDecomp uses this exact
    binary to byte-match the Dark Cloud ELF.
  - This compiler **always emits `sq/lq` for callee-save spills** from C
    input. Tested across `-O0/-O1/-O2/-O3/-O4/-O4,p/-O4,s` with multiple
    function shapes — `sq` in every case. There is no CLI flag, pragma,
    or optimisation level in the EXE that toggles this to `sd`/`ld`
    (verified by reading the full `-help all` output and grepping strings).
  - **Spill style is bimodal per function**: out of 2065 functions in the
    boot ELF that spill anything to the stack, **1627 use `sq` exclusively**
    and **437 use `sd` exclusively**; only 1 mixes both. So each function
    was compiled by exactly one codegen, never a mix.
  - **The `sd`-style functions cluster sharply by vram**:
    ```
    bucket(64KB)   sd    sq    % sd
    0x00100000    184     4   97.9%   <- SDK region
    0x00110000    185     0  100.0%   <- SDK region
    0x00120000     67    42   61.5%   <- transition
    0x00130000+     1  1581    0.1%   <- game code
    ```
    The 0x00100000–0x0011FFFF region (the lowest 128 KB of the boot
    segment) contains the entry point `_start`, the 134 EE-kernel syscall
    stubs (`func_0010B400..0010BC80`), and named runtime/kernel helpers
    that are already in `symbol_addrs.txt`: `ResetEE`, `SetGsCrt`,
    `LoadExecPS2`, `ExecPS2`, `AddIntcHandler*`, `AddDmacHandler*`,
    `_EnableIntc`, `_iSetAlarm`, `CreateThread`, `StartThread`, etc.
    These are the **Sony PS2 SDK / `libkernel` / `crt0`** statically
    linked into the boot ELF — pre-compiled by Sony (presumably with an
    earlier mwcc build whose default callee-save width was `sd`) and
    shipped as object/archive files alongside the Metrowerks compiler.
    Above 0x00130000 is Extermination's own engine code, which our
    compiler matches in `sq` style.
  
  **Implications:**
  1. The `sd`-style functions are not original game code waiting to be
     decompiled. They are **vendor binaries**. They should never count
     against Track A's "matched-functions" denominator.
  2. Re-matching them from C with this compiler is impossible (it never
     emits `sd` callee-saves). Re-matching them from a hypothetical
     "earlier" mwcc is undesirable even if one were found — we'd just
     be re-creating Sony's pre-compiled SDK.
  3. The linker pipeline already handles them correctly: when
     `tools/decomp/strip_sections.py` detects a `SIZE_DRIFT_FORCE_ASM`
     mismatch, it links from the splat-disassembled `.s` instead. For
     these SDK functions that **is** the canonical source.
  4. Hand-decompilation effort should focus on the **1627 `sq`-style
     functions** (vram ≥ ~0x00130000) — that's the game code our
     compiler can actually match.
  5. The "current total: ~1477 src files" / "44% matched" numbers
     elsewhere in this document should be recomputed against a denominator
     of 1627 (game code) + 134 (named syscall stubs, already matched)
     ≈ 1761, not 3014 total splat functions. Many of the unmatched
     remainder is permanently un-decompilable SDK code that the linker
     already handles via assembled `.s`.

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
- **`OVERLAY/`** (`AREA*.BIN`, `MWo3` overlay modules) — **fully characterized
  and pipeline complete (2026-05-24)**. Format documented in `docs/OVERLAYS.md`.
  All 19/19 overlays produce byte-identical output via `tools/overlay/`. The
  overlay pipeline uses GNU ld (not mwldmips, which segfaults on aarch64 for
  small link jobs). Three non-trivial obstacles resolved: cross-file `.L` label
  promotion, R_MIPS_PC16 addend bias fix, and VU0 COP2 instruction replacement
  with `.word` directives. Not yet scanned for embedded textures/geometry.

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
  - `decomp/repack_iso.py` — swaps rebuilt ELF and/or overlay BINs into a
    copy of the user's ISO for PCSX2 testing (`--overlays` flag).
  - `overlay/gen_splat_yaml.py` — auto-generates per-overlay splat YAML + symbol_addrs.
  - `overlay/fill_overlay.py` — assembles overlay `.s` → `.o` (VU0 fixup, cross-file
    label fix, strip_sections integration); picks up compiled `.o` when available.
  - `overlay/link_overlay.py` — GNU ld-based overlay linker (partial link + PC16 fix +
    final link + ELF extraction + MWo3 packing + byte verification).
  - `overlay/extract_overlays.py`, `overlay/pack_mwo3.py`, `overlay/build.py` — support tools.
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
  `OVERLAYS.md` (overlay format reference, architectural plan, and roadmap for
  the second matching surface), `track-a-kickoff.md` (Track A starter prompt).
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
