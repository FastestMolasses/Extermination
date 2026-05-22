# Extermination — Reverse-Engineering Findings

Technical reference for formats and facts established so far. The authoritative,
exhaustive format details live in the docstrings of the `tools/` scripts; this
file summarises them and records findings that have no other home.

_Last updated: 2026-05-22_

## Target identity

Pinned in `CLAUDE.md`. In short: **SCUS-97112** (USA), boot ELF `SCUS_971.12`,
compiled with **Metrowerks CodeWarrior** (`mwccps2` 2.3.1.01), **no DWARF and
no symbol table** — blind matching, the hardest tier.

## Disc layout

| Path | What it is |
|---|---|
| `SYSTEM.CNF` | boot config, names the boot ELF |
| `SCUS_971.12` | boot executable (EE MIPS) |
| `EXTER.BIN` | exactly 1 GiB — **disc padding/dummy** (`"EXTERMINATION"` + `0xAA` filler). Ignore. |
| `EXTER1.DAT` | an MPEG-PS video (FMV) |
| `DATA/DATA.DAT` + `DATA/INDEX.IDX` | the main asset archive (see below) |
| `OVERLAY/AREA*.BIN` | runtime code overlays, `MWo3` (Metrowerks) format, ~19 files |
| `MOVIE/*.PSS` | cutscene FMVs (Sony PSS / MPEG-PS) |
| `STREAM/MUSIC.DAT`, `STREAM/VOICE.DAT` | streamed music and dialogue |
| `IRX/*` | IOP-side modules (mostly stock Sony SDK) |

## `DATA.DAT` / `INDEX.IDX` archive

Two-level container, fully validated by exact tiling. Tool: `tools/extract_data.py`.

- `INDEX.IDX` = N sectors of `0x800`; sector *i* is the descriptor for chunk *i*
  (`{id, off, size, flags, ...}`), plus nested sub-chunk blocks of `0x70` bytes
  at `+0x100`, plus a file-entry table (`[u24 offset, u8 file_id]`).
- Top-level chunks + nested sub-chunks form **81 leaf regions** that tile
  `DATA.DAT` exactly; **603 files** tile every region exactly.
- Full layout: see the `extract_data.py` module docstring.

### Asset taxonomy

`file_id` is an **asset-role tag**, not a format tag — content varies by region.
Observed content types: SShd sound banks, float-array geometry/model data,
`07 XX 00 60` GS texture packets, ASCII text (`id 0x80`), tables/padding. A
region bundles one logical entity's pieces (geometry + textures + audio).

## Textures

Textures are PS2 **GS texture-upload packets** in `DATA.DAT` — 28 of them, each
a file beginning `07 XX 00 60`. Tool: `tools/extract_textures.py`.

- The packet is GIF/DMA data: GS register writes (`BITBLTBUF` 0x50, `TRXPOS`
  0x51, `TRXREG` 0x52, `TRXDIR` 0x53) then an IMAGE-mode GIF tag with the payload.
- `TRXREG` gives the transfer region `w x h` in PSMCT32 (32-bit). The real
  texture is **8-bit indexed (PSMT8)**, dimensions `(w*2) x (h*2)` — the
  PSMCT32-page / PSMT8-page footprint ratio is 2x per axis.
- The index data is in PS2 **swizzled** VRAM order. The tool recovers it with
  the documented GS pipeline — write the PSMCT32 transfer into simulated VRAM,
  read it back as PSMT8, using the hardware page/block/column swizzle tables.
  This was verified **byte-identical** to the standard combined `unswizzle8`,
  so the swizzle is provably correct. Results are 512-wide sheets.
- The extractor also finds packets **embedded** in larger `id 0x44` level
  files (57 packets total, vs 28 standalone). A few embedded results look
  noisy — likely false-positive signature matches.
- **Color source is unresolved.** The format is 8-bit (PSMT8), which on GS
  hardware always samples through a CLUT — so a CLUT exists in VRAM at
  runtime. But no CLUT data was found in `DATA.DAT` or the boot ELF, and a
  smoothness test (adjacent-pixel delta ~7-22, vs ~85 for a random palette)
  shows the 8-bit values are luminance-ordered. So either the runtime CLUT is
  a grayscale ramp (color then comes from renderer vertex-color modulation)
  or there is a luminance-sorted color CLUT not yet located. The `TEX0` setup
  in the decompiled draw code will settle it. The grayscale PNG output is
  faithful to the index data either way.
- Each decoded sheet is a texture **atlas** — many individual textures packed
  into one sheet, some stored flipped/rotated to pack tighter, plus
  non-texture padding. The swizzle is correct (proven above); the sheet just
  *is* a packed atlas. Cutting it into clean, correctly-oriented individual
  textures needs each texture's UV rectangle from the geometry/draw data.

## Audio — VAG ADPCM

All game audio is **PS2 VAG ADPCM** (16-byte frames). Decoder: `tools/decode_sound.py`.

- **SFX**: `SShd` sound-bank containers inside `DATA.DAT` files. A bank holds
  many sounds; each ends on a VAG flag with bit 0 set. `decode_sound.py batch`
  deduplicates across banks → 241 unique sounds.
- **`VOICE.DAT`**: one raw **mono** VAG stream, no container, no end-flags.
  Split into clips on silence gaps. → 116 clips.
- **`MUSIC.DAT`**: one raw **interleaved-stereo** VAG stream — alternating
  **64-frame (1024-byte) L/R blocks**. Deinterleave then decode.
  `decode_sound.py stream --interleave 64`. → 55 stereo tracks.

### Sample rate

Streamed audio plays at **48000 Hz** (PS2 SPU2 native rate). Evidence:
`music/clip_0000` (End Credits) matches an official-soundtrack rip — the rip is
encoded at 48000 Hz, and our clip's frame count / the rip's duration ≈ 48090 Hz.
The rate is **not stored in the audio data** (engine/SPU-pitch parameter).
SFX-bank rate is still unconfirmed (provisionally 22050). Both should be
re-confirmed from the decompiled audio engine.

## Geometry / models

Reverse-engineering **started — characterized, not yet fully decoded.**

3D geometry lives in the large `id 0x44` level files. Observed structure:
- A geometry file is a long run of fixed **0x40-byte (64-byte) records** — one
  ~5.4 MB level file (`chunk04.n0/f06_id44.bin`) holds ~85,000 of them.
- Each record looks like one **vertex**: a marker/id field (`15 42 37 5X`
  followed by a u32) then three vec4s — a UV-like pair (0..1), a unit normal,
  and a world-space position (xyz + w ≈ -1.0). All IEEE-754 floats.

Not yet determined: how vertices group into primitives/strips/meshes, any
index data, material/texture binding, and whether VIF/GIF draw packets wrap
it. This is the next deep task; per-texture extraction depends on it.

## `MUSIC.DAT` track listing

`MUSIC.DAT` decodes to 55 tracks. Per the user (cross-referenced with an online
soundtrack listing): **25 are the official soundtrack; the remaining 30 are
cutscene audio.** Not yet separated or labelled — a roadmap item.
