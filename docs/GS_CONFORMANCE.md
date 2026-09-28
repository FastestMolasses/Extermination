# GS conformance captures (PCSX2 software renderer)

Job B16, lane GSCAP, 2026-09-27. This is the reference data for the port's
GS-exact Original profile: designed primitives are sent to the GS of a
running PCSX2 with its **software renderer**, and the pixels it writes are
recorded and turned into rules.

**Clean room.** No emulator source was opened, read or searched for this work
(not the local `pcsx2/` tree, not PCSX2/GSdx/Play!/DobieStation code online).
The inputs are public GS documentation (register and field layouts, the GIF
tag and DMA tag formats, the documented texture-function, alpha-test and
blend equations, pixel formats) and the measurements below. Every candidate
model is written from that documentation or from generic rasterisation
conventions. It counts as a rule only where it reproduces every recorded pixel.

**Reference, not hardware.** PCSX2's software renderer is PCSX2's model of the
GS. Nothing here shows that it equals a real GS bit for bit.

Tools (all native arm64 macOS, decomp `.venv`):
- `tools/gs_conformance.py`: the harness (packet builder, emulator driver,
  decoder).
- `tools/gs_conformance_suite.py`: the test batches.
- `tools/gs_conformance_analyse.py`: the rules and their scores.

Outputs go to `build/b16/gscap/` (ignored).

## 1. Feasibility: how a designed GIF packet is drawn

No emulator change and no GS dump is needed. The DebugServer and Pine tools
suffice.

1. **Load and synchronise.** User slot 04 (first control) is loaded hidden
   through `route_capture.open_session`, with `-statefile`. The slot file is
   never written, and `pcsx2_session` checks its sha256 on close. The tool
   runs to the main loop's vsync-wait start `0x1AAFF0`, then to the vsync ISR
   entry `0x1AB140`. CAPTURES_C7.md 5b measured that the game writes nothing
   more to the GS from there to the next loop top, and that its last writes
   of the frame can land after the wait start. Kicking at the ISR entry
   therefore leaves the GS to the harness.
2. **Plain memory writes do not start DMA (measured).**
   - The first probe wrote D2_QWC, D2_MADR and D2_CHCR (`0x1000A020`,
     `0x1000A010`, `0x1000A000`) through the DebugServer's `write_memory`.
   - All three read back unchanged (CHCR 0x1, MADR 0x821210, QWC 0), both
     at once and at the next vsync ISR.
   - Nothing was drawn.
   - Receipt: `build/b16/gscap/probe/write_memory_attempt.json`.
3. **An injected EE routine starts the DMA.**
   - A 16-instruction routine at `0x01A00000` does the register stores. It
     sets D2_QWC to 0, D2_TADR to the tag list and D2_CHCR to 0x105 (from
     memory, source chain, STR). It then polls D2_CHCR until STR clears and
     stops on a breakpoint.
   - The routine's words never change between batches. The tag-list address
     is read from a data word next to it, so a rewrite can never meet a
     stale recompiled block.
   - The harness enters the routine with the DebugServer's `set_pc`. It then
     restores the PC and the four GPRs the routine uses exactly, and asserts
     that all 32 GPRs are back.
   - The DMA tag list uses REF tags and ends with REFE. It is written at
     `0x01BF0000`, and the GIF packets at `0x01C00000`. That region is zero
     in every EE capture.
   - The first working probe (`probe/probe_injected.json`) drew two sprites.
     Both rectangles decoded exactly.
4. **Evidence per batch** (`<batch>/kick.json`):
   - D2_CHCR has STR clear, D2_QWC is 0, and D2_MADR is at the packet end.
   - Every batch ends with a fence: a 64x32 sprite with a known word into
     page 0x1BF. `decode.json` checks that fence (`fence_ok`).
   - Each batch took 0.8..1.0 s of host time, breakpoint round trips
     included.
5. **Snapshot.** A save state goes to a free slot (40 or higher) and is
   moved out of the slot folder at once. Only its `gs.bin` (the GS freeze:
   425-byte header, 4 MB of local memory, 84 trailing bytes; see
   tools/gs_vram.py) and the host screenshot are kept.
6. **Decode.**
   - Every test's buffers are decoded through swizzle maps that the
     `layout` batch measures (section 5.1).
   - For PSMCT32 and PSMZ32 those maps equal the documented tables
     (`c7cap_partb.gs_word_map`).

The session keeps running between batches: after a snapshot the game runs
to the next vsync-wait start. The game's own frame then draws with whatever
state the batch left. That does not matter, because every test starts from
a full state reset.

**Every test is self-contained** (`gs_conformance.Test`):
- It gets its own colour buffer, and its own Z buffer when Z is read back.
- It writes the whole state first: XYOFFSET, SCISSOR, PRMODECONT, COLCLAMP,
  DTHE, DIMX, PABE, FBA, SCANMSK, TEXA, FOGCOL, FOG, ALPHA, TEX1, CLAMP,
  TEST and ZBUF.
- It pre-fills its Z buffer through a PSMCT32 view of the same pages, so
  even the byte that Z24 never writes is known (0x5A).
- It clears its colour buffer.
- The inputs of every test are stored as structured JSON in
  `<batch>/batch.json`: every register write as (register, value) in packet
  order, every upload with its sha256, and the primitives with their vertex
  attributes. The uploaded arrays are in `<batch>/inputs.npz`. The exact
  packet is `<batch>/packet.bin`.

## 2. The emulator ini (user rule 2026-09-26)

The procedure is the one CAPTURES_C7.md 5b records. No tool edits the ini.

1. **Pre copies.** PCSX2 was not running. `build/b16/gscap/pre/` received
   copies and `sha256.txt` of:
   - `build/startup-reference/inis/PCSX2.ini` (fc7d417d…);
   - `inis/playtime.dat` (39391071…);
   - `portable-data/memcards/Mcd001.ps2` (757d6f77…) and `Mcd002.ps2`
     (47ebe237…);
   - the unused `portable-data/inis/PCSX2.ini` (957dc2ef…).
2. **Switch.** Line 220, the only `Renderer` line, was verified to read
   `Renderer = 17` and changed to `Renderer = 13`. No other line was
   touched. `gs_conformance.py capture` refuses to start unless the live
   ini says 13.
3. **Captures.** Six sessions ran: two probes, the layout batch, the five
   main batches, probe2 and the full repeat. Each ended with
   `no emulator process left: True`.
4. **Restore.** With PCSX2 stopped, the line was set back to
   `Renderer = 17`.
   - The live `PCSX2.ini` is byte-identical to the pre copy (same sha256;
     PCSX2 rewrote no key this time).
   - Both memory cards and the unused ini still have their pre sha256.
   - `playtime.dat` changed (PCSX2's play-time counter): 39391071… →
     3ae95c32….
   - The save-state folder holds only the user slots (01-04, 06-08, 11-15).

## 3. Reproduce

```sh
# macOS arm64, decomp root.  Manual: pre copies + Renderer 17 -> 13 (section 2), PCSX2 not running.
.venv/bin/python tools/gs_conformance.py list              # batches, pages, packet sizes
.venv/bin/python tools/gs_conformance.py capture           # all batches, ~1 s each + ~25 s start-up
.venv/bin/python tools/gs_conformance.py decode            # no emulator: maps, then every test
.venv/bin/python tools/gs_conformance_analyse.py           # no emulator: build/b16/gscap/analysis.json
GSCAP_OUT=build/b16/gscap_repeat .venv/bin/python tools/gs_conformance.py capture   # optional second copy
# Manual: Renderer 13 -> 17 with PCSX2 stopped, diff the ini against build/b16/gscap/pre/.
```

**Repeatability.** A second full run (`build/b16/gscap_repeat`, separate
session, different game frames) produced identical packets. All 293 decoded
arrays (colour and Z of every test) were identical.

## 4. The suite

The suite has 252 tests in 7 batches, one DMA kick and one snapshot each.
Coverage tests draw each primitive with a one-bit colour id through the
additive blend `ALPHA` A=Cs B=0 C=FIX(0x80) D=Cd. Each pixel's value then
lists exactly which primitives covered it. Count tests add 1 per primitive
instead.

| batch | tests | covers |
|---|---|---|
| layout | 7 | known uploads of PSMCT32 (one page and a 2x2-page buffer), PSMCT16, PSMZ32, PSMZ16; drawn sprites in CT32 + Z24 and in CT16 |
| raster | 26 | integer-vertex triangles (split squares, fan, shared edges); 8 x 24 random triangles with 1/16 vertices, extents 60 / 14 / 4 / 2 px; half and quarter-pixel vertices; 14 degenerate and near-degenerate triangles; 48 slivers (1/16..1 px wide); 4 triangles in all 6 vertex orders; two jittered 72-triangle meshes (count); 3 x 24 random sprites (half given in reversed corner order) + 17 special sprites; 3 x 24 random lines + 20 axis / 45-degree / zero-length lines; 24 points; a tristrip and a trifan (count) |
| shade | 18 | Gouraud RGBA (extremes, deltas of 1..4, 8 random triangles, strip + fan); flat vertex selection for tri, strip, fan, line, line strip, sprite (IIP 0 and 1), point; Gouraud lines; Z interpolation in Z24 and Z32; sprite Z; Z beyond the format in Z24 and Z16 |
| texture | 40 | 64x64 CT32 1:1 (UV and ST); 16x16 -> 64x64 nearest and bilinear (UV, ST, fractional UV and positions); 64x64 -> 32x32 / 21x21 minification; REPEAT / CLAMP / REGION_CLAMP / REGION_REPEAT x nearest / bilinear; TFX 0..3 x TCC 0/1 with four vertex colours; PSMT8 and PSMT4 with CT32 CLUT (CSM1), nearest and bilinear; identity-index textures that map index -> CLUT position; T4 with CSA 1; four perspective quads (ST/Q, Q per corner); a UV triangle with MODULATE; the game's level class |
| pixel | 120 | fog (64 F values x 4 colours; a Gouraud-F triangle; fog over MODULATE); the 8 alpha-test modes with AREF 0x40 / 0x81; the 4 AFAIL modes (colour and Z); all 81 blend equations (FIX 0x5A) over a random 64x32 destination and a random 1:1 source texture; the game presets 0x44, 0x68, 0xA8 with COLCLAMP 1 and 0; PABE; FBA with and without blending; DATE with DATM 0/1; the shadow receivers' TEST with ALPHA 0x44; FBMSK; Z test NEVER / ALWAYS / GEQUAL / GREATER on columns around the clear value; ZMSK; crossing Z triangles; SCISSOR |
| frame | 15 | CT16 and CT32 frames with DTHE 0/1, two DIMX matrices, COLCLAMP 1/0 (flat bands + Gouraud); CT16 alpha bit; one primitive list under XYOFFSET (1792, 1936), (1792, 1936.5), (1792.5, 1936); a 512x224 field with six Gouraud triangles under OFY 1936.0 and 1936.5 |
| probe2 | 26 | follow-ups: which primitive kinds dither; two random Gouraud triangles in all 6 vertex orders (colour and Z); ten 1-D colour ramps (x only / y only); 40 lines that start or end exactly on a pixel diamond's boundary |

## 5. Measured rules

`analysis.json` holds every candidate's score. "Exact" means the rule
reproduces every recorded value it covers.

### 5.1 Local memory (layout batch)

- **Swizzle maps.** Uploading index values and reading the raw pages back
  gives full permutations for PSMCT32, PSMCT16, PSMZ32 and PSMZ16.
  - The PSMCT32 and PSMZ32 maps equal the documented block and column
    tables. PSMZ uses the PSMCT32 block order XOR 24.
  - A 128x64 buffer is row-major in pages.
- **Drawn sprites decode exactly** through the maps in CT32, CT16 and Z24.
- **PSMZ24 never writes the top byte.** It stays at the value the pre-fill
  left (0x5A) at every written pixel.

### 5.2 Coverage

Coverage is measured over 500 triangles and 93 sprites, 136 lines and 24
points, across all coverage tests, including the XYOFFSET, half-pixel and
scissor tests.

- **Window coordinates.** A primitive's window coordinates are its vertex
  coordinates minus XYOFFSET (12.4 fixed point). Pixel (x, y) is sampled at
  the integer point (x, y).
- **Triangles: exact top-left rule, 0 mismatches over 38,809 pixels.**
  - A pixel is covered when its sample point lies strictly inside all three
    edges.
  - A sample exactly on an edge is covered when that edge is a left edge
    (the interior lies to its right) or a horizontal top edge.
  - Zero-area triangles draw nothing.
  - The result does not depend on vertex order (4 x 6 orders).
  - The two meshes are watertight: every inside pixel is covered exactly
    once.
  - The other 23 candidate rules (sample offsets of 0 or 1/2 on each axis x
    six tie rules) mismatch 564..4,959 pixels.
- **Sprites: exact, 0 mismatches over 5,704 pixels.** A sprite covers
  x0 <= x < x1 and y0 <= y < y1 on the sorted corners, whichever order the
  corners are given in. Zero-width sprites draw nothing.
- **Points: exact.** A point lights pixel (floor(x + 1/2), floor(y + 1/2)).
- **Lines: exact over 136 lines and 1,931 pixels,** including the 40
  boundary cases.
  - **Major axis.** It is the axis of the larger extent. A 45-degree line is
    x-major.
  - **Pixels lit.** For every pixel centre line on the major axis that lies
    strictly between the endpoints, the pixel at the minor coordinate
    rounded half up is lit. It is not lit when the end point lies inside
    that pixel's diamond.
  - **The diamond** is |dx| + |dy| < 1/2 around the pixel centre.
  - **Start.** A start exactly on a centre line lights its pixel. An end
    exactly on a centre line does not.
  - **The pixel just before the start** (a start that is not on a centre
    line) is lit only when the start lies inside its diamond.
  - **Diamond boundary.** A point exactly on the boundary counts as inside
    when its offset along the line's minor axis is negative (above the
    centre for an x-major line, left of it for a y-major line).
  - Zero-length lines draw nothing.
  - Plain half-open major-axis stepping mismatches 33 pixels. The diamond
    rule with other boundary conventions mismatches 5..70.
- **SCISSOR** bounds are inclusive: SCISSOR (10..40, 5..50) keeps
  x 10..40 and y 5..50.
- **XYOFFSET half line.** OFY 1936.5 against 1936.0 is a resampling at the
  shifted positions, not a one-row shift: 530 of 4,096 pixels differ in the
  64x64 test and 18,841 in the 512x224 field. The coverage rules above hold
  under both offsets with 0 mismatches.

### 5.3 Vertex attributes

- **Flat colour.**
  - A triangle takes its third vertex, so strips and fans take the last
    vertex of each triangle.
  - A line or line strip takes the second vertex of each segment.
  - A sprite takes its second vertex, with IIP 0 or 1.
  - A point takes its own vertex.
- **Sprite Z** is the second vertex's Z.
- **Z beyond the format** clamps to the format's maximum:
  - Z24 stores 0xFFFFFF for 0x01000000, 0x01234567 and 0xFFFFFFFF.
  - Z16 stores 0xFFFF for 0x10000 and above.
  - Colour is still written.
- **Gouraud colour: not exact here; settled by the follow-up probes
  (section 8.2: 8.7 row starts with 4-pixel lanes).**
  - floor(exact barycentric value at the integer sample) matches 41,607 of
    42,072 channel values. The rest differ by ±1 and lie near integers.
  - The result is independent of vertex order (probe2, colour and Z).
  - Ramps that vary only in y are exact.
  - An x-only ramp with slope 7/60 shows a smaller effective slope. At
    x = 26, 43 and 52 the value is one lower than floor(exact).
  - **Best model found.** On each row, the exact value is taken at the
    triangle's first covered pixel. Each pixel to the right adds dC/dx
    truncated toward zero to 2^-9. The result is floored.
  - That model matches 291,373 of 291,716 channel values (99.88 %) over the
    Gouraud tests, the ramps and the 512x224 field. The residues lie within
    about 0.005 of an integer in the small triangles and are more frequent
    over long spans.
  - The arithmetic that sets the row start was identified later (8.2).
- **Z interpolation: not exact (open; partly settled in 8.2).**
  - Z24 is within ±1 of floor(exact) on every pixel, and equal on 1,692 of
    1,894.
  - Z32 is within 56 (mean −27), which suggests single-precision arithmetic
    at large values. This is not modelled.
- **Fog weight F interpolated over a triangle: not exact here; settled in
  8.2** (fog uses the 8.7 weight). The F that explains each pixel is closer
  to round(exact) (1,331 of 1,907) than to floor(exact) (971).

### 5.4 Texturing

All results here are exact unless stated.

- **Nearest.** The texel is floor(u), floor(v), where u, v are the texel
  coordinates at the integer pixel sample. For sprites, UV and ST/Q are
  interpolated affinely between the two corners. This holds in 1:1 copies,
  in 4x magnification, at fractional UV and positions, and in minification.
  A half-pixel sample offset fails the fractional and minification tests
  (3,322 of 3,970 and 0 of 2,489 match).
- **Bilinear.**
  1. Take U = floor((u − 1/2) * 16), and V likewise.
  2. The texel index is U >> 4 and the weight fu = U & 15, and likewise
     for V.
  3. Interpolate horizontally first: a = t00 + ((t10 − t00) * fu >> 4), and
     b the same for the next row.
  4. Then vertically: a + ((b − a) * fv >> 4). Every shift is arithmetic
     (floor).

  This matches 14,651 of 14,651 pixels over four tests.
  - Eight-bit weights match the 4x magnifications but fail at fractional UV
    (2,054 of 3,970) and in minification (2,058 of 2,489).
  - Vertical-first order, a single weighted sum and truncation toward zero
    fail everywhere (393..2,732 of 4,096).
  - A half-pixel sample offset matches no pixel.
- **Wrap modes.** They apply to the integer texel index, for both filters.
  - REPEAT: i & (w − 1).
  - CLAMP: clamp to 0..w−1.
  - REGION_CLAMP: clamp to MINU..MAXU.
  - REGION_REPEAT: (i & MINU) | MAXU.
  - 8 of 8 tests match all 4,096 pixels.
- **Texture functions** (8 of 8 tests, all 4,096 pixels each). All results
  clamp to 255.
  - MODULATE: C = Ct*Cf >> 7, A = At*Af >> 7.
  - DECAL: C = Ct, A = At.
  - HIGHLIGHT: C = (Ct*Cf >> 7) + Af, A = At + Af.
  - HIGHLIGHT2: C as HIGHLIGHT, A = At.
  - TCC 0 gives A = Af.
- **CLUT (CSM1, CT32 palettes).**
  - PSMT8: index i reads the 16x16 CLUT image at position i with bits 3 and
    4 swapped (256 of 256 indices).
  - PSMT4: index i reads the 8x2 CLUT image in row-major order. With CSA 1
    and a 16x16 CLUT image, the 16 entries are that image's first 8x2 block
    (positions 0..7 and 16..23).
  - Bilinear filtering is applied to the looked-up colours. T8 and T4, both
    filters: 4 of 4 tests exact.
- **Perspective (triangles, ST with per-vertex Q).**
  - Exact perspective-correct S/Q and T/Q at the integer sample, plus the
    bilinear or nearest rule, match 13,926 of 13,932 pixels. The 6 misses
    are in two quads and are not explained. The STQ vertex grid and the
    span rule of 8.2 settle most of this.
- **The game's level class** is not reproduced (1,805 of 3,481 written
  pixels): a fogged, Gouraud, perspective tristrip with T8+CLUT, MODULATE,
  alpha GREATER 0 and Z GEQUAL.
  - The misses are in the interpolated colour and F. Alpha (texture only)
    matches everywhere.
  - Screen-linear colour and F with floor is closer than perspective-correct
    colour and F (for example 428 of 801 against 249 for R on the first
    triangle).
  - This is the most important open item for the Original profile. See
    section 6. **Settled in 8.2:** the colour and F reach the texture
    function and fog in 8.7, and the level strips are drawn without the
    STQ vertex grid because their vertex Z varies. The level class is
    exact except 4 colour values in 36,864 and its Z (one LSB on 0.8-4.8 %
    of pixels).

### 5.5 Pixel pipeline

- **Fog, exact.**
  - C' = FOGCOL + ((C − FOGCOL) * F >> 8), with an arithmetic shift. This
    equals (F*C + (256 − F)*FOGCOL) >> 8.
  - F = 0 gives FOGCOL exactly. F = 255 leaves 1/256 of FOGCOL.
  - It holds on 4,096 of 4,096 flat-F pixels, and on 4,096 of 4,096 fogged
    MODULATE pixels, where fog is applied after the texture function.
  - **(F*C + (255 − F)*FOGCOL) >> 8 is wrong:** 1,040 and 640 of 4,096
    match. The port's CHAIN_PAGE.md section 5 and OWNER_DRAW.md section
    7.2 state that form; this measurement contradicts it.
- **Alpha test, exact.**
  - All eight ATST modes compare the source alpha with AREF as documented,
    with AREF 0x40 and 0x81 (8 x 4,096 pixels).
  - AFAIL, for a failing pixel:
    - KEEP writes nothing.
    - FB_ONLY writes RGBA and no Z.
    - ZB_ONLY writes Z and no colour.
    - RGB_ONLY writes RGB, keeps the destination alpha and writes no Z.
- **Blending, exact** (95 of 95 tests x 2,048 pixels: the 81 equations with
  FIX 0x5A, the game presets and the variants).
  - C = ((A − B) * C >> 7) + D per RGB channel, with an arithmetic shift
    (floor). Truncation toward zero matches only 65 of the 95 tests.
  - As and Ad enter unclamped (alpha up to 0xFF).
  - COLCLAMP 1 clamps to 0..255. COLCLAMP 0 keeps the low 8 bits.
  - The written alpha is the source alpha.
  - PABE 1 blends only where As bit 7 is set. Other pixels take the source
    unblended.
  - FBA 1 ORs 0x80 into the written alpha, with or without blending.
  - DATE: a pixel is written only where the destination alpha bit 7 equals
    DATM.
  - FBMSK bits keep the destination bits.
- **Z test, exact.** NEVER, ALWAYS, GEQUAL and GREATER are unsigned
  compares against the stored Z. The GEQUAL / GREATER boundary sits exactly
  at the equal value. ZMSK 1 keeps Z and still writes colour.
- **Dithering and 16-bit frames, exact.**
  - A CT32 frame ignores DTHE: its output is identical to DTHE 0.
  - Writing to a CT16 frame without dither gives R, G, B >> 3, and alpha
    = A bit 7 (A 0x00 and 0x7F give 0; 0x80 and 0xFF give 1).
  - With DTHE 1 the value is (C + DIMX[y & 3][x & 3]) >> 3. DIMX[i][j] is
    the signed 3-bit field at bit 16i + 4j. The sum is clamped (COLCLAMP
    1) or wrapped to 8 bits (COLCLAMP 0) before the shift. This holds on
    4 x 1,792 Gouraud pixels.
  - **Which primitives dither.** Triangles do, flat and Gouraud (465 of 465
    pixels each), and so do lines (28 of 28). Sprites do not, IIP 0 or 1:
    196 of 196 sprite pixels and all flat-band sprites are undithered.
    Points do as well (section 9: p8_misc, 4,096 points per test); no
    point had been drawn on a CT16 frame before probe 8.
  - This matters to the first level only if a CT16 frame is used. The
    level's frames are CT32 (CAPTURES_C7.md 5).

## 6. What this settles for the port, and what is still open

**Settled, as measured rules of the reference:**
- the local-memory layout;
- triangle, sprite, line and point coverage, scissor and the half-line
  XYOFFSET;
- flat vertex selection and sprite Z;
- Z clamping and the Z24 top byte;
- nearest and bilinear sampling (the exact weight arithmetic), the wrap
  modes, TFX / TCC, the CSM1 CLUT order and CSA;
- fog with a constant F;
- the alpha test and AFAIL;
- all blend equations with COLCLAMP, PABE, FBA, DATE and FBMSK;
- the Z tests and ZMSK;
- CT16 conversion and dithering.

These are enough to write a software rasteriser for sprites and flat
primitives that reproduces the reference exactly.

**Open after this lane. Section 8 (lane RASTER) settles most of items 1
and 2; what remains is in the port's docs/GS_EXACT.md section 8.**
1. **The arithmetic of attribute interpolation inside triangles:** RGBA, F
   and Z. The best colour model reaches 99.88 %. Within the level class,
   the combination of interpolated colour and F with MODULATE and fog
   reaches only 52 %. That suggests that the texture function and fog see
   the interpolated values with more precision than the 8-bit floor used
   here. The next batch should isolate MODULATE with Gouraud colour, and
   fog with Gouraud F, on untextured and textured triangles, with Q = 1.
2. **Perspective STQ:** 6 of 13,932 pixels.
3. **Not tested:**
   - AA1, PRIM FIX, the context-2 registers;
   - mipmapping (MXL > 0, LCM, MTBA) and TEXA with CT16 / CT24 textures;
   - PSMT8H / T4HL / T4HH, CSM2, CT16S / Z16S frames;
   - DATE with a CT16 frame, SCANMSK;
   - sprites with Q other than 1, and textured sprites under DTHE.
4. **Display.** Only the GS local memory is recorded. The PCRTC merge and
   the field presentation are outside this harness (CAPTURES_C7.md 5b).

## 7. Files

- `tools/gs_conformance.py`: register encoders, GIF and DMA tags, the
  `Test` / `Batch` model, the capture driver (injected kick routine,
  register restore, snapshot) and the decoder with the measured maps.
- `tools/gs_conformance_suite.py`: the batches (fixed seeds; nothing
  disc-derived).
- `tools/gs_conformance_analyse.py`: the candidate models and scores.
- `build/b16/gscap/` (ignored):
  - `pre/`: the ini, playtime and memory-card copies and hashes.
  - `probe/`: the feasibility receipts.
  - `<batch>/`: `batch.json`, `packet.bin`, `inputs.npz`, `kick.json`,
    `snap/` (`gs.bin`, host screenshot, snapshot.json), `<test>.npz`
    (decoded colour and Z), `<test>.png` and `decode.json`.
  - `layout/maps.npz` and `layout_report.json`.
  - `analysis.json`.
- `build/b16/gscap_repeat/`: the repeat run.
- `tools/gs_conformance_probe3.py` .. `probe8.py` and `build/b16/gscap3` ..
  `gscap8`: the follow-up batches of lane RASTER (sections 8 and 9).

## 8. Follow-up probes for the CPU GS model (lane RASTER, 2026-09-27)

The port's CPU GS model (`extermination-port/src/gs/em_gs_raster.c`) needed
the interpolation arithmetic that sections 5.3 and 6 left open. Five more
probe tools use the same harness, rules and clean room as sections 1-4:
designed packets only, nothing disc-derived, no emulator source read. The
full rules, the model and the per-test verification are in the port's
`docs/GS_EXACT.md`. This section records the captures and the rules that
settle this document's open items.

### 8.1 The batches

| tool | batches (tests) | design |
|---|---|---|
| `gs_conformance_probe3.py` -> `build/b16/gscap3` | p3_start (38), p3_z (43), p3_stq (21) | Colour and fog weight with dC/dx = k/512 exactly over 512-pixel rows, so each row shows its start to 1/512. Vertical, sloped and off-screen left edges, inner scissors. MODULATE over 0xFF texels; Gouraud lines. Z24 / Z32 / Z16 planes under three XYOFFSETs. A 16x16 address texture (R = 16u, G = 16v) read by perspective triangles, sprites with Q, UV triangles; four level-class strips |
| `gs_conformance_probe4.py` -> `gscap4` | p4_rcp (6), p4_z (24), p4_misc (18) | Constant-STQ sprites and triangles over a 1024x1 address texture (the perspective divide); Z rows with dZ/dx = 1/512 exactly; lines and UV triangles with exact gradients; k/128 colour planes; triangle pairs on shared non-dyadic edges |
| `gs_conformance_probe5.py` -> `gscap5` | p5_s (39), p5_q (39) | A 1024x1 U16 address texture (R = 16i & 255, G = i >> 4). S planes with Q = 1 and slow exact gradients, and Q planes with S/Q constant or varying, some crossing a binade |
| `gs_conformance_probe6.py` -> `gscap6` | p6_cov (12), p6_tfx (25), p6_z (24) | 288 triangles with an edge through pixel centres (24 slopes x side x direction x vertex slot); all texture functions x TCC on exact colour planes with and without fog; apex-top Z triangles in four vertex orders |
| `gs_conformance_probe7.py` -> `gscap7` | p7_lvl (37), p7_wrap (33), p7_z (51), p7_zc (51), p7_scope (17), p7_flush (29) | Which state decides the STQ vertex grid (8.2): the level strips with one feature changed at a time, the wrap mode, Z writes against vertex Z, constant against varying vertex Z, and which writes end the span the decision covers |

All 24 batches decode with an intact fence. Reproduce (decomp root, macOS
arm64): `.venv/bin/python tools/gs_conformance_probeN.py capture` and then
`decode`, inside the manual ini switch of section 2.

### 8.2 Rules settled (each exact on the named tests)

- **PSMT8 / PSMT4 layout.** Measured from the T8 / T4 uploads:
  - PSMT8 byte in block = column * 64 + (j & 1) * 4 + (j >> 1) * 16 +
    ((x >> 3) & 1) * 2 + (r & 1) * 8 + (r >> 1), with r = y & 3 and
    j = (x & 7) XOR 4 on alternate column halves;
  - PSMT4 is analogous with 128, 8, 32, ((x >> 3) & 3) * 2 and 16.
- **Colour and fog weight in triangles (p3_start, exact).**
  - Each row starts at `floor(128 * exact)` (8.7 fixed point) at its first
    drawn pixel (the scissor-clipped first covered pixel).
  - The row is stepped in 4-pixel blocks aligned to x mod 4:
    `trunc(512 * dC/dx)` per block, plus `trunc(128 * d * dC/dx)` for a
    lane d away from the start lane.
  - The 8-bit colour is the value >> 7. This replaces the 99.88 % model of
    5.3.
- **Texture function and fog at 8.7 (p3 `mod_*`, `fog_*`, all of
  p6_tfx).**
  - MODULATE is Ct * C7 >> 14, and HIGHLIGHT adds A7 >> 7. TCC 0 gives
    A7 >> 7.
  - Fog is FOGCOL + ((C − FOGCOL) * F7 >> 15).
  - With constant F this is the formula of 5.5.
- **Z row start (p4_z).** From the top vertex, with the gradient
  numerators times the binary32 reciprocal of the area. The in-row step is
  exact, and the result is floored. It is exact on flat-top rows; apex-top
  and general triangles are one LSB off on a few percent of pixels (open).
- **Sprites with Q (p3_stq).** Both corners use the second vertex's Q.
- **The perspective divide (p4_rcp, p5).**
  - Each vertex's S and T are floored to 2^-(14 − E) and its Q to
    2^-(15 − E), with E the exponent of that vertex's Q.
  - The values are interpolated, rounded to binary32, divided, and floored
    in 1/16 texel.
  - This grid applies only as the next item says. A small noise (relative
    about 2^-19) remains open.
- **When the grid applies (p7).**
  - The grid applies exactly when every vertex of the whole span of queued
    primitives has the same Z. It does not matter whether Z is written or
    tested. One vertex with another Z removes the grid from every triangle
    of the span.
  - The span ends at a change of TEX0, CLAMP, TEST, COLCLAMP, DTHE or the
    PRIM attribute bits. (Section 9 measures the other boundaries, refines
    TEX0 to its texture fields and shows that sprite spans always use the
    grid.)
  - It does not end at the same value written again, at ALPHA with ABE 0,
    FOGCOL with FGE 0, TEXFLUSH, a strip -> list switch, or a transfer
    into unused memory.
  - This is behaviour of the reference, not a documented GS property.
  - It explains the level class: its strips have varying Z, so they are
    drawn without the grid.
- **Coverage on edges through pixel centres (p6_cov, 288 triangles).**
  The top-left rule of 5.2, exact. Two shared non-dyadic edges below a far
  middle vertex are not watertight (p4_misc `cov_nd_0/2`, 3 pixels; open).
- **Lines with exact gradients (p4_misc).** The attributes are
  `floor(128 * exact)` along the major axis.

**Correction.** 5.3 called Gouraud colour, fog weight and the level class
open, with the best model at 99.88 % and 52 %. They are settled above. The
level class is exact except 4 colour values in 36,864, plus its Z (one LSB
on 0.8-4.8 % of pixels).

**Model totals (superseded by section 9).** The model reproduces 553 of
the 759 tests of all batches bit for bit. The other 206 are recorded with exact counts: 1,914 colour and
16,569 Z values (port `tools/test_gs_raster_reference.py`,
`docs/GS_EXACT.md` sections 7-8).

### 8.3 The emulator ini for these captures

**Procedure** (section 2), once per capture run: probes 3, 4, 5 and 6 and
the six probe-7 batches, ten runs in all.
1. With PCSX2 not running, copies of the live `inis/PCSX2.ini`,
   `inis/playtime.dat`, both memory cards and the unused
   `portable-data/inis/PCSX2.ini` went to `build/b16/gscapN/pre/`, with
   `sha256.txt` and a listing of the save-state folder. Probe 7 uses `pre`
   and `pre2` .. `pre6`, one folder per run.
2. Line 220 was verified to read `Renderer = 17` and set to
   `Renderer = 13`. The diff against the pre copy showed only that line.
3. After the capture ("no emulator process left: True"), line 220 was set
   back to `Renderer = 17`.

**Results after each run:**
- The live ini differed from the pre copy at most in the
  `[GameListTableView] HeaderState` line, which PCSX2 rewrites on exit. It
  was identical after probe 4 and after p7_lvl.
- `Renderer` and every other key were equal.
- Both memory cards kept their sha256 (757d6f77..., 47ebe237...). The
  unused ini kept 957dc2ef....
- `playtime.dat` changed (the play-time counter).
- The save-state folder kept the same entries; only its mtime moved.

At the end, `Renderer = 17` is in place and no PCSX2 process is running.

## 9. Probe 8: the B16 review's open claims (fix round, 2026-09-27)

A review of the CPU GS model found rules that rested on documentation or
on analogy: points modelled as undithered with no point capture, span
boundaries "by the same rule", the CLD 4 / 5 compare, and the PACKED /
REGLIST paths with the Q of a PACKED RGBAQ. `tools/gs_conformance_probe8.py`
measures them. Same harness, clean room and ini procedure as sections 2
and 8.3; nothing disc-derived; no emulator source read.

### 9.1 The batches (`build/b16/gscap8/`, ignored)

| batch | tests | what |
|---|---|---|
| p8_span | 67 | p3_stq's level strips over the address texture: a varying-Z half strip, one write or transfer, a constant-Z half strip (or the reverse). 33 cases per strip: TEX1 K, TEX2 same / CBP, MIPTBP1, TEXA, TEXCLUT, XYOFFSET moved with the vertices, SCISSOR shrunk, FRAME FBMSK / same, ZBUF, FBA, DIMX with DTHE 1 / 0, ALPHA / PABE with ABE 1 / 0, FOGCOL with FGE 1, same-value TEST / CLAMP / COLCLAMP / DTHE / ALPHA / FOGCOL, PRIM FGE / ABE, the reverse order for IIP / TME / FST and nothing, transfers of the bytes already there into the texture, the frame and the Z buffer |
| p8_class | 17 | a 1024 x 1 address texture: a varying-Z strip then 1,024 constant-Z 2x2 sprites and the reverse orders, strips and points, and sprites whose corners' Z differ |
| p8_misc | 15 | 4,096 points per test on CT16 with DTHE 1 (COLCLAMP 1 / 0), CT16 DTHE 0 and CT32 controls; CLD 0..5 sequences with the CLUT memory rewritten between loads; textured points (UV / STQ, nearest / bilinear) |
| p8_gif | 8 | PACKED [ST, RGBAQ, XYZ2] with PRE; the Q of a PACKED RGBAQ in a tag without ST (after a PACKED ST tag, an A+D RGBAQ, an A+D ST); PACKED UV / XYZF2 / FOG; ones in unused bits, ADC, A+D and NOP descriptors; REGLIST even and odd |
| p8_more | 35 | second run: TEX0 field changes (TBP0 to an identical copy, CBP, TCC, CLD on CT32), TEX2 CLD / CSA; a T8 + CLUT that reads as the address texture with TEX0 / TEX2 CBP changes to an identical CLUT copy with and without a load, a same-CBP reload, CLUT-memory transfers without a load, a reload of changed memory; strips drawn as triangle lists with one Z per triangle |
| p8_gif2 | 5 | third run: PACKED descriptors 0, 6, 8, C, D; REGLIST with A+D and NOP descriptors whose words would move the drawing if written |

**Discrimination.** Before capturing, `gs_conformance_probe8.py
discriminate` runs the port's model on every packet three ways: as is,
with the boundary forced to end the span, and forced not to. Every p8_span
and p8_more boundary test differs by 19..49 values between the two
(p8_class 151..2,116). The triangle-list and CLUT-change tests were checked
with the grid switched on and off instead (42..74 values). So each capture
decides its question. Model-only marker registers do this; they
are never in a captured packet.

### 9.2 Results (each exact on the named tests)

- **Points are dithered** on CT16 with DTHE 1 (`dither_points_ct16_*`;
  the undithered model left 2,164 and 2,166 values off). Sprites are not.
- **Span boundaries.** End: a class change (triangle / sprite / point),
  PRIM FGE, ABE, TME, FST, IIP; TEX0 TBP0, TCC, TFX; TEX1, CLAMP, TEXA
  (with TME 1); ALPHA and PABE (ABE 1), FOGCOL (FGE 1), DIMX (DTHE 1);
  TEST, COLCLAMP, DTHE, XYOFFSET, SCISSOR, FRAME, ZBUF, FBA; HOST -> LOCAL
  transfers into the span's texture, frame or Z memory; a CLUT load that
  changes the CLUT or comes from another CBP. Do not end: same values;
  those registers with their bit 0; MIPTBP1 (MXL 0); TEXCLUT (CT32); TEX0 /
  TEX2 changes of the CLUT fields alone (any format); a same-CBP reload of
  the same contents; transfers into CLUT memory without a load.
- **The grid decision.** Triangles and points: all vertices of the span
  share one Z (a triangle list with one Z per triangle, different between
  triangles, gets no grid; nor do points of different Z). Sprite spans
  always use the grid, whatever either corner's Z.
- **The divide.** The reference's quotient S/Q is rounded to binary32
  before the scale: three p8_class sprites with an exact value within
  0.0005 below a 1/16-texel step read the step.
- **CLD.** 0..5 as documented; CBP0 and CBP1 are independent; a load reads
  the memory at that moment.
- **GIF.** The Q of a PACKED RGBAQ is 1.0 at the start of every tag, then
  the last PACKED ST's Q in that tag; A+D RGBAQ / ST do not change it.
  PACKED descriptors 0, 1, 2, 3, 4, 5, 6, 8, A, E, F and REGLIST (E and F
  skip their word) behave as documented, unused bits ignored. PACKED C / D
  (XYZF3 / XYZ3) use the PACKED XYZF2 / XYZ2 layout without a drawing kick;
  the port's first model read them in the register layout (4,320 values
  off).
- **Textured points** (UV and STQ) are exact.

**Still open in probe 8's tests** (port `docs/GS_EXACT.md` 8.3, 8.7):
UV-triangle texels in p8_span `rev_prim_fst` (8 and 5 values, all in the
UV half) and p8_gif `pk_uv_xyzf2_fog` (1); one Gouraud colour value in
p8_gif `pk_junk_adc_ad_nop`.

**Model totals.** 703 of the 906 tests of the 30 batches bit for bit, in
strict mode, with no refusal and no span fault. The other 203 are recorded
with exact counts: 1,874 colour and 16,569 Z values (port
`tools/test_gs_raster_reference.py`, `docs/GS_EXACT.md` sections 7-8).

**Provenance note (from the review).** `FREEZE_VRAM` = 425, the header
size before local memory in `gs.bin`, predates B16 (`tools/gs_vram.py`,
`tools/c7cap_partb.py`); an earlier session (workflow wf_b95432db,
2026-09-26) found it by reading PCSX2's save-state code. It is a container
offset, not GS behaviour; every upload and fence page of all 30 batches
decodes exactly at that offset.

### 9.3 The emulator ini for these captures

Three capture runs (p8_span / p8_class / p8_misc / p8_gif; p8_more;
p8_gif2), each with the section 8.3 procedure and its own pre folder
(`build/b16/gscap8/pre`, `pre2`, `pre3`: the live ini, `playtime.dat`,
both memory cards, the unused `portable-data/inis/PCSX2.ini`,
`sha256.txt`, a listing of the save-state folder; `sha256_after.txt`
after the restore).
- Line 220 read `Renderer = 17` and was set to 13; the diff against the
  pre copy showed only that line. After each run ("no emulator process
  left: True") it was set back to 17.
- After each run the live ini differed from its pre copy only in the
  `[GameListTableView] HeaderState` line, which PCSX2 rewrites on exit.
- Both memory cards (757d6f77..., 47ebe237...) and the unused ini
  (957dc2ef...) kept their sha256; `playtime.dat` changed (the play-time
  counter).
- The save-state folder kept the same 12 user slots.

At the end, `Renderer = 17` is in place and no PCSX2 process is running.
