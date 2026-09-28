# HANDOFF — Extermination (PS2) decomp + native port

**Current as of 2026-09-26 (Claude, s87).** This is the short cross-repo entry point.
Below the "MATCHING-WORKFLOW REFERENCE" line is the older byte-matching reference
(compiler, build loop, NEARMISS, idioms, next matching tasks).

Working directory: `/Users/abe/Documents/Extermination.nosync/Extermination` (port:
`../extermination-port`). **Never** the iCloud copy at `~/Documents/Extermination` —
that is a different, stale tree.

## Start here
1. Read both `CLAUDE.md` files (this repo and `../extermination-port`). They hold the
   legal hard rules (never commit or print disc-derived material), the fidelity rules
   ("a label is not evidence"; missing workers fault), the test policy (first-level
   scope, ~10 s default, `EM_TEST_FULL=1` exhaustive, headless) and the cleanup policy.
2. **Goal (user, 2026-09-22/23):** port the whole first snow level (AREA11) completely
   and faithfully, up to Roger, then continue into the next level. Nothing may be
   invented: every behavior is a translation of original code and is verified by an
   original-instruction oracle or an original capture. The decomp's C stays
   byte-identical (`tools/verify_all.py`).
3. **Two port profiles (user, 2026-09-23):** Original (the default, and the only thing
   fidelity work measures: the exact GS framebuffer at 4:3, no smoothing, no CRT) and
   Enhanced (switches over the same logic). See port `docs/PORT_PROFILES.md`.
4. The port roadmap lives in its docs:
   - `docs/FIRST_LEVEL_AUDIT.md`: H-items and WP-0..WP-18 with status.
   - `docs/SCENE_COORDINATOR_DESIGN.md`: S1..S13, lead decisions D2..D5.
   - `docs/ORIGINAL_FRAME_ORDER.md`.
   - `docs/FIRST_LEVEL_ROUTE.md`: the original route to Roger, captured in
     `build/s87/route/` (keep).
   - `docs/EE_FLOAT_MODEL.md`: measured EE/VU0 float rules.

## Verification toolkit
- Original-instruction oracles: port `tools/test_*_reference.py` run the ORIGINAL
  ELF/overlay code in a Python MIPS/VU oracle over captured RAM in
  `build/startup-reference/`. Floats follow port `tools/ee_float_model.py`; the
  recordings are in `build/startup-reference/ee_float/` and the recorders in
  `tools/ee_float/`.
- Original runtime: `tools/pcsx2_session.py` runs PCSX2 hidden, with exact one-frame
  steps, pad input, memory reads and snapshots. Save states are in
  `build/startup-reference/portable-data/sstates/` (01-15 are the user's; never
  overwrite them).
- Port live checks: `EM_STARTUP_TEST=newgame-control` (displacement 9.599989) and the
  level smoke `EM_STARTUP_TEST=newgame-level` + `tools/test_level_smoke.py`. Its phases
  are checked against the route captures.
- Decomp gate: run `tools/decomp/build.py build` fresh, then `tools/verify_all.py`.
  Latest (bdd40fb): 2150/2211 units; boot ELF byte-identical; 19/19 overlays.
- Lanes: each owns disjoint files, builds privately and gets an adversarial review. The
  lead commits after an isolated index build (`git checkout-index` into scratch, then
  `make all`) and a leak scan.

## State (2026-09-26)
- **Chain C6 finished** (port c7a04a4 VOICE, 955f1c2 RCTX, 8d65dff FXLIVE, b7868e1 DOOR,
  de64410 OBJKERNEL, c8e658f SHADOW, 097fbd9 UI, d610cec ROUTE). The level smoke plays the
  whole main route 01..14 (18 phases, incl. the director beats and Roger) plus side beats 00
  and 09, each checked against its PCSX2 capture; `--require-through` makes a shorter route
  fail. Census (port FIRST_LEVEL_CENSUS.md 1.22): live 640 / verified-unbound 87 /
  unverified 5 / stand-in 0 / missing 1 / boundary 451 = 90.8% of non-boundary instructions.
  newgame-control baseline is now 9.599849.
- **What still differs from the original** (C6 limitations; port LEVEL_SMOKE.md "What the
  full route does not yet compare"): the player, Roger and the other non-crate owners still
  draw legacy meshes (only crates, drums, truck and fence door use the original object
  units); the effect chains are built exactly but not drawn; the scripted takeover
  (001CA770, 0015B530/00182B30/001837A0) is a stand-in; step V (001D2300) and the static
  world (001C1D00, bank *D_0028A5A0) are not bound; voiced lines tear down 6-8 rows early
  (no drive-latency model); rand() order is unaudited; duplicate translations remain;
  Metal rasterization stands in for the GS DDA (no framebuffer capture yet).
- **Running:** chain C7 (build/workflows/snow-level-chain-c7-*.js: PLAYERDRAW, FXDRAW,
  OWNERS, TAKEOVER, STEPV, DEDUP, FPUAUDIT, then capture-dependent VOICELAT, DOOR1,
  RNGORDER, and ROUTE) and a capture job (build/workflows/c7-original-captures-*.js:
  stream latency, fence door side 1, lane-3 writer, rand() caller trace, framebuffer
  feasibility, panel module load; outputs build/s87/c7cap/, doc docs/CAPTURES_C7.md).
- **Not yet planned into a chain:** Roger's face units (001CB3C0, 001D3F50, 001D3E40,
  001C7900, 001CB2C0) and his kind-0x29 shadow proxy; the static world and background
  channel (001C1D00, 001D5370, 001E1E60/001E1AD0); UI cues and unit sound (WP-14); SPU2
  reverb; the GS-exact Original-profile renderer and framebuffer harness.
- **Policy and registries (user, 2026-09-27):** the original code is the oracle;
  hardware timing is not reproduced by default (disc at host speed, no slowdown,
  no CRT); the recorded disc-drive timing (C7 VOICELAT) becomes an optional
  switch, off by default (queued). Port docs/FIDELITY_FEATURES.md lists what the
  port reproduces (evidence + status); port docs/LAUNCHER_OPTIONS.md lists every
  launcher option and the decisions the user still has to review (field
  presentation). Future goals: a launcher; a PS2 compile target for the port's
  game code (test it in the ELF under PCSX2).
- **Queued for the next first-level chain** (FIDELITY_FEATURES "blockers" and
  C7 limitations): the drive-timing switch; H7 aligned on load completion;
  wire the load veil; the player-reachable fail-stops (DATABASE/SPR4/MAP pages,
  non-battery item takes); Roger's face units (001CB3C0 ...) and kind-0x29
  shadow; the static world (001C1D00, bank *D_0028A5A0) and background
  channel; the fan spin, husks and indicator draw; flame and snow on the chain
  page; UI/unit sounds and SPU2 reverb; disc-sourced textures (today some come
  from PCSX2 captures); then the GS-exact renderer and the pixel harness.
- **Decision (user, 2026-09-26): do not integrate ran-j/PS2Recomp** (GPL-3.0 static
  recompiler). Evaluated read-only: host-IEEE EE/VU0 floats, no runtime-overlay support,
  non-GS-exact rasterizer, disc-derived output with per-instruction disassembly comments, and
  linking or copying it would make the port GPL. Do not link, vendor, copy or run it, and do
  not read its runtime sources while writing port hardware code (clean-room).
- **Guards:** tools/check_no_disassembly.py in both repos. The asm bodies stay by the
  user's decision. Never stage while a workflow chain's committer is running (the index
  is shared).
- **Level 2 (AREA01) on the side** (user, 2026-09-25; rules in memory
  `level2-side-track`: new files, decomp overlay sources, ignored build/ only, never the
  port's live files while a first-level chain runs):
  - Phase 1 done: original route recorded (`tools/route_capture.py` group `a01`,
    `build/s87/route_a01/`), census delta (`route_census.py a01-delta`: 154 new
    functions, 89 main line), overview (`tools/area_overview.py`), overlay matching
    (bdd40fb: 33 of 41 byte-identical C). Docs: port `docs/SECOND_LEVEL_ROUTE.md`,
    `docs/AREA01_OVERVIEW.md`; FINDINGS "AREA01 route capture" section.
  - Phase 2 wave 1 done (2026-09-26): port e21bd95 has 90 standalone translations of the
    main-line new functions (overlay 14, math 30, render 21, sys 25) and the AREA01 asset
    exports (assets/area01/), each oracle-tested, not bound; decomp 07c4e32/837d548 corrected
    9 NEARMISS bodies the lanes found wrong. Mutation sweeps were closed out on named
    survivors (they did not converge). Next: the 65 side/exit-only new functions.
  - 2026-09-28: AREA01 overlay at 37 of 41 functions linked from C (3 NEARMISS
    jump-table dispatchers: text and tables byte-identical, blocked only on
    tools/overlay/link_overlay.py placing compiled .rodata at link + 0x40; details
    docs/AREA01_OVERLAY_C.md). New AREA01 side beats a01_s4 (east room, save
    terminal), a01_s5 (control-room duct, healing pickup); the north room is
    unreachable on the first visit (its bridge lowers only on a return).
- **Level 3 = AREA00** (AREA01's shaft door exits to AREA00 sub 0 entry 0), 2026-09-28:
  overlay fully linked from C (33 byte-identical + the entry pad; 0 NEARMISS),
  AREA01's too (41/41; the overlay link now pins compiled jump tables,
  tools/overlay/jt_pin.py). The AREA00 route is recorded to its progression exit
  (a00_00..a00_10, port docs/THIRD_LEVEL_ROUTE.md): raised ferry deck, cab roof,
  container stack, crate tops, a one-way duct into the north-east room, its switch
  (D_0081075D), crates blocking door [51], then back through the shaft door, which
  returns to AREA01 with D_0081075E = 0xFF (the AREA01 bridge lowers: the north
  room and AREA02 lie beyond). Game order so far: AREA11 -> AREA01 -> AREA00 ->
  AREA01 (revisit) -> AREA02. Census: AREA00 runs 141 functions the first two
  levels never ran (76,048 bytes: 128 main line, 13 in the arrival back in
  AREA01); the AREA01 room beats add 20 (AREA01 total new: 174).
- **Level 4 = AREA01 revisit + AREA02** (2026-09-28): recorded to AREA02's exit into
  AREA04 (port docs/FOURTH_LEVEL_ROUTE.md: a01r_00..03, a02_00..05, side beats). The
  revisit plays a new ~6,900-frame cinematic (script 0x82AD90), gives item 0x20 and
  spawns six creatures; the lowered bridge leads to door [16] and AREA02 sub 1.
  AREA02: switch [31] starts a rail car (lethal if you stay on the rails), a ladder
  escape, a ledge climb with bug grabs, battery panel [24], door [25] to AREA04.
  Census: 111 functions new (a02_delta.json). AREA02 overlay: 15 of 17 functions
  link from C (008254E0 NEARMISS only because fill_overlay cannot absorb its
  trailing pad piece), docs/AREA02_OVERLAY.md. AREA00 phase 2 committed (port
  037e8b6). Next: AREA02/revisit phase 2 translations, AREA04 (level 5).
  - Port flags for binding: em_actor_cells rejects AREA01's cell directory (uid 0 word bit
    29); em_coll_segment_walkers returns -1 on 0019D770's no-span path (FINDINGS).
  - Phase 3 (after the first level is done): an AREA01 binding chain.
- **Next:** the Original profile (exact 512x448 GS framebuffer, 4:3) and the
  framebuffer-compare harness, EE-float harmonization of the older oracles.

---
MATCHING-WORKFLOW REFERENCE (§1-§13). Pre-s87 text kept as the reference for the
byte-matching loop; counts refreshed 2026-09-27, other details may lag — re-check
against `tools/verify_all.py` and `docs/PROGRESS.md` before relying on them. The
pre-s87 session-log continuations that used to sit here were cut on 2026-09-27
(git history has them).
---

## 1. What this project is, in one paragraph

A matching decompilation of **Extermination** (PS2, 2001), target **SCUS-97112**. "Matching"
means the C we write must compile to **byte-identical** machine code. The original was built
with **Metrowerks CodeWarrior for PS2** (`MW MIPS C Compiler 2.3.1.01` per `.comment`), with
some Sony SDK objects built by **ee-gcc**. The boot ELF is stripped — no DWARF, no symbol
names — so this is blind matching. 2,953 functions total.

**Current standing (2026-09-27; source counts from `src/`, object counts from the latest
fresh gate in `docs/PROGRESS.md` — re-run `tools/decomp/build.py build` +
`tools/verify_all.py` for live numbers):**

| | count | of 2953 |
|---|---|---|
| compiled units (objdiff units; includes inline-assembly wrappers) | **2211** | 74.9% |
| ...of which objdiff 100% | 2150 (97.2% of units; matched_code 98.60%) | |
| `// NEARMISS` (readable C, original assembly linked) | 727 | 24.6% |
| `// INCLUDE_ASM` stubs (no readable C at all) | 15 | 0.5% |

Ordinary compiled C actually linked is a separate, smaller number (about 1516 slots at
the 2149/2210 checkpoint); `tools/decomp/audit_link_provenance.py` gives the current
figure. Two separate goals, do not conflate them: readable semantic C and compiled-C byte
matching. Object matching is not proof of readable C, and a 100% object is not proof of
a linked compiled-C slot.

---

## 2. THE COMPILER — read this section twice

The period-correct compiler is a **32-bit Windows PE** that cannot run natively on the user's
Apple Silicon Mac. The chain is:

```
Apple `container` (Linux VM, arm64)
  └─ qemu-i386            (x86 emulation)
       └─ wibo32          (minimal Win32 PE loader)
            └─ mwccps2.exe (CodeWarrior MIPS compiler)
```

The MIPS **assembler** and **linker** are arm64-native inside the same container; only the
compiler needs qemu+wibo. `objdiff-cli` runs natively on the host.

### 2.1 Start the container service first

```bash
container system start
```

**Known wart, do not be alarmed:** `container images list` fails with
`Error: Plugin 'container-images' not found` — the CLI (0.12.3) and the installed plugins
(`/usr/local/libexec/container/plugins/`, which provides `container-core-images`) disagree on
a plugin name. **`container run` works fine**, and that is all the build uses. Verify with:

```bash
container run --rm exterm-permuter sh -c 'echo CONTAINER_OK'
```

If the whole daemon is down, `verify_all.py` reports `boot-elf FAIL — link produced no verify
line`. That is a stopped daemon, **not** a broken build. Start the service and re-run.

### 2.2 The image

`IMAGE = "exterm-permuter"` (set in `tools/decomp/build.py`). It is a superset of the older
`exterm-toolchain` image: adds i386 libs for ee-gcc plus decomp-permuter dependencies.
Recipe at `docker/Dockerfile`. Every container call is:

```bash
container run --rm -v <REPO_ROOT>:/work -w /work exterm-permuter sh -c '<script>'
```

### 2.3 The actual compile commands

CodeWarrior (the game code, default):
```bash
qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/obj/<name>.o src/<name>.c
```
ee-gcc (Sony SDK functions):
```bash
tools/eegcc/ee-compile.sh src/<name>.c build/obj/<name>.o <FLAGS>
```

### 2.4 Which CodeWarrior builds actually exist

`tools/mwccps2*/` are **user-supplied and gitignored**. Only **three** are installed:

| directive | path | notes |
|---|---|---|
| `mwcc` (default) | `tools/mwccps2/mwccmips.exe` | 2.3, build 991202 |
| `mwcc233` | `tools/mwccps2-233/mwccps2.exe` | 2.3.3 (000906) — **the workhorse**; cracks the idiom-13 delay-slot family |
| `mwcc24` | `tools/mwccps2-24/mwccps2.exe` | 2.4 |

`build.py` also knows `mwcc30` / `mwcc301`, but **those are NOT installed.** Any sweep that
reports them is reporting `n/a`. Do not plan around them.

Matching a function with a *later* CodeWarrior build than 2.3.1 is legitimate — byte-identity
of the loadable region is the only criterion, and `.comment` is not in that region.

### 2.5 Per-file compiler routing

A file selects its own compiler and flags via directives in the **leading comment block**:

```c
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
```

Defaults are `mwcc` and `-O4,p`. `-sdatathreshold` (0 / 4 / 8) matters a lot and is
per-file — getting it wrong silently costs ~0.5%; one function this session measured
99.44 / 99.96 / 99.57 across the three values.

**Historical bug worth knowing:** `file_cflags()` used to stop scanning at the first
non-directive comment line, which silently discarded the declared flags for all 628 near-miss
files (whose first line is the `// NEARMISS` banner). Fixed. If you touch that parser, re-measure.

---

## 3. The build loop

```bash
cd /Users/abe/Documents/Extermination.nosync/Extermination
source .venv/bin/activate          # splat, objdiff bindings, pygltflib live here

python3 tools/decomp/build.py setup   # splat split + asm_fixup + regenerate objdiff.json
python3 tools/decomp/build.py build   # assemble expected/*.o + compile obj/*.o  [container]
python3 tools/verify_all.py           # THE GATE
```

`setup` is only needed when the set of translation units changes (a file flips between
stub/NEARMISS/compiled). `build` alone suffices after editing an existing compiled unit.
A full `setup && build && verify_all` is roughly 4–6 minutes, dominated by the ~130 s link.

`verify_all.py` must print **all six PASS**:

```
boot-elf   PASS  boot ELF byte-identical (0x175b00 loadable bytes)
overlays   PASS  19/19 overlays passed
match      PASS  matched_code 98.60% (floor 95.0), functions 2150/2211
gltf / selftest / gs-offset  PASS
```

**Do not commit unless `boot-elf` says byte-identical.** That is the project's whole point.

### Beware the `matched_code` percentage

`matched_code 98.60%` counts matched over **compiled** units, so near-misses are excluded from
the denominator by construction. Promoting correct functions *lowers* it. It is not the
progress metric. The whole-game object-matching number is **2150 / 2953 = 72.8%**;
`docs/PROGRESS.md` also records the assembly-fallback qualification above.

---

## 4. The `// NEARMISS` mechanism

A `src/<f>.c` whose **first line** starts with `// NEARMISS` is excluded from the
ordinary matched-unit selection. Matching tools can compile and measure it;
the marker alone is not a claim that it never compiles. The linked filler uses
original assembly for these slots. Verify actual link provenance whenever
promoting a candidate; a passing boot ELF alone can hide assembly substitution.

This lets us commit *body-correct readable C* that is not byte-exact — valuable as ground
truth for the native port — without weakening the byte-identity guarantee. Every near-miss
header must record the real re-measured objdiff %, the compiler, and the precise divergence.
Registry: `docs/NEARMISS.md`. Promote one by making it byte-exact and deleting the
marker, so it becomes a normal compiled unit.

---

## 5. THE MOST IMPORTANT LESSON: objdiff 100% is NOT a sound promotion gate

Late in the last session, all 792 near-misses were re-measured through the **canonical**
pipeline (trial-promoted so `build.py`'s `expected` path, including `inject_relocs.py`,
applied — not the scratch harness the sweeps use). Five scored 100.0%. **Only one was real:**

```
KEPT     func_001152D8   boot ELF byte-identical
REVERTED func_0012CAA0   first diff 0x0012cccc  0xc9 vs 0x8e
REVERTED func_00169730   first diff 0x0016987a  0x24 vs 0x25
REVERTED func_0016AE40   first diff 0x0016af4e  0x24 vs 0x25
REVERTED func_001EA240   first diff 0x001ea954  0x80 vs 0xa0
```

**Four false positives out of five**, every one caught only by linking and comparing the ELF.
The failing bytes matched the parked wall descriptions exactly (`0x24` vs `0x25` is the
a0-vs-a1 register colouring; `0x80` vs `0xa0` is the `0x3f80`/`0x41a0` FP-constant ordering).

**Gate every promotion on the boot ELF, bisecting when a batch fails.** Commit `9ac388a` has
the original write-up. Also verify `fill_unmatched.py` actually selects the candidate's
compiled object: GPREL_FORCE_ASM, SIZE_DRIFT_FORCE_ASM, local data, and size drift can
silently replace it with assembly. A passing ELF then says nothing about that C.
This continuation removed a stale force-assembly entry for func_00180850 and kept
func_00135D00 parked on its local jump-table placement blocker. The new source fixes
for func_0012CAA0 and func_001EA240 now pass the actual compiled-C ELF gate.

Corollary: **an agent's reported percentage is not evidence.** Four agent 100%-claims failed
integration across three batches last session. Always re-measure with
`tools/match/integrate_nearmiss.py`, which does its own canonical measurement.

---

## 6. Tooling map

```
tools/decomp/build.py              setup / build / --single-file (objdiff's custom-make hook)
tools/decomp/asm_fixup.py          re-applies VU/endlabel fixups after splat regenerates .s
tools/decomp/inject_relocs.py      relocation injection on the expected-object path
tools/decomp/fill_unmatched.py     linker fill from splat .s; LOCALDATA_FORCED + drift guards
tools/verify_all.py                the six-check gate
tools/match/integrate_nearmiss.py  THE integrator: <wave.json> [MIN%]; re-measures, writes
                                   src/, updates docs/NEARMISS.md. Pass 0 to keep low-% C.
tools/match/clean_registry.py      drops stale/duplicate NEARMISS.md rows after promotions
tools/match/baseline.py            measure named functions through the canonical harness
tools/match/permute.py             drive decomp-permuter (tools/permuter/)
tools/match/*_wave.js              Workflow fan-out scripts (Claude-specific; see §8)
docs/fanout/MATCHING_GUIDE.md      THE compiler-idiom bible — read before any matching work
docs/NEARMISS.md                   near-miss registry (committed)
docs/PROGRESS.md                   standing + roadmap
```

Most `docs/*.md` are committed; a few working references (e.g. `docs/STARTUP.md`,
`docs/OPENING_ACTORS.md`, `docs/fanout/*_GUIDE.md` except MATCHING_GUIDE) are kept
untracked on purpose. Check `git status` and run the no-disassembly guard before staging.

---

## 7. Compiler idioms — read `docs/fanout/MATCHING_GUIDE.md`

31 documented idioms. The latest is **idiom-31**, nonzero integer-to-float staging:
20 staged through an int fixes func_001EA240; four degrees converted/scaled to the
identical float bits fixes func_0012CAA0. Correct callee prototypes also resolved
func_00180850 and the remaining object residual in func_00135D00.

Two useful earlier wins:

**idiom-29 (strength-reduced multiply: fresh vs in-place shift destination).** The original
allocates a **fresh** register for the final shift (`FRESH = src << k; FRESH >>= 15;
src = FRESH + B`); all three installed mwcc builds reuse the dying source register in place.
The fix is a **source spelling**, not a flag: make the multiply and the shift compound
assignments on the *same* variable.

```c
int x = func_00122BB8() >> 16;
x *= K;
x >>= 15;
... = x + B;              /* the + B must be a SEPARATE expression */
```

Folding the multiply into the initialiser (`int x = r * K; x >>= 15;`) does **not** work, nor
does splitting stages across variables. ~50 respellings were tried; do not re-derive them.
This took the whole 7-function family at once (`func_001549C0` went 64.14 → **100**).

**idiom-30.** Hoisting a call out of a float argument reproduces the target's delay-slot `nop`
in the `mtc1`/`mul.s` sequence.

**The generalizable rule from idiom-29:** when a wall has **no matched exemplar anywhere in
the corpus**, it is a *respelling* problem, not a *search* problem. The permuter did not crack
idiom-29 despite it being pure register colouring; a source-spelling sweep did, with corpus
mining supplying the model. Grep the matched corpus before recording any wall.

---

## 8. If you are Codex (or any non-Claude agent): the parallelism story

`tools/match/*_wave.js` are **Claude Code `Workflow` scripts** and will not run for you. They
are still worth reading as specifications — each encodes the prompt, the schema, and the
per-function attack plan that produced last session's results.

The underlying loop is plain and tool-agnostic:

1. Pick candidates (§9).
2. For each: read the splat `.s`, read the current near-miss C, diff against the expected
   object, form a hypothesis about the divergence, respell, re-measure.
3. Sweep the three installed mwcc builds and the `-sdatathreshold` values 0/4/8.
4. Collect results as JSON: `[{"func": ..., "c_source": ..., "wall": ...}, ...]`.
5. `python3 tools/match/integrate_nearmiss.py <that.json>` — it re-measures canonically and
   decides KEEP / NEARMISS / REVERT.
6. `build.py setup && build.py build && verify_all.py`, then commit.

**Run ONE container invocation at a time.** The Apple `container` daemon has wedged under
concurrent `container run` calls in past sessions. Parallelise your *reasoning*, serialise
your *builds*.

---

## 9. Concrete next tasks, highest value first

### (a) Sweep the ≥99% near-miss band — the reliable grind
Current source headers contain 4 functions in 99.9–100%, 14 in 99.5–99.9%, and
27 in 99.0–99.5%, plus func_00135D00 at 100% with the linker-table blocker.
Regenerate rankings from each near-miss header; percentages are object scores.
Historical yield varies. This continuation recovered four verified matches.

Remaining top targets:
```
func_0016AE40  99.98  mwcc233 -O4,p -sdatathreshold 8
func_001F3620  99.92  mwcc233 -O4,p -sdatathreshold 8
func_001796C0  99.91  mwcc233 -O4,p -sdatathreshold 0
func_0010E8A8  99.89  eegcc   -O2
func_001662D0  99.83  mwcc233 -O4,p -sdatathreshold 8
```

Do not repeat the just-tested modulo compound-assignment variants on func_001F3620
or float-local/union variants on func_001796C0: no new match. For func_0016AE40,
the missing actor argument after its pad-mask test is a real source concern, but
explicitly forwarding it adds a reload; same-TU leaf experiments did not solve it.
See docs/FINDINGS.md. Auditing the 212 stale/intentional explicit fallback entries
among objdiff-100% units is another useful lane; trial each through the ELF gate.

**Skip `func_00169730` and `func_002134C0`** — both are genuine walls confirmed by ~31.5k and
~25k permuter iterations, and both were re-confirmed by the ELF test in §5. Do not re-grind them.

### (b) Family hunting — the step-change lever
idiom-29 produced seven improvements from one insight. Look for other idiom classes where
**every** user is a near-miss and **no** matched exemplar exists. That signature is the tell.
Mine `docs/NEARMISS.md` for repeated wall descriptions, then check the matched corpus.

### (c) The `lui`+`sw` scratchpad symbolization gap — diagnosed, NOT yet fixed
splat symbolizes `lui`+`addiu` pairs for `0x7000xxxx` scratchpad addresses but leaves
`lui`+`sw` pairs as raw constants, so the expected object cannot carry the relocation and
`func_001BF6B0` shows a permanent 4-row residual that is **not** a compiler wall — its linked
bytes are already correct.

**Caution, this has bitten twice.** A direct attempt to symbolize those pairs measured
99.9577 → **99.7531**, i.e. worse. The negative result is recorded in `build.py` beside
`_SPAD_SYMS`. Read that note before trying again, and measure the control through the same
harness.

### (d) The 15 remaining `INCLUDE_ASM` stubs — mostly not real
- **8** (`func_0025F0D4`…`func_0025F144`) are **data, not code** — a 36-byte-stride table whose
  words happen to decode as `bltzal`/`bgezal`, with zero references anywhere. splat false
  positives. Suppressing them means editing the segment config; not worth risking byte-identity
  for a counter.
- **`sub_D2_TADR_08x`** is an alias of the already-decompiled `0x00100A60`.
- **`func_001000B0`** is a genuine 2-instruction syscall thunk (it loads syscall number 0x23 and traps).
- **5** (`func_001CE860`, `anim_eval_skeleton`, `func_001F0A60`, `func_001F6FB0`,
  `sub__0000000000000000Inf`) were decoded to readable C last session, but that C **failed to
  compile** and the wave output has since been lost to temp cleanup. **These need re-deriving.**
  They are large VU0/COP2 functions; the C will never byte-match (see §10) — the goal is
  compiling, body-correct, documented C.

So exactly **one** genuinely undecoded function remains, and it is a kernel-call stub.

---

## 10. What can never byte-match, and why

VU0 macro mode (COP2), MMI 128-bit SIMD, and COP0 have **no C spelling under mwcc 2.3**. Those
functions are permanently near-misses; their C is a faithful scalar equivalent with the
divergence documented per file. Do not burn cycles trying to match them.

Related: PS2 floats are **not IEEE-754** (flush-to-zero, saturation, mostly truncating
rounding; division rounds to nearest — measured rules in port `docs/EE_FLOAT_MODEL.md`).
Note the divergence; don't emulate it in the decomp.

One classification trap that cost a lot of time: **the EE byte-add of a register with zero is
mwcc's REGISTER MOVE idiom, not SIMD.** A classifier that reads it as SIMD will invent a "structural ceiling"
that does not exist. Stripping move idioms first collapsed "genuine SIMD" from 30 functions to 1.

---

## 11. Process rules that were learned the expensive way

1. **A label is not evidence — read the definition.** splat marks compiler output
   "handwritten"; thunk headers describe sound cues as "allocations"; twelve callers declare a
   no-argument function with arguments. Every significant win last session came from checking
   the definition against its description.
2. **Measure the control through the same harness before attributing anything.** One
   conversion read as "only 4 of 47 reached 100%" — a failure — until the baseline run showed 3
   of those 4 already matched *unconverted* and 12 others had improved. Same numbers, opposite
   conclusion.
3. **Validate a detector against a known positive before generalising.** A detector that finds
   nothing looks exactly like a clean codebase. One returned zero twice on the very function it
   was written for.
4. **Never size a class by raw pattern count** (20,019 bare `lui`s; 11 real sites).
5. **Run audits at least twice** — round 2 has caught regressions round 1 introduced.
6. **A wrong "fix" is worse than an honest unknown.** Mark port-side choices explicitly rather
   than dressing them as decoded.
7. **Watch for silent no-ops in your own tooling.** `integrate_nearmiss.py`'s REVERT path used
   to fail silently, leaving non-compiling C on disk — invisible downstream, because a
   near-miss is never compiled, so every check including the boot ELF still passed. It now
   verifies its own post-condition. Assume your other scripts have the same class of bug.

---

## 12. There is also a port repo

`/Users/abe/Documents/Extermination.nosync/extermination-port` — a native macOS/Windows/Linux
port (Cocoa+Metal / Win32+D3D12 / X11+Vulkan), clean-room, **zero third-party dependencies**,
no code from any emulator. It consumes this repo's readable C as ground truth.

**Current user priority:** continue PS2 byte matching and readable native C together,
prioritizing a faithful first-level opening and player interaction. The later explicit
checkpoint expands the earlier PS2-only selection. Do not trust existing port behavior
or decomp comments without checking original instructions/runtime evidence.

If the port: `CLAUDE.md` there has a "Verified backlog" section with per-item blockers, and
`tools/xref_decomp.py` joins port annotations to decomp status.


## 13. Latest integration safeguards (2026-09-22 UTC)

Commit d80e74d hardens integrate_nearmiss.py: remove old outputs before compiling,
require command success and fresh objects, reject duplicate function entries,
restore exact original source bytes (including NEARMISS) on build/validation
failure or interruption, and accept the documented direct-list JSON input.
Run `.venv/bin/python3 tools/match/test_integrate_nearmiss.py` (14 synthetic tests).
GNU as pads reference .text sections: known nonzero compiled sizes may be smaller
than the reference section. Exact section-size equality wrongly rejected four real
matches; oversize remains rejected. The linked-ELF/provenance gate is still required.
