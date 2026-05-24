# Extermination — Runtime Overlay Matching Plan

_Created: 2026-05-24_

The disc ships 19 `OVERLAY/AREA*.BIN` files that are the **second matching surface**
of this project, alongside the boot ELF `SCUS_971.12`. The boot ELF is well under
way (~44% matched); the overlays are currently 0% addressed. This document records
everything learned in a dedicated investigation session, and lays out the concrete
plan for matching them.

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

## 4. Architectural Plan for Matching

### 4.1 Splat extension

**Splat can disassemble overlays directly** with a per-overlay YAML. The
approach mirrors how the boot ELF config works:

```yaml
# config/AREA07.yaml (example)
name: AREA07
basename: AREA07
target_path: config/AREA07.BIN
base_path: build
build_path: build
asm_path: build/asm/overlay/AREA07
asset_path: assets/overlay/AREA07
src_path: src/overlay/AREA07
platform: ps2
compiler: mwccps2
# The overlay's .text starts at 0x823500, but the file starts at 0x40 (past the header)
header_encoding: none
options:
  platform: ps2
  compiler: mwccps2
  
segments:
  - name: AREA07
    type: code
    start: 0x40      # skip the MWo3 header
    vram: 0x823500   # maps to the overlay arena base
    bss_size: <bss_size_from_header>
    subsegments:
      - [0x40, c, AREA07]
```

A wrapper script should read the MWo3 header, compute `bss_size`, and
generate the YAML automatically.

**Symbol scoping**: because every overlay loads at the same vram, symbol names
must be scoped per overlay. The convention is:

```
overlay_AREA07_func_00823580   # the init function of AREA07
overlay_AREA07_func_00823900   # another function in AREA07
overlay_AREA07_D_008260C0      # a data label in AREA07
```

Splat's `symbol_addrs.txt` does **not** support per-file scoping natively. Two
options:
1. Maintain a **separate `symbol_addrs_overlayAREANN.txt`** per overlay, and
   reference it in the per-overlay splat config.
2. Use a single global file with a naming prefix convention and let splat
   discover symbols by address range (all overlays share 0x823500+, but you
   only run one config at a time).

Option 2 is simpler for now. Since objdiff diffs one overlay at a time, and
the build system rebuilds one overlay at a time, address collisions never
occur in practice.

### 4.2 Build system

Each overlay is compiled as a **separate mwldmips link** producing a separate
`AREA07.BIN` (the full MWo3 file with header). The pipeline mirrors
Track A's `fill_unmatched.py` / `link.py` but targets overlay vram:

1. **splat disassembles** the original AREA07.BIN → per-function `.s` files
   in `build/asm/overlay/AREA07/`.
2. **fill_unmatched** assembles all `.s` files into filler objects, substituting
   matched `.c` objects from `src/overlay/AREA07/`.
3. **link.py (overlay variant)** generates an overlay-specific LCF:
   - `MEMORY` section: one region `overlay_AREA07 : ORIGIN = 0x00823500,
     LENGTH = 0 > AREA07.BIN`
   - Places each function object in vram order.
   - Sets `_gp = 0x0027D370` (same as the boot ELF — overlays share the same
     `$gp` since they run in the same address space).
   - Outputs the MWo3-format binary (mwldmips produces relocatable output;
     `tools/decomp/repack_iso.py` will swap in the rebuilt binary).
4. **Compare**: the loadable region of the rebuilt `AREA07.BIN` must be
   byte-identical to the original.

The overlay LCF is simpler than the boot ELF's (no GPREL16 overflow issues, no
cross-segment labels, no symbol deduplication). The main complication is the
**MWo3 header** — mwldmips does not write MWo3 headers; the linker produces a
raw ELF. A post-link step (`tools/overlay/pack_mwo3.py`) reads the resulting
ELF's `.text` and `.data` sections, prepends the 0x40-byte header (copied from
the original), and writes the final AREA07.BIN.

Alternatively: mwldmips with `> AREA07.BIN` output may strip the ELF
and output a raw binary — the same behavior exploited by DCDecomp for its
overlay outputs (see `SCUS_971.11.lcf` `title (RWXO) > TITLE.bin`). If so, the
header must be prepended in the post-link step.

### 4.3 objdiff integration

Each overlay gets its own objdiff unit. In `objdiff.json`:

```json
{
  "units": [
    {
      "name": "overlay/AREA07/overlay_AREA07_func_00823580",
      "target_path": "build/filler_overlay/AREA07/overlay_AREA07_func_00823580.o",
      "base_path":   "build/obj_overlay/AREA07/overlay_AREA07_func_00823580.o",
      "metadata": { "complete": null }
    }
  ]
}
```

### 4.4 Compiler flags

Overlays were compiled with the **same mwccps2 2.3.1.01** (the `MW MIPS C
Compiler (2.3.1.01)` string is in the boot ELF `.comment`, and there is no
reason to expect a different compiler for the overlays — they link into the same
address space). Expected flags:
- `-O4,p` (same as game code in the boot ELF)
- `-sdatathreshold N` (TBD per overlay — depends on what variables are gp_rel)
- The `$gp` value is the same as the boot ELF (0x27D370), so gp_rel range
  (±32KB) covers the boot ELF's data; the overlays' own data is in the
  0x823500+ range, which is outside gp_rel range. Expect overlay-local
  globals to use `lui`/`lw` (hi/lo) addressing.

### 4.5 Relocation handling

The MWo3 format is a **position-dependent binary** (loaded at a fixed address,
not position-independent). The boot ELF's overlay loader reads text_size + data_size
bytes from file and copies them verbatim to vram 0x823500. No relocation patching
is needed at load time — unlike ELF, MWo3 is pre-linked to a fixed address.
This matches what we see: the text section already contains absolute jal/j
targets in the 0x82XXXX range.

For the **decomp link**: mwldmips can produce a fixed-address binary directly,
exactly as it does for the boot ELF, if the LCF specifies the correct origin.

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

## 6. Recommended First Target: AREA18

**AREA18.BIN** (2,176 bytes, overlay ID 15) is the recommended "hello world"
overlay target:

- **Smallest overlay** at 2,176 bytes total (only AREA22 at 2,304 bytes is
  comparable; both are stubs).
- **Exactly one function** in the text section (7 instructions at 0x823540,
  plus 0x40 bytes of nop padding before it).
- **No non-leaf calls** — the single function is a pure leaf using only
  `addiu`, `sw`, `lui`, `jr $ra`.
- The function was already decoded above; it's straightforwardly matchable
  from C using `-sdatathreshold 0` (gp_rel for the three gp-relative stores).
- Serves as the scaffold test for the **entire overlay pipeline**:
  splat config, fill_unmatched overlay variant, mwldmips overlay LCF,
  MWo3 header packing, byte-identity check.

Once AREA18 is 100% matched and the pipeline produces a byte-identical
AREA18.BIN, the same pipeline applies to every other overlay.

**After AREA18**: AREA22 (also a stub, one function, slightly different) as a
sanity check that the pipeline generalizes. Then AREA08 (smallest real area,
18,944 bytes, ~30–40 functions) as the first non-trivial overlay match.

---

## 7. Required New Tools

### `tools/overlay/extract_overlay.py`
Extracts text and data sections from a MWo3 file into separate loose files,
and dumps the header fields. Use as a quick inspection tool and as input to
the splat pipeline.

### `tools/overlay/pack_mwo3.py`
Packs a compiled text section + data section back into a MWo3 binary:
- Reads original header from the reference file (or takes CLI args).
- Prepends the 0x40-byte header.
- Appends text + data sections.
- Writes the rebuilt AREA**.BIN.
- Verifies byte-identity against the original.

### `tools/overlay/gen_splat_yaml.py`
Reads a MWo3 header and generates the per-overlay splat YAML config,
including the correct `bss_size` and `vram` values.

These three tools are clean originals (no disc content) and can live in
`tools/overlay/` alongside the existing `tools/decomp/` scripts.

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
