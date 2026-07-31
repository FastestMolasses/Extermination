# Remaining-stub inventory (s86 audit)

`src/*.c` totals **2953** units: **2044 byte-matched**, **785 NEARMISS** (readable C,
documented non-byte-identical), **124 stubs**. Readable C therefore stands at
**2829/2953 = 95.8%**.

The headline "124 stubs" overstates the remaining decompilation work, because the bucket
mixes three very different things. Classified by disassembly content:

| class | count | decompilable to C? |
|---|---|---|
| plain MIPS code | 50 | yes — this is the real target |
| genuine 128-bit SIMD (MMI) | 30 | no direct C; needs a documented spec |
| VU0 macro-mode / COP2 | 36 | no direct C; needs a documented spec |
| COP0 privileged | 3 | no direct C (mfc0/mtc0/ei/di) |
| 4-byte data mis-segmented as code | 8 | **not code at all** |

## The 8 that are not functions

`func_0025F0D4`, `_0D8`, `_0F8`, `_0FC`, `_11C`, `_120`, `_140`, `_144` are single 4-byte
words in the 0x25F0xx data region that splat disassembled as `bltzal`/`bgezal`/`bgezall`
(splat itself tags them "handwritten instruction"). They are data, cannot become C, and
should be excluded from any "unmatched code" denominator. They already link byte-exact from
the `.s`.

## MMI is not automatically hand-written

`lq`/`sq`/`qmfc2` alone do NOT mean hand-written SIMD — mwcc emits them for ordinary
128-bit stack spills and struct copies in plain C. Only *arithmetic* MMI (`paddw`, `pextlw`,
`pcpyld`, `pmaxh`, ...) indicates real vector work. Splitting on that:

- **5 are lq/sq moves only** and are ordinary decompilable C: `_start`, `func_00207E40`,
  `func_0010B160`, `gs_vram_readback`, `sub__0000000000000000Inf`.
- Several more carry only 1–3 arithmetic MMI ops across hundreds of instructions
  (`func_00181BA0` 1/115, `sub_BASCUS_97112_DS00_00_EX_DATA_00` 1/190, `func_001B99F0`
  2/107, `func_001F4190` 3/426, `sub_O_STREAM_MUSIC_DAT_1` 3/42). These are mostly ordinary
  code with a couple of compiler-generated MMI instructions and are worth attempting.

## The `sub_*` names are library code, not game code

Names like `sub_Invalid_macroblock_type_code_0` and `sub_chroma_format_needs_to_be_1_420`
come from splat naming a function after a nearby string. The strings identify them as Sony's
`sceMpeg` MPEG decoder and `libpad`/`libkernel` — all in the SDK/lowmem region below
0x120000, so they route to the **ee-gcc** lane, not mwcc.

## What "fully readable C" actually requires

- 50 plain-MIPS stubs — decompile (in progress).
- ~10 more recoverable from the low-SIMD MMI set.
- ~66 genuine VU0/MMI/COP0 — these can never be C that byte-matches. The port needs a
  **documented semantic spec** per function (what it computes, in what layout), not a C
  translation. That is the remaining open work item.
- 8 data words — reclassify, not decompile.
