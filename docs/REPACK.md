# Lossless disc and archive rebuilding

`tools/repack/` rebuilds the user's own SCUS-97112 disc from loose local
files. It leaves the legacy extractor and the native-port export paths
unchanged. Generated manifests, padding, templates, payloads, and images
are disc-derived: keep the entire working tree under ignored `build/repack/`
and never publish it.

## Reference image

The 2026-10-07 audit checked the user's original image against the supplied
`Extermination-rebuilt.iso` before starting. Both are 2,060,386,304 bytes.
They are **not identical**:

| Image | SHA-256 |
|---|---|
| Original user disc | SHA-256 `b6fdb617d2438d3bf1163553e46760365f056e5428030550943142f9c5cc6efe` |
| Supplied rebuilt image | SHA-256 `15dd34d0105a8bc7a0605ec1afe844dfdfff2bb022bf1042614081cf7b9b0d33` |

All 1,246,308 differing bytes are within `SCUS_971.12`. All other file
contents, filesystem metadata, inter-file padding, and unused disc bytes
match. The embedded boot files have different ELF packaging: the original
primary load segment starts at file offset 768 and has 1,530,624 bytes;
the rebuilt one starts at 128 and has 1,530,752 bytes. The original-length
load content is identical after aligning those file offsets. This corrects
the previous unconditional ISO-equality statement in MODDING.md without
changing the parallel decomp build.
Local receipts are `build/repack/identity.json` and
`build/repack/identity_differences.json`; the complete per-file comparison
and ELF header measurements are in `build/repack/identity_comparison.json`.

Use the **original** image for the canonical round trip. Unpacking the
supplied rebuilt image also preserves it exactly, including its different
boot ELF.

## Commands

The archive, ISO and asset editors use standard-library Python on **native
arm64 macOS** or Linux (Python 3.10+). `build-disc` additionally uses the
existing project virtual environment, compiler installations and Apple
`container` image `exterm-permuter` (x86_64 Linux); `proof-title` uses the
local Apple Silicon PCSX2 build. Neither editor requires Pillow or Rosetta.
Run from the repack checkout:

```sh
cd /Users/abe/Documents/Extermination.nosync/Extermination-repack
export PYTHONDONTWRITEBYTECODE=1

python3 -m tools.repack inventory \
  --iso '/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
  --out build/repack/inventory.json

python3 -m tools.repack unpack-disc \
  --iso '/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
  --out build/repack/loose

# Edit native payloads under build/repack/loose/archive/.
python3 -m tools.repack pack-disc \
  --tree build/repack/loose --out build/repack/packed

shasum -a 256 '/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
  build/repack/packed/Extermination.iso
```

An unchanged tree must produce equal hashes. Packing uses the loose files
and preserved layout material, without reopening the original ISO.
Extraction destinations must be empty, and outputs must be inside this
checkout's `build/repack/`. Keep pack output separate from the loose tree.
Input symlinks are read only; never run the older in-place ISO patcher
against the worktree's `Extermination-rebuilt.iso` link.

The separate archive commands are:

```sh
python3 -m tools.repack unpack \
  --data build/repack/loose/iso/files/DATA/DATA.DAT \
  --index build/repack/loose/iso/files/DATA/INDEX.IDX \
  --out build/repack/archive-only
python3 -m tools.repack pack \
  --tree build/repack/archive-only --out build/repack/archive-packed
python3 -m tools.repack pack-iso \
  --tree build/repack/loose/iso --out build/repack/edited.iso \
  --data build/repack/archive-packed/DATA.DAT \
  --index build/repack/archive-packed/INDEX.IDX
```

`unpack-iso --iso IMAGE --out TREE` also exposes every disc file without
unpacking DATA/INDEX. `pack-iso --tree TREE --out IMAGE` rebuilds that tree.
`pack-disc` always takes DATA/INDEX from the archive tree; edit other disc
files in `loose/iso/files/`. It preserves the loose boot and overlay files.
Use `build-disc` below to compile and link all executables afresh.

## Archive format and corrected labels

INDEX contains 56 sectors of 2,048 bytes. Each contains a top descriptor;
nested descriptors start at sector `+0x100`, stride `0x70`. There are 85
descriptors including zero-length ones, and 81 nonempty DATA regions.
They cover all 246,398,976 DATA bytes, in physical offset order. The corrected
loose tree contains **674 spans**: 583 resident files, 35 sound sections,
and 56 transient upload sections. The original has no inter-region gaps.

All integers below are little endian. A top descriptor's tables must fit
before its nested descriptors, and each nested descriptor's tables must
fit within its own `0x70` bytes.

| Offset | Meaning |
|---|---|
| `+0x00` | descriptor ID, preserved |
| `+0x04` | absolute byte offset in DATA |
| `+0x08` | whole region length |
| `+0x0c` | first upload group count, u16 |
| `+0x0e` | second upload group count, u16 |
| `+0x10` | resident upload group count, u32 |
| `+0x14` | resident data start relative to the region |
| `+0x18` | nested descriptor count |
| `+0x1c` | resident file/relocation count |
| `+0x20` | upload records: `(offset, size)`, eight bytes each |
| after all upload records | four-byte resident entries: low 24 bits offset, high byte role ID |

The legacy extractor interprets counts as flag bits and cuts resident
entries from the region start. That is not the loader's addressing rule.
The correct byte address is:

```text
DATA address = descriptor absolute offset + resident start + entry offset
```

This affects flat blocks as well as nested blocks whenever `+0x14` is
nonzero. The last resident file ends at the region end, not another full
region length after the resident base. An ID denotes an asset role, not a
universal format. The repacker labels resident leaves by these corrected
IDs and names upload sections separately. It does not rename anything
under the legacy `extract/` tree.

Evidence: decomp `docs/HANDOFF.md` (systemic extraction label shift), port
`docs/AREA22_ASSETS.md` and `docs/AREA06_ASSETS.md`, and the loader behavior
at `func_001FFCD0`, `func_00200360`, `func_001FF590`, and `func_00200780`.
No source or exporter changes are needed for this independent tool.

## Manifest schema and preserved layout

The archive tree contains `manifest.json`, `index.template.bin`, and
directories such as `chunk10`, `chunk10.n0`, and `chunk26`. Resident leaf
names retain their table ordinal and role ID (`f00_id43.bin`); sound and
transient uploads use `soundNN.bin` and `transientNN.bin`. An unlabelled
resident range uses `resident.bin`. Gaps, if present in a supported input,
get explicit padding files. The payload span includes any internal padding:
the unpacker never guesses that trailing zero bytes are expendable.

| Archive manifest field | Meaning |
|---|---|
| `schema`, `sector_size` | `extermination-archive-v1`, 2048 |
| `index_template`, `index_sha256`, `index_size` | preserved complete INDEX and integrity check |
| `data_sha256`, `data_size` | original DATA identity |
| `source_paths` | original input paths, used only to refuse overwriting them |
| `regions[]` | descriptor location, sector/nested ordinal, ID, original offset/size, resident start, all counts |
| `regions[].sections[]` | upload kind, table location, stored offset, effective span, size, offset interpretation |
| `regions[].entries[]` | resident ordinal, role ID, resident-relative offset, table location |
| `regions[].files[]` | relative loose path, kind, block-relative offset, size, original SHA-256 |
| `data_layout[]` | physical region order and any separately preserved gaps |

The packer reparses the preserved INDEX and checks manifest structure
against it; edit payloads, not the descriptor template or manifest. Unknown
descriptor bytes and unused table space remain exact. Empty descriptors
keep their boundary anchors when preceding data changes size.

Resident DMA sections describe bytes already in resident leaves, so they
are not duplicated into a second editable file. On this disc, chunk27's
single resident DMA section aliases `f00_id35.bin`; resizing that leaf also
updates the section length. If a section boundary falls inside a resized
leaf, packing rejects the ambiguous edit.

The ISO tree contains `manifest.json`, `files/<disc path>`, and preserved
`metadata/*.bin` spans. Its schema is `extermination-iso-v1`. It records
`image_size`, `image_sha256`, `volume_sectors`, `pvd_offset`, volume
descriptors, all path tables and directories, and `files[]` with paths,
classification, extent, length, directory-record locations, sector capacity,
padding description, and hash. `spans[]` records the remaining byte ranges
with paths, lengths, and hashes. Together files and spans partition every
original image byte. Metadata spans are immutable, hash-checked inputs.
The `udf` object records the bridge revision, physical partition, both
partition descriptor locations, integrity descriptor, anchors, file-entry
and allocation-record locations, and protected metadata ranges.
The `source_image` path is retained only to prevent accidental overwrite;
packing does not read that image's bytes. Original DATA/INDEX files may
also be absent when packing their extracted tree.

See [REPACK_INVENTORY.md](REPACK_INVENTORY.md) for every disc file, its
classification and extent, padding observations, and all ISO9660 tables.
Files remain at their original extents when their new length fits their
sector allocation. A DATA/INDEX file that grows past that allocation is
appended at a sector boundary. The packer patches both endian copies of its
directory extent and byte length and the primary volume's sector count.
Directories and path tables do not move, so their counts and pointers do
not change. This disc also has a UDF bridge: resizing must update its file
entries, allocation descriptors, partition sizes, integrity size table,
checksums and trailing anchor, as described in the inventory document.
Only DATA/INDEX may change size in the ISO writer; other files
may be replaced with equal-length content. Old vacated payload space is
zeroed on relocation, while unrelated files and preserved metadata stay
in place. The output is checked structurally and each file is hashed
against its loose input before atomic publication.

## Editing boundaries

Same-size byte edits preserve placement. A changed payload byte changes
that DATA byte; INDEX and unrelated bytes remain unchanged. No-op packing
retains descriptor slack and padding verbatim, including nonzero bytes.

Resident leaves and upload sections may grow or shrink by **individual bytes**.
The packer aligns each physical region, upload start and resident base to 2,048
bytes, inserting explicit zero padding outside asset spans. Region lengths and
resident relocations retain exact byte lengths; a second unpack recovers the
edited payload exactly. All subsequent absolute offsets, resident starts,
section offsets/sizes and empty-descriptor anchors are updated. Existing padding
bytes remain in order; padding files can expand when they absorb new alignment.
An interior DMA boundary within a resized leaf is ambiguous and is rejected.
Container byte alignment alone does not prove a valid game asset: preserve any
4/16-byte alignment, internal pointers and memory-budget requirements of the
asset being edited and the leaves that follow it.

`add-entry` appends a resident role to an existing region, updates its file count
at `+0x1c`, and writes a new offset/ID relocation. The new word must fit the
fixed INDEX descriptor slot and overwrite only zero slack. Duplicate role IDs,
removal, new descriptors, new upload groups, and recursive nesting are not
supported. An entry being structurally valid does not bind it to game logic;
the corresponding runtime consumer still needs to recognize its role ID.

```sh
python3 -m tools.repack add-entry --tree build/repack/loose/archive \
  --region chunk04.n0 --id 0xfe --input build/repack/my-native-asset.bin
python3 -m tools.repack pack --tree build/repack/loose/archive \
  --out build/repack/with-new-entry
```

The original manifest stays immutable. Additions live in `edits.json` with schema
`extermination-archive-edits-v1` and ordered `{region, id, path}` records. Use the
command to populate them; the packer independently validates every field and the
available original table slack. Re-unpacking produces an ordinary complete tree
that needs no edit metadata and packs identically again.

An edit must also fit the 24-bit resident offset and 32-bit container fields.
Within the disc's UDF bridge, a resized DATA or INDEX must remain below
1 GiB to fit its single recorded short allocation descriptor. Other UDF
allocation schemes, alternate ISO namespaces, file aliases, multi-extent
ISO records, and changes to directory topology are rejected. The original
1 GiB dummy file has an unusual UDF length encoding; it is preserved exactly
and is never resized.

Payload formats inside leaves remain the editor's responsibility. Native
model, collision, sound-bank, DMA, and script data can contain their own
lengths, offsets, or runtime memory constraints. Container relocation does
not fix those internal structures or prove game compatibility. Stream movies and cue-based music/voice files remain intact. The supported
editable formats and real-game proof are described below. Model inverse
conversion remains future work: it needs native packet topology, material/TEX0
references, VIF/VU uploads, vertex quantization, skinning/bone bindings and all
internal relocation/size fields, plus original-game validation. A glTF or EMDL
export alone does not retain everything needed to regenerate those bytes.

## One-command build from source

After `unpack-disc`, run on **native arm64 macOS**, with the Apple container
service running (`container system start` if stopped) and the existing toolchain
installed in the sibling main checkout:

```sh
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m tools.repack build-disc \
  --tree build/repack/loose --out build/repack/source-built --require-original
```

A fresh run uses a new output directory. It acquires the shared main-checkout
`build/.decomp_build.lock` with a 10-second retry and releases it in `finally`.
All compiler inputs are staged beneath the output's `source/workspace/`; all
build writes stay there. The main checkout, its index, and its existing build
products are not modified. `--toolchain-root PATH` can select the local compiler
installation. The lock remains the shared project lock.

The source driver uses `tools/decomp/build.py`'s unit selection, exact per-file
compiler/flags and relocation injection, with four bounded compiler workers.
It runs the canonical linker and provenance audit. The overlay driver compiles
eligible source and freshly links all 19 overlays. Missing compiler objects or
failed compiler commands fail the build before filler can conceal them.
`source/source-build.json`, `source/boot-provenance.json`, and local stage logs
record the inputs, objects, linking routes and output hashes. The overlay receipt
separates selected compiled objects from compiled objects bypassed by the canonical
split/link selection. This checkout compiles 443 overlay units; 16 renamed units
are bypassed in favor of the canonical original-assembly pieces. Near misses are
also explicitly retained as original assembly.

Fresh splitting renames one AREA04 function while a compiled caller retains its
old symbol. The driver restores that alias only when the tracked linker assignment
and a fresh split function boundary independently corroborate its address. The
repair affects generated staging files only; all overlay bytes are still checked.

If a link fails after successful compilation, `--resume` validates the retained
source/tool snapshot, original executable inputs, compiler receipts and object
hash checkpoint before relinking all overlays. Other failures require a fresh
output directory. The initial local recovery predates object checkpoints; its
receipt explicitly distinguishes the validated present objects from a historical
hash proof. Subsequent fresh builds always write that checkpoint.

This is the current matching-decomp build, **not an all-C build**: unmatched
functions, near misses, data, and overlay headers still use the user's original
locally split assembly/data. Boot and overlay outputs are freshly linked; the
ISO writer never substitutes the original executables for a failed build.

The old rebuilt ISO's discrepancy was ELF packaging. The canonical linker emits
128 extra zero alignment bytes after the original 1,530,624-byte LOAD range.
The new packager verifies the entire original-length fresh LOAD, rejects short,
nonzero-tail or differently addressed output, and removes only the verified
128-byte zero tail. It places those fresh linked bytes inside the original
2,000-byte nonloaded ELF envelope (headers, metadata and padding). The original
boot file is then byte-identical without replacing compiled code with a copy.
Overlay payloads similarly come from the fresh links, with their native headers.
Current source builds require matching executable bytes; intentionally changing
game code is outside this matching-only command.

`build-disc` then packs the loose archive and whole ISO, including both ISO9660
and UDF metadata. `build-disc.json` records the final SHA-256. `--require-original`
fails if that hash differs from the unpacked original reference. Omit that flag
when building edited assets.

The 2026-10-08 completed source build produced the original 2,060,386,304-byte
image exactly. An independent test rehashed both the user-owned original ISO
and the actual rebuilt ISO: SHA-256
`b6fdb617d2438d3bf1163553e46760365f056e5428030550943142f9c5cc6efe`.
Its local receipt is `build/repack/source-built/build-disc.json`; the hash test
log is `build/repack/source-full-test.log`. It freshly compiled 2,214 boot units
and 443 overlay units, then verified the complete boot and all 19 overlays.
The boot provenance has 1,664 ordinary-C functions, 482 inline-assembly functions
and 864 retained original-assembly functions, including 58 missing-source slots.
Of the compiled overlay objects, 427 are selected and 16 are bypassed as described
above. These counts describe the local snapshot, not a promise about later source
revisions.

## Editable native formats

Each decoder retains a hash-checked native template and layout manifest next to
its standard files. Pack to a separate native output, verify it, then copy it
into the loose archive. Do not edit templates or manifests.

| Native content | Standard view | Supported edits and limits |
|---|---|---|
| Bounded GS texture uploads | PNG index sheets, RGBA textures and palette swatches | PSMT8/PSMT4 and CSM1 CLUTs; physical upload canvases can resize; logical TEX0 dimensions and palette counts stay fixed |
| SShd sample banks; mono raw ADPCM or VAGp | Mono PCM16 WAV and `loops.json` | Longer/shorter samples, relocated tone starts and bank sizes, explicit loop starts; fixed preview rate and SPU budget |
| Message-bank and bare OUTER/TEXT tables | JSON `table.json` | Longer/shorter strings, relocated style anchors and extents; native renderer scratch limits enforced |

### Textures

```sh
python3 -m tools.repack texture-unpack \
  --input build/repack/loose/archive/chunk01/transient00.bin \
  --out build/repack/title --preset title
# Edit a textureNNN.png or paletteNNN.png in a PNG editor.
python3 -m tools.repack texture-pack --tree build/repack/title \
  --out build/repack/title-edited.bin
cp build/repack/title-edited.bin build/repack/loose/archive/chunk01/transient00.bin
```

Without a preset, every upload becomes `uploadNNN.indices.png`: grayscale values
are native indices, not guessed colors. This reversible index view covers all
113 observed transfers in 63 loose leaves. `--tex0 0x...` (repeatable) adds a
specific audited PSMT8/PSMT4 view. `--preset title|warning|logos` uses the startup
TEX0s and display orientation decoded in `tools/export_startup.py`. The title
preset's `palette004.png` controls the selected NEW GAME label.

Explicit TEX0 views currently require PSMCT32 CSM1/CSA0 palettes and an even
texture-buffer width. CSM1 needs both the index-bit permutation and the PSMCT32 address mapping.
Packing applies the inverse byte/nibble mapping, preserves unedited duplicate
indices and native alpha bytes, and rejects contradictory overlapping PNG edits.
Alpha represents PS2 alpha doubled and clamped to 255. `--quantize exact` (the
default) rejects unrepresentable edited colors/alpha; `--quantize nearest` opts
into palette matching and reports the number of quantized pixels and largest
channel error. Edit a palette swatch to introduce new colors. Unchanged files
remain byte-identical even where several native values decode to the same color.
Supported PNG input is noninterlaced 8-bit grayscale, grayscale-alpha, RGB/RGBA or indexed PNG;
unsupported bit depths and interlacing are rejected.

### Texture size changes

For an **index-only** tree exported without `--preset` or `--tex0`, change an
`uploadNNN.indices.png` canvas and opt in:

```sh
python3 -m tools.repack texture-pack --tree build/repack/index-uploads \
  --out build/repack/resized-upload.bin --resize-uploads
```

This resizes the physical PSMCT32 upload canvas. It reconstructs the contiguous
DMA/VIF/GIF envelope, DMA QWC, VIF DIRECT count, IMAGE NLOOP, BITBLTBUF DBW,
TRXREG dimensions and subsequent packet positions. Only the characterized page
geometry and zero trailing padding are accepted. GIF's 15-bit IMAGE count,
DMA/VIF's 16-bit counts, 4 MiB GS addressing and new intra-leaf overlaps are checked.
The old logical texture references are unchanged: adding unused atlas space
alone does not make a model use a higher-resolution image.

Logical TEX0 dimensions and 16↔256 palette counts are explicitly refused.
Actor variants, other uploads, embedded models and executable constants can
share the same GS storage; their complete reference set is not yet established.
For startup UI, `func_00207E40` derives UV extent from TEX0, while `func_001ABF90`
and `func_001AC7F0` use fixed screen sizes. A logical resize could scale correctly
if the inline executable TEX0s were patched, but this asset-only editor cannot
safely close those references. Palette colours remain editable at their existing
count. This limitation is about missing reference relocation, not an inherent
GS ban on other dimensions.

All 63 real leaves / 113 transfers retain exact bytes on no-op. A startup canvas
512×768 was grown to 512×832 and shrunk to 512×704; a two-transfer leaf also
relocated its second packet. Every edited pixel decoded through the existing
`extract_subtextures` implementation. Receipt:
`build/repack/size-formats/texture/receipt.json`. These canvas edits have extractor
proof; they are not advertised as runtime texture-resolution upgrades.

### Audio

```sh
python3 -m tools.repack audio-unpack \
  --input build/repack/loose/archive/chunk04.n0/sound00.bin \
  --out build/repack/sound --rate 48000
# Replace an existing bankNN/sampleNNNN.wav with longer/shorter mono PCM16.
# Keep the exported rate; edit loops.json when its loop start needs to move.
python3 -m tools.repack audio-pack --tree build/repack/sound \
  --out build/repack/sound-edited.bin
cp build/repack/sound-edited.bin build/repack/loose/archive/chunk04.n0/sound00.bin
```

`--kind auto|sshd|raw|vag` selects the input form. SShd and raw clips use the
chosen preview rate (runtime pitch/tone metadata stays intact); VAGp supplies its
rate. Each 16-byte native frame represents 28 samples. Unchanged decoded PCM
reuses the exact original encoded bytes; WAV header-only changes do not trigger
re-encoding. Edited PCM uses deterministic predictor/shift selection; duration
rounds up to 28 samples with zero padding. `loops.json` maps sample paths to a
loop-start ADPCM frame index, `null` for one-shot, or `"end"` for a terminal-frame
loop. The loop end follows the new final frame. An unchanged clip retains its
exact original flags; edited loop-start frames reset the predictor. Re-encoding
is lossy. Whole MUSIC/VOICE cue files belong in the stream editor, not raw mode.

SShd packing relocates every tone start in both program tables (u16 addresses
in eight-byte units), both bank-size copies, following bank bodies, and the
container/image lengths. VAGp's big-endian payload length is also updated.
Unknown header bytes and terminal padding remain exact. Independently keyed
interior sample aliases cannot be re-encoded safely and are refused. A one-frame
looping bank sample is ambiguous with a terminator and is refused.

The original loaders allocate equal-type banks consecutively with 64-byte
alignment, at SPU byte bases `0x15040`, `0x1A0000`, `0x122000`, `0x132000` for
groups 1–4. Tone addressing limits an individual bank to 512 KiB. Growth cannot
cross the next group or reverb storage. `func_001FB210` initializes mode 4, but
`func_00118828` sequence event 15 can change it through `func_00119810`.
Executing the original driver and resident LIBSD for all modes 0–9 measured a
maximum reservation of `0x18040` bytes (modes 7/8); new allocations in groups
2/4 therefore stop at `0x1E7FC0` / `0x187FC0`. Existing larger occupied spans
are preserved, with no growth beyond their old endpoint. This is a conservative
allocation guard, not a promise that every sound replacement suits every scene.
The 24-bit IOP upload-size field is subsumed by these smaller limits.

Local evidence: `build/repack/audio-growth/reverb-modes.json` records the original
oracle, and `duration-proof.json` records real growth and shrinkage decoded by
the existing audio extractor. All 41 containers / 116 banks / 2,318 samples keep
identical native bytes on an unchanged round trip.

### Message tables

```sh
python3 -m tools.repack table-unpack \
  --input build/repack/loose/archive/chunk00/f02_id02.bin \
  --out build/repack/messages
# Edit text fields; for styled lines, edit segments to move the style anchors.
python3 -m tools.repack table-pack --tree build/repack/messages \
  --out build/repack/messages-edited.bin
cp build/repack/messages-edited.bin build/repack/loose/archive/chunk00/f02_id02.bin
```

The bank's 9 groups contain 271 lines; `chunk03/f14_id16.bin` is a bare OUTER
with 54 lines. `--kind auto|bank|outer` selects the layout. Latin-1 in JSON is a
one-to-one native byte mapping, not a Unicode font promise. High-bit glyph bytes
remain escaped and unchanged unless explicitly edited. Longer and shorter text
is supported. NULs, count changes and non-Latin-1 characters are rejected.
Styled lines expose `segments`: change those strings, leaving their count intact,
to relocate every control record's glyph anchor at record +8. Conflicting edits
to `text` and `segments`, unsupported anchors, and split/dangling two-byte glyph
escapes are refused. No-op JSON output remains byte-identical.

Packing rewrites both copies of every TEXT string offset, native and terminated
lengths, the total string size, bank group offsets and offset/16 fields, logical
and padded lengths, and total payload size. Counts stay fixed and validated.
Opaque control words stay intact. `func_001FC7B0` and `func_001FE070` use 0x80-byte
scratch buffers: edited runs must leave room for NUL (127 rendered bytes).
Direct text callers are checked across style anchors too. Group 8's options
substitution through `func_001FCBD0` has its own 128-byte destination check.
The renderer `func_001CC1E0` flushes a 512-pixel staging strip; that is not a
universal line-width cap. Screen fit still depends on glyph advances and each
caller's layout. The `D_00820ED0[64]` decomp declaration is not evidence of a
64-byte engine limit.

Real longer/shorter and styled edits re-extract through `tools/export_ui.py`:
271 bank lines plus 54 bare OUTER lines. Receipt:
`build/repack/size-formats/text/receipt.json`.
The bank payload-size field excludes the directory base. OUTER markup offsets
are relative to `OUTER + OUTER[0]`, rather than to the OUTER header itself.
Both details are checked against the real tables; padding remains exact.

## Original-game proof

The `proof-title` command cold-boots the ISO itself, without a boot-ELF override
or preloaded VRAM state, through `tools/pcsx2_session.py`:

```sh
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m tools.repack proof-title \
  --iso build/repack/source-built/Extermination.iso --out build/repack/title-proof
```

It acquires the shared `.pcsx2.lock` with 30-second retries, launches hidden,
and confirms emulator shutdown or reports failure; the lock is always released. A private copied emulator
and configuration under the output directory disable every memory-card slot.
The screenshot comes from private slot 16; main-checkout slots 01–15 are hashed
before/after and never loaded or overwritten. No reference emulator installation
is moved or modified. The private card directory must remain empty.

The local mod proof changes the selected NEW GAME palette to magenta. It edits
`palette004.png`, encodes it back into `chunk01/transient00.bin`, and runs the
whole `build-disc` command afresh from an empty output directory. All 44 source
stages pass, with no resume; the boot and 19 overlays remain byte-identical.
Only 43 bytes differ anywhere in the 2,060,386,304-byte disc, precisely the
expected palette bytes. The modified ISO has SHA-256
`bbec53f7567380abc5c1c797d5d6bc9cecc85f9737296a792a437f85ca190b7b`.
`build/repack/source-mod-compare.json` records the exact offsets, actual-original
hash, fresh build provenance, and unchanged canonical loose leaf.

Hidden cold boots of the original and the source-built mod both reached the
interactive title menu at frame 1,410. The 640×480 screenshot comparison changes
2,895 pixels, confined to the NEW GAME label at x=214..421, y=340..354. It changes from cyan to magenta; the rest of the screenshot
is identical. Local screenshots are `build/repack/proof-before/title.png` and
`build/repack/proof-after/title.png`. `build/repack/mod-proof.json` records both
image identities, the screenshot comparison, all 674 canonical archive leaves
and 43 canonical ISO files unchanged, disabled/empty memory cards, and all 12
existing protected save states unchanged. Native process inspection confirmed no
PCSX2 process remained and both shared locks were released.

After capture, the isolated mod tree, temporary modified ISO, compiler workspaces,
duplicate containers and private emulator copies were removed. The original loose
tree, screenshot pair, format edits and audit receipts remain under `build/repack/`.
The duplicate baseline ISO was removed after the follow-up cold New Game proof;
its byte-identity receipt remains and the build command regenerates it. `cleanup.json` lists the removed
scratch paths. No canonical asset needed restoration because the mod used a
separate tree; regenerate an edited disc with the walkthrough when needed.

## Verification

```sh
# Quick synthetic format, mutation, resize, and validation checks.
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m unittest discover \
  -s tools/repack -p 'test_*.py'

# Full local-disc archive/ISO, texture, audio and table proofs.
PYTHONDONTWRITEBYTECODE=1 EM_TEST_FULL=1 \
EM_TEST_ISO='/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
  .venv/bin/python -m unittest discover -s tools/repack -p 'test_*.py'
```

The source-build test is deliberately a separate opt-in because it freshly
compiles and links the full game:

```sh
PYTHONDONTWRITEBYTECODE=1 EM_TEST_FULL=1 EM_TEST_SOURCE_BUILD=1 \
EM_REPACK_TREE=build/repack/loose \
  .venv/bin/python -m unittest tools.repack.test_source_build
```

To validate a completed fresh build against the actual original ISO without
compiling it again:

```sh
PYTHONDONTWRITEBYTECODE=1 EM_TEST_FULL=1 \
EM_SOURCE_BUILD_OUTPUT=build/repack/source-built \
  .venv/bin/python -m unittest tools.repack.test_source_build
```

The independent UDF test oracle uses `pycdlib` in the existing project
virtual environment. The repacker itself has no external dependencies.
The full suite defaults to `Extermination-rebuilt.iso` when `EM_TEST_ISO`
is omitted; set it explicitly as above to check the original image.

The 2026-10-08 original-disc integration proof passed in 53.128 seconds,
including a one-byte control, +37/-13-byte resident resizing, addition of a new
resident role, exact second-generation archive repacking, and independent UDF
reads. All 674 existing asset leaves remain unchanged except each selected edit.
The quick suite currently has 69 passes and 7 intentionally gated integration
checks (about one second on this machine).

| Format | Local proof |
|---|---|
| GS texture uploads | 63 native leaves / 113 transfers unchanged after PNG round trip; edited PSMT8/PSMT4 textures and CLUTs decode through the existing startup exporter |
| Audio | 41 SShd leaves / 116 banks / 2,318 clips unchanged after WAV round trip, including corrected title bank `chunk01/f00_id06.bin`; a real silence edit decodes correctly through the old decoder |
| Message tables | 271 bank lines and 54 bare OUTER lines unchanged after JSON round trip; isolated equal-length edits re-extract correctly |
| Archive growth | +37 bytes, -13 bytes and a new resident entry; every other asset unchanged, second pack identical, ISO/UDF namespaces agree |

The per-format receipts are `build/repack/texture-full.json`,
`audio-test-receipt.json`, `audio-edit-test-receipt.json`, and
`tables-test-receipt.json`. They contain local hashes, not shipped asset bytes.
Original and no-op rebuilt hashes are equal:

| Output | SHA-256 (original and rebuilt) |
|---|---|
| DATA.DAT | SHA-256 `b6dc43cf8c75dce4c980211f12e11dde61fb280206642c237af729e518e6a323` |
| INDEX.IDX | SHA-256 `bed4390e7686bf2c46d4ab81fcbec4c9c42d2604ffd4d9bee39647ca43b398fc` |
| Full original ISO | SHA-256 `b6fdb617d2438d3bf1163553e46760365f056e5428030550943142f9c5cc6efe` |

`build/repack/test-receipt.json` records per-file original/packed hashes
for all 43 ISO files, their independent UDF reads, and all 674 loose archive
spans. The single-byte control changed only DATA byte 7,393,280 and ISO
byte 1,082,101,760; INDEX stayed identical. Growing and shrinking
`chunk04.n0/f00_id43.bin` by +37 and -13 bytes unpacked to the exact edited
tree, with all 673 other loose files unchanged and all unrelated ISO/UDF
files unchanged. The earlier `build/repack/archive-controls.json` additionally
records whole-sector sound-upload growth, resident-DMA shrinkage and empty-descriptor
anchor relocation. Quick synthetic checks cover those table types,
malformed inputs, source aliases, and false UDF anchor detection.

The tests remove their large intermediate images and loose trees; receipts
remain. Regenerate an editable tree and deliverable ISO with the commands
above. PCSX2 proof outputs stay under `build/repack/` too. No game files under
`src/`, legacy extractors, native-port files, main emulator configuration or
protected save states are changed by this workflow.

Only original tooling, synthetic test builders, and documentation belong
in commits. Before each commit, inspect the staged paths and run:

```sh
python3 tools/check_no_disassembly.py --staged
```
