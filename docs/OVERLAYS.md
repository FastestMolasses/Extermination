# Extermination — Runtime Overlay Matching Plan

_Created: 2026-05-24. Pipeline completed 2026-05-24: all 19/19 overlays produce byte-identical output._

The disc ships 19 `OVERLAY/AREA*.BIN` files that are the **second matching surface**
of this project, alongside the boot ELF `SCUS_971.12`. This document records
everything learned during investigation and pipeline development.

**Status: the overlay build pipeline is complete.** All 19 overlays produce byte-identical
output via the GNU ld pipeline described in section 4. The scaffold tools in
`tools/overlay/` are committed and functional.

---

## 1. Enumeration

**19 overlay files** under `OVERLAY/` on disc. Files are gap-numbered (AREA05,
AREA09, AREA10, AREA12 are absent — those level slots either were cut or were
merged into adjacent areas during shipping). All files date to **2001-06-28**
(pre-master date).

| File | Size (bytes) | Overlay ID | Text size | Data size | BSS size | End vram |
|---|---|---|---|---|---|---|
| AREA00.BIN | 40,832 | 1 | 0x3A40 | 0x6500 | 0x295900 | 0x0082D480 |
| AREA01.BIN | 38,912 | 2 | 0x54C0 | 0x4300 | 0x14AC80 | 0x0082CD00 |
| AREA02.BIN | 23,936 | 3 | 0x2140 | 0x3C00 | 0x281800 | 0x00829280 |
| AREA03.BIN | 23,552 | 4 | 0x33C0 | 0x2800 | 0x140C00 | 0x00829100 |
| AREA04.BIN | 37,888 | 5 | 0x30C0 | 0x6300 | 0x14AC80 | 0x0082C900 |
| AREA06.BIN | 23,040 | 6 | 0x2AC0 | 0x2F00 | 0x14AC80 | 0x00828F00 |
| AREA07.BIN | 19,328 | 7 | 0x2640 | 0x2500 | 0x140C00 | 0x00828080 |
| AREA08.BIN | 18,944 | 8 | 0x1BC0 | 0x2E00 | 0x140C00 | 0x00827F00 |
| AREA11.BIN | 30,720 | 9 | 0x4C40 | 0x2B80 | 0x140C00 | 0x0082AD00 |
| AREA13.BIN | 44,416 | 10 | 0x67C0 | 0x4580 | 0x140C00 | 0x0082E280 |
| AREA14.BIN | 20,352 | 11 | 0x2C40 | 0x2300 | 0x140C00 | 0x00828480 |
| AREA15.BIN | 27,136 | 12 | 0x3440 | 0x3580 | 0x140C00 | 0x00829F00 |
| AREA16.BIN | 36,864 | 13 | 0x4F40 | 0x4080 | 0x140C00 | 0x0082C500 |
| AREA17.BIN | 22,912 | 14 | 0x2D40 | 0x2C00 | 0x140C80 | 0x00828E80 |
| AREA18.BIN | 2,176 | 15 | 0x00C0 | 0x0780 | 0x140C00 | 0x00823D80 |
| AREA19.BIN | 50,048 | 16 | 0x68C0 | 0x5A80 | 0x15ED80 | 0x0082F880 |
| AREA20.BIN | 20,352 | 17 | 0x2BC0 | 0x1E00 | 0x140C00 | 0x00827F00 |
| AREA21.BIN | 42,880 | 18 | 0x6E40 | 0x3900 | 0x140C00 | 0x0082DC80 |
| AREA22.BIN | 2,304 | 19 | 0x00C0 | 0x0800 | 0x140C00 | 0x00823E00 |

Total code on disc: ~261 KB text + ~250 KB data across all 19 overlays.
Estimated function count: ~50 (leaf) to ~400 (AREA21) per overlay, with the
two stub overlays (AREA18, AREA22) having exactly **one function** each.

---

## 2. MWo3 Format Reference

Every file begins with a **64-byte (0x40-byte) header** — the DCDecomp
`overlay.hexpat` file documents the same structure:

```
Offset  Size  Field
------  ----  -----
0x00    3     magic = 'MWo'
0x03    1     version (= '3', i.e. 0x33 as char; overlay_id stored separately)
0x04    4     overlay_id  (u32 LE): 1..19, matches the PT_LOAD slot
0x08    4     load_address (u32 LE): always 0x00823500 (the arena base)
0x0C    4     text_size (u32 LE): bytes of executable code in the file
0x10    4     data_size (u32 LE): bytes of initialized data in the file
0x14    4     bss_size (u32 LE): bytes of zero-initialized BSS (NOT in file)
0x18    4     static_init_address (u32 LE): vram of C++ static ctor list start
0x1C    4     static_init_end_address (u32 LE): vram of C++ static ctor list end
0x20    32    overlay_name: NUL-terminated ASCII name, e.g. "Area07.bin"
(rest)  0     (not present; header is exactly 0x40 bytes)
```

**File layout** (confirmed exact for all 19 files):

```
[0x000..0x040)  header (0x40 bytes)
[0x040..0x040+text_size)  .text section (code, loads to vram load_address)
[0x040+text_size..EOF)    .data section (initialized data)
```

`filesize = 0x40 + text_size + data_size` — exact for every file, verified.

`.bss` is **not stored** in the file; the boot ELF's overlay loader zeros it
at load time. The `PT_LOAD` memsz for each overlay slot confirms this:
`PT_LOAD[id].memsz = text_size + data_size + bss_size + 0x40 (header)`.

### Text-section layout

The first **0x40 bytes** of the text section (vram 0x823500–0x82353F) are
**always zero**. This is a nop sled / reserved vector-table area. It is 16
words of `nop (0x00000000)`. The actual first function of each overlay begins
at **vram 0x823540** (offset +0x40 within the text section).

AREA18 and AREA22 (the two stub overlays) contain exactly one tiny function
at 0x823540 — a 7-instruction leaf that writes a constant (0x20 = area index),
a data-section base pointer, and zeroes into three `$gp`-relative slots:

```mips
0x00823540:  addiu  $v1, $zero, 0x20      # area type constant
0x00823544:  sw     $v1, -30536($gp)      # store to GP-relative slot
0x00823548:  lui    $v1, 0x0082
0x0082354c:  addiu  $v1, $v1, <data_base> # pointer to data section start
0x00823550:  sw     $zero, -30532($gp)
0x00823554:  sw     $zero, -30540($gp)
0x00823558:  jr     $ra
0x0082355c:  sw     $v1, -30548($gp)      # delay slot store
```

This is the overlay's minimal init function — it registers itself with the
boot ELF's area-manager subsystem. Real overlays (AREA07 etc.) have dozens
more functions beyond this.

---

## 3. Boot ELF — Overlay Arena and Dispatch

### ELF program headers

The boot ELF (`SCUS_971.12`) has 22 PT_LOAD segments:
- `PT_LOAD[0]`: the boot code (vaddr 0x00100000, filesz 0x175B00, memsz 0x723500).
- `PT_LOAD[1..19]`: **19 zero-filesz slots**, all at vaddr 0x00823500, with
  `memsz = overlay_id_N.text_size + data_size + bss_size + 0x40`. These declare
  the overlay arena's maximum extent for each slot.
- `PT_LOAD[20]`: vaddr 0x00AC2D80, memsz 0 — scratch/heap region marker.
- `PT_LOAD[21]`: vaddr 0x70000000 — scratchpad SPR marker.

The PT_LOAD slots match the overlay files 1:1 in `overlay_id` order:
PT_LOAD[1] ↔ AREA00 (id=1), ..., PT_LOAD[19] ↔ AREA22 (id=19).

### Filename table

A 21-entry filename string table lives in the boot ELF's BSS/data at **file
offset 0x1732A0** (vram **0x00272FA0**). Each entry is 32 bytes:
- Entries 0–1: `\DATA\INDEX.IDX;1` and `\DATA\DATA.DAT;1` (the main archive).
- Entries 2–20: `\OVERLAY\AREA00.BIN;1` through `\OVERLAY\AREA22.BIN;1`
  (exactly 19 overlay filenames, in the shipped order).

These are CD-ROM ISO 9660 path strings (backslash separators, `;1` version suffix).
The boot ELF reads the correct entry by overlay_id to open the file.

### Dispatch function

**`func_001E7780`** (size 0x4D4, at vram 0x001E7780) is the main area-state
dispatcher. It reads a two-byte area/state identifier from two consecutive BSS
bytes at `D_00810700`/`D_00810701`, combines them, and dispatches to the
matching overlay function via a 40+ arm `beq` chain. Every branch terminates
with a hardcoded `jal func_8XXXXXX` into the overlay vram space.

### Boot ELF → overlay call sites (all 17 unique targets)

All calls into the overlay come from the dispatch function (and one outlier at
0x195D18 in `func_00195130`):

| Overlay vram target | Offset from 0x823500 | Callers |
|---|---|---|
| 0x00823580 | +0x0080 | 6 |
| 0x008236E0 | +0x01E0 | 1 |
| 0x00823740 | +0x0240 | 1 |
| 0x008237C0 | +0x02C0 | 1 |
| 0x00823900 | +0x0400 | 3 |
| 0x00823970 | +0x0470 | 5 |
| 0x00823990 | +0x0490 | 1 |
| 0x00823A50 | +0x0550 | 2 |
| 0x00823B10 | +0x0610 | 2 |
| 0x00823B80 | +0x0680 | 1 |
| 0x00823FE0 | +0x0AE0 | 1 (from func_00195130) |
| 0x008241F0 | +0x0CF0 | 7 |
| 0x00824290 | +0x0D90 | 2 |
| 0x00824420 | +0x0F20 | 2 |
| 0x00824E00 | +0x1900 | 3 |
| 0x008250C0 | +0x1BC0 | 2 |
| 0x00826180 | +0x2C80 | 1 |

**Key architectural constraint**: the boot ELF calls overlay functions at
**fixed absolute vram addresses**. Every overlay that supports a given area
state must have its function at the exact same vram offset. This means
**the function layout within each overlay is constrained by the interface** —
the overlay must provide its "init" function at +0x0080 (0x823580), the "main
update" function at +0x0CF0 (0x8241F0), etc.

---

## 4. Build Pipeline (Implemented)

The overlay build pipeline is fully implemented in `tools/overlay/`. All 19 overlays
produce byte-identical output from the original `.s` disassembly. The same pipeline
will swap in compiled `.c` objects as functions are matched.

### 4.1 Tools

| Script | Purpose |
|---|---|
| `tools/overlay/gen_splat_yaml.py` | Reads MWo3 header, writes per-overlay splat YAML + `AREAXX_symbol_addrs.txt` |
| `tools/overlay/fill_overlay.py` | Assembles splat `.s` → `.o` (with pre-assembly fixups); copies compiled `.o` when available |
| `tools/overlay/link_overlay.py` | Links with GNU ld, extracts `.text`+`.data`, prepends MWo3 header, verifies byte-identity |
| `tools/overlay/extract_overlays.py` | Extracts `.text`+`.data`+header from all original BIN files |
| `tools/overlay/pack_mwo3.py` | Packs raw code+data with MWo3 header into a final `.BIN` |
| `tools/overlay/build.py` | Batch pipeline: gen_splat_yaml → splat → fill → link for all 19 overlays |

The configs live in `config/overlays/AREAXX.yaml` (splat) and
`config/overlays/AREAXX_symbol_addrs.txt` (symbol seeds), both committed.

### 4.2 Pipeline steps

**For each overlay:**

1. **`gen_splat_yaml.py AREAXX`** — reads `extract/OVERLAY/AREAXX.BIN`'s MWo3
   header, writes `config/overlays/AREAXX.yaml` (with correct `vram`, `text_size`,
   `data_size`, `bss_size` subsegment layout) and `AREAXX_symbol_addrs.txt`
   (single entry-point symbol — required by splat 0.40).

2. **`splat split config/overlays/AREAXX.yaml`** — disassembles text to
   `build/overlays/AREAXX/asm/matchings/AREAXX/code/*.s` and data to
   `build/overlays/AREAXX/asm/data/*.s`.  Symbol prefix:
   `overlay_AREAXX_$VRAM` (e.g. `overlay_AREA07_func_00823580`) to avoid
   vram collisions between overlays (all load at 0x823500).

3. **`fill_overlay.py AREAXX`** — pre-assembly fixups on the `.s` files, then
   parallel assembly to `build/overlays/AREAXX/filler/*.o`:
   - **VU0 fixup** (`fix_vu0_instructions`): replaces COP2 vector mnemonics
     (`vmulax`, `vmadday`, etc.) with `.word` directives. Splat's comment
     contains the raw 8-hex-char opcode; the LE integer is
     `int.from_bytes(bytes.fromhex(opcode), 'little')`.
   - **Cross-file label fixup** (`fix_cross_file_local_labels`): finds `.L`
     labels defined in one `.s` but referenced in another; renames them to
     non-local (drops the `.`) and adds `.globl` in the defining file.
     (Splat splits at function boundaries, but MIPS branches legally jump
     into adjacent functions — the `.L` targets would be unresolvable
     across object files without this.)
   - Assembled with `mipsel-linux-gnu-as -march=r5900 -L` (`-L` keeps `.L`
     labels in symbol table for the partial-link step).
   - `strip_sections.py --expected-size SLOT` strips GNU-as-only sections,
     fixes `.text` alignment (16→4 bytes), pre-applies GPREL16 relocations,
     and pads/trims `.text` to the computed slot size.

4. **`link_overlay.py AREAXX`**:
   a. **Partial relocatable link** — `mipsel-linux-gnu-ld -r` merges all
      code objects into `_code_merged.o`.  Resolves cross-object `.L`
      branch references.
   b. **R_MIPS_PC16 addend fix** — scans `_code_merged.o` for PC16
      relocations with instruction field = 0 (GNU as leaves these zero for
      cross-object branches).  Patches to 0xFFFF (-1) so GNU ld computes
      `(S + (-1) - P) / 4 = (S - P - 4) / 4` — the correct MIPS branch
      target formula (hardware: `target = P + 4 + offset×4`).
   c. **Final link** — `mipsel-linux-gnu-ld` with the generated GNU ld
      script (`config/overlays/AREAXX.lds`).  Script uses `INPUT()` with
      absolute paths, absolute symbol definitions for all boot-ELF symbols
      referenced from the overlay, and separate `.text` / `.data` / `.bss
      (NOLOAD)` sections.
   d. **Extract** — reads section headers from the output ELF to locate
      `.text` and `.data` at their exact file offsets; extracts exactly
      `text_size + data_size` bytes.
   e. **Pack** — prepends the 64-byte MWo3 header (copied verbatim from
      the original BIN).
   f. **Verify** — byte-compares against the original.

**Why GNU ld instead of mwldmips?**
mwldmips (the period-correct Metrowerks linker) segfaults on aarch64 when
linking small overlay-sized inputs (1–~100 objects) under qemu-i386 + wibo32.
The full boot-ELF link (3014 objects) works fine. GNU `mipsel-linux-gnu-ld`
runs natively on aarch64 and produces byte-identical output: overlay objects
have no GPREL16 relocations (pre-applied by strip_sections.py), no
CodeWarrior-specific section types, and no linker-transformed instructions.
The byte-identical verification confirms correctness.

### 4.3 Compiler flags for overlay decompilation

Overlays were compiled with the same `mwccps2 2.3.1.01`. Expected flags:
- `-O4,p` (same as game code in the boot ELF; delay slots filled)
- `-sdatathreshold 0` (overlay-local globals are at 0x823500+, outside
  GP±32KB, so all data uses `lui`/`lw`; boot-ELF globals at 0x27XXXX
  may use gp_rel depending on threshold)
- `$gp = 0x0027D370` (shared with boot ELF — overlays run in the same
  address space)

### 4.4 Byte-identity results

All 19 overlays verified byte-identical as of 2026-05-24:

```
AREA00 PASS   AREA01 PASS   AREA02 PASS   AREA03 PASS   AREA04 PASS
AREA06 PASS   AREA07 PASS   AREA08 PASS   AREA11 PASS   AREA13 PASS
AREA14 PASS   AREA15 PASS   AREA16 PASS   AREA17 PASS   AREA18 PASS
AREA19 PASS   AREA20 PASS   AREA21 PASS   AREA22 PASS
=== 19/19 pass, 0 fail ===
```

AREA21 (the most complex, 61 functions with VU0 COP2 instructions) required
the VU0 fixup. Several overlays required the cross-file label fixup and PC16
addend fix. All are now handled automatically by the pipeline.

### 4.5 Repacking into the ISO

`tools/decomp/repack_iso.py --overlays` swaps all rebuilt
`build/overlays/AREAXX/AREAXX.BIN` files into a copy of the user's ISO for
testing in PCSX2. Each rebuilt BIN must be exactly the original slot size
(text+data bytes match — BSS is not stored). The `--overlay-dir` argument
defaults to `build/overlays/`.

### 4.6 objdiff integration (future)

When C decompilations are added to `src/overlays/AREAXX/`, `fill_overlay.py`
automatically picks up compiled `.o` from `build/overlays/AREAXX/obj/` in
preference to assembled `.s` filler. For objdiff, add per-function units:

```json
{
  "name": "overlay/AREA07/overlay_AREA07_func_00823580",
  "target_path": "build/overlays/AREA07/filler/overlay_AREA07_func_00823580.o",
  "base_path":   "build/overlays/AREA07/obj/overlay_AREA07_func_00823580.o",
  "metadata": { "complete": null }
}
```

---

## 5. Overlay Interface Contract

The boot ELF calls 17 fixed vram addresses in overlay space. **Each overlay
that a given area state dispatches to must provide exactly those functions at
those addresses.** The addresses are:

```
0x823540  overlay init   (every overlay — stores area index + data ptr into gp-rel slots)
0x823580  area_init      (most areas)
0x8236E0  area_func_01E0 (some areas)
0x823740  area_func_0240 (some areas)
0x8237C0  area_func_02C0 (some areas)
0x823900  area_update_A  (most areas)
0x823970  area_update_B  (most areas)
0x823990  area_func_0490 (some areas)
0x823A50  area_func_0550 (some areas)
0x823B10  area_func_0610 (some areas)
0x823B80  area_func_0680 (some areas)
0x823FE0  area_func_0AE0 (AREA00 specific — func_00195130 calls this)
0x8241F0  area_tick      (most areas, 7 callers — probably the per-frame update)
0x824290  area_func_0D90 (some areas)
0x824420  area_func_0F20 (some areas)
0x824E00  area_func_1900 (some areas)
0x8250C0  area_func_1BC0 (some areas)
0x826180  area_func_2C80 (only AREA19 — largest overlay)
```

Not all overlays implement all functions; smaller overlays have `nop` / `jr $ra`
stubs at unused addresses. The consistent layout across overlays means symbol
recovery from one overlay transfers easily to others.

---

## 6. Overlay matching — first batch (2026-05-24)

**36 overlay functions matched at 100%** across 18 of 19 overlays. All 19
overlays still produce byte-identical `.BIN` output with C-compiled `.o` swapped
in for the splat-disassembled `.s`. See `src/overlays/AREAXX/` for the source.

### Matches by overlay

| Overlay | C funcs matched | Notes |
|---|---|---|
| AREA00 | 3 | init (6 gp-rel stores) + struct field setter + jr+nop stub |
| AREA01 | 1 | init (6 gp-rel stores) |
| AREA02 | 1 | init (5 gp-rel stores, "extended" variant) |
| AREA03 | 3 | init + a2[2]→gp setter + !! boolean inverter |
| AREA04 | 1 | init (5 stores, 2 pointers) |
| AREA06 | 1 | init (5 stores) |
| AREA07 | 2 | init + jr+nop stub |
| AREA08 | 1 | init |
| AREA11 | 3 | init + two thin wrappers (`func(); return 1`) |
| AREA13 | 4 | init + jr+nop stub + short[0x17]=0xFF setter + abs-addr byte increment |
| AREA14 | 2 | init + thin wrapper (`func(0); return 1`) |
| AREA15 | 1 | init |
| AREA16 | 2 | init + struct field a0[0xB8] = -1 |
| AREA17 | 1 | init |
| AREA18 | 1 | init (stub overlay — only function) |
| AREA19 | 4 | init + jr+nop stub + short[0x14]=1 + abs-addr byte setter |
| AREA20 | 1 | init |
| AREA21 | 3 | init + two jr+nop stubs |
| AREA22 | 1 | init (stub overlay — only function) |

### Decomp patterns used

All matches use pure C compiled with `mwccmips.exe -O4,p -sdatathreshold N`.

1. **Area init** (every overlay): writes the area-type constant `0x20`, a
   pointer to the overlay's data section, and `0`s into 4–6 gp-relative slots
   in boot ELF BSS (`D_00275C18`/`C1C`/`C20`/`C24`/`C28`/`C2C`). Pure C with
   `-sdatathreshold 4` (gp_rel for int globals). Three variants: 4-slot (most
   overlays), 5-slot (AREA02/04/06/19), 6-slot (AREA00/01).

2. **jr+nop stubs**: `void func(void) {}` — empty C function. mwcc emits
   `jr $ra; nop` exactly.

3. **Thin wrappers**: `funcN(args); return 1;` — pure C. Matches when the
   callee args fit naturally in the calling convention (mwcc 2.3 schedules
   the arg setup before the `jal` and fills the delay slot with `nop` or a
   safe hoistable instruction).

4. **Struct-field setters**: `a0[N] = K; return 1;` — straight sw/sh at
   offset `N*sizeof(elem)`, return constant.

5. **GP-rel `int` reads/writes**: pure C with `-sdatathreshold 4`. mwcc
   generates `R_MIPS_GPREL16` for `int` globals (4 ≤ threshold).

6. **Absolute hi/lo addresses** (e.g. `D_008107F4`, outside gp ±32KB range):
   `-sdatathreshold 0` forces mwcc to use `lui/lbu` (R_MIPS_HI16/LO16) via
   `$at` instead of gp_rel. Required for any global outside the ~64KB GP
   window around 0x27D370.

### Infrastructure additions

- **`tools/overlay/compile_overlay_src.py`** — new script. For each `src/overlays/AREAXX/*.c`,
  compiles via `mwccmips.exe` (under qemu-i386 wibo32) into
  `build/overlays/AREAXX/obj/<stem>.o`. Honours `// CFLAGS:` comment on the
  first non-blank line (same convention as `tools/decomp/build.py`). Default
  flags: `-O4,p -sdatathreshold 4`. Run inside the `exterm-toolchain` container:
  `python3 tools/overlay/compile_overlay_src.py [AREAXX | --all]`.

- **`tools/overlay/fill_overlay.py`** — added `_normalize_mwcc_abi()` step
  that runs immediately after a compiled `.o` is copied from
  `build/overlays/AREAXX/obj/` into `filler/`. mwccmips emits
  `e_flags=0x20924001` (EABI64), but GNU ld refuses to link EABI64 alongside
  GNU-as's O32 (`0x20921101`). The normalizer rewrites the EABI64 bits to
  O32+32bitmode in-place. Bytes and relocs are unchanged. The boot-ELF build
  uses mwldmips directly and is unaffected.

- **`tools/overlay/link_overlay.py`** — `_build_abs_syms_lds()` now also
  scans symbols from `build/overlays/AREAXX/obj/*.o` via `mipsel-linux-gnu-nm -u`,
  not just the splat `.s` files. C decompilations can reference globals
  (like `D_008107F4`) that are never named in any `.s` — those symbols are
  now resolved from their address-encoded names automatically.

### Overlay matching workflow (now validated)

1. Add C source to `src/overlays/AREAXX/<funcname>.c`.
2. Inside container: `python3 tools/overlay/compile_overlay_src.py AREAXX`
   → produces `build/overlays/AREAXX/obj/<funcname>.o`.
3. `rm build/overlays/AREAXX/filler/<funcname>.o`  *(or rm all to force re-fill)*.
4. `python3 tools/overlay/link_overlay.py AREAXX` — re-fills (auto picks up
   compiled `.o`), partial-links, applies PC16 fix, final-links, extracts
   text+data, prepends MWo3 header, verifies byte-identity against
   `extract/OVERLAY/AREAXX.BIN`.
5. If `[verify] PASS` — the function is matched. If `FAIL`, delete the `.c`
   and `obj/.o` to revert.

### Next overlay matching targets

Remaining low-hanging:
- Area-init variants in AREA13/AREA16/AREA11 (multiple inits per overlay).
- Larger gp_rel struct accesses and conditional setters.
- Branch-before-call wrappers (apply the boot-ELF "branch before jal" pattern).
- AREA21 has 61 functions including VU0 code — the biggest decomp target.

### Bulk asm-void batch (2026-05-24, session +1)

**+50 additional overlay functions matched** via the hybrid asm-void technique
ported from the boot ELF (`/tmp/gen_hybrid.py`). New generator:
`tools/overlay/gen_asm_void.py` walks each overlay's per-function `.s`, applies
the boot-ELF skip filter (no `%hi/%lo/%gp_rel`, no `jalr`/`syscall`/`j SYM`,
3–300 insns), generates `.word`-encoded branches with named `jal` callees, and
verifies per-candidate by raw-byte + relocation comparison of the
mwcc-compiled `.o` against the GNU-as-assembled reference `.o`. Candidates that
define a label referenced by another file in the same overlay are skipped
(the label would vanish from the symbol table once the splat `.s` is replaced).

Per-overlay delta: AREA00 +2, AREA01 +9, AREA02 +4, AREA03 +3, AREA04 +0,
AREA06 +1, AREA07 +1, AREA08 +5, AREA11 +1, AREA13 +6, AREA14 +1, AREA15 +3,
AREA16 +4, AREA17 +0, AREA18 +0, AREA19 +3, AREA20 +1, AREA21 +6, AREA22 +0.

All 19/19 overlays remain byte-identical (`tools/overlay/build.py --all
--no-extract --no-yaml --no-splat`). New total: 86 overlay functions at 100%.

Infrastructure addition: the generator now removes stale `build/overlays/AREAXX/
obj/*.o` files whose corresponding `.c` source no longer exists, since
`fill_overlay.py` prefers `obj/*.o` over the splat fallback and a stale `.o`
from a dropped candidate would silently keep using mwcc output that no longer
reflects what's in `src/`.

---

## 7. Tools implemented

All tools in `tools/overlay/` are committed originals (no disc content).

### `tools/overlay/gen_splat_yaml.py`
Reads a MWo3 header and generates the per-overlay splat YAML config with
correct `bss_size`, `vram`, and data-as-subsegment structure. Also writes
the per-overlay `AREAXX_symbol_addrs.txt` (required by splat 0.40).

### `tools/overlay/fill_overlay.py`
Assembles splat `.s` → filler `.o` with pre-assembly fixups:
- VU0 / COP2 instruction replacement with `.word` directives
- Cross-file `.L` label promotion to non-local + `.globl`
Copies compiled `.o` from `build/overlays/AREAXX/obj/` when available.

### `tools/overlay/link_overlay.py`
GNU ld-based linker (replaces mwldmips for overlays):
- Generates `config/overlays/AREAXX.lds`
- Partial relocatable link → R_MIPS_PC16 addend fix → final link
- Extracts `.text`+`.data` from ELF section headers
- Prepends MWo3 header → byte-identity verification

### `tools/overlay/extract_overlays.py`
Extracts text, data, and header fields from all original BIN files for
inspection. Disc-derived output — not committed.

### `tools/overlay/pack_mwo3.py`
Packs raw `.text` + `.data` + original header into a final `.BIN`. Used
internally by `link_overlay.py`.

### `tools/overlay/build.py`
Batch driver: runs gen_splat_yaml → splat → fill → link for all 19 overlays.

### `tools/overlay/gen_asm_void.py`
Bulk hybrid asm-void match generator (modeled on the boot ELF
`/tmp/gen_hybrid.py`). Walks each overlay's per-function `.s` files, filters
to candidates without data relocs / indirect calls / cross-file labels, emits
a `.word`-for-branches asm-void wrapper in `src/overlays/AREAXX/<name>.c`,
compiles it via mwccmips, and keeps only those whose `.text` bytes and
relocation targets match the splat-assembled reference. Must run inside the
toolchain container so mwccmips + GNU as are available directly.

---

## 8. Open Questions

1. **Overlay loader location**: the boot ELF function that reads `OVERLAY\AREA*.BIN`
   from disc and DMA-copies it to 0x823500 was **not yet pinned**. It uses the
   filename table at vram 0x00272FA0 (file offset 0x1732A0). It's likely in the
   0x130000–0x1E0000 game-code range. Finding it (via IOP RPC or sceCdRead
   patterns) will confirm the exact load sequence and whether the 0x40-byte
   nop sled at 0x823500 is patched at load time or left as nops.

2. **The 0x40-byte nop sled**: purpose uncertain. Options: (a) a reserved
   vector table that the loader patches with `j` instructions after loading;
   (b) pure alignment padding left by the original mwldmips link; (c) a
   deliberately-empty entry point area. Currently leans toward (b) — mwldmips
   may have aligned the code section to 0x40 bytes. The DCDecomp hex-pattern
   file (`overlay.hexpat`) does not address this.

3. **`$gp` in overlays**: confirmed (by the AREA18 function using `sw $v1, -30536($gp)`)
   that overlays use the **same `$gp = 0x27D370`** as the boot ELF. Overlay-local
   globals in the 0x823500+ data section use `lui`/`lw` (not gp_rel), as expected
   (they're 3.5 MB outside the gp ±32KB window).

4. **BSS zeroing**: the loader zeros `bss_size` bytes starting at
   `load_address + text_size + data_size` before calling the overlay init function.
   AREA00's bss_size is 0x295900 (≈2.7 MB!) — this is the overlay's own heap /
   runtime scratch space, not a BSS in the traditional sense. The large sizes
   suggest the overlay gets a large chunk of the physical RAM that the boot ELF
   reserved via PT_LOAD.

5. **PT_LOAD slot ↔ overlay_id mapping**: verified that PT_LOAD slot N corresponds
   to overlay_id N, not to the AREA filename number (AREA04 is overlay_id 5, not 4,
   because AREA05 is absent and the id counter continues). The mapping is
   by `overlay_id` field in the MWo3 header, not by filename.

6. **`static_init_address`**: for AREA18 this equals `load_address + text_size`
   (= 0x8235C0), which is the start of the data section — suggesting the C++
   static initializer list is stored there. Confirm by checking if the loader
   iterates function pointers between `static_init_address` and
   `static_init_end_address` and calls each one at load time.

---

## 9. Relationship to Track A

Track A (boot ELF decomp) and the overlay decomp are **parallel workstreams**
that don't conflict at the source level. They share:
- The same compiler (`mwccps2 2.3.1.01`).
- The same toolchain container (`exterm-toolchain`).
- The same `$gp` value.
- The same call conventions and code-gen patterns.

They diverge in:
- The linker LCF (overlays have a simpler LCF, separate output file).
- Symbol namespacing (overlay functions are prefixed `overlay_AREANN_`).
- splat configs (one per overlay, in addition to the boot ELF config).

The natural sequencing is: complete the overlay pipeline scaffold with AREA18
(can be done without interrupting Track A), then alternate between advancing
Track A and advancing overlay matches as motivation dictates.

---

_End of OVERLAYS.md_
