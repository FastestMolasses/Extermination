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
DATA/INDEX may change size. MUSIC/VOICE may also resize through a verified
`build-disc --stream-bundle` pairing or the strict `stream-edits.json` emitted
by mod-pack installation, with freshly built ELF cue tables. Unpaired loose
stream resizing is refused. Other files may be replaced with equal-length content. Old vacated payload space is
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
Within the disc's UDF bridge, each supported resized file must remain below
1 GiB to fit its single recorded short allocation descriptor. Other UDF
allocation schemes, alternate ISO namespaces, file aliases, multi-extent
ISO records, and changes to directory topology are rejected. The original
1 GiB dummy file has an unusual UDF length encoding; it is preserved exactly
and is never resized.

Payload formats inside leaves remain the editor's responsibility. Native
model, collision, sound-bank, DMA, and script data can contain their own
lengths, offsets, or runtime memory constraints. Container relocation does
not fix those internal structures or prove game compatibility. The bounded
format editors below handle their characterized internal tables. Movies remain
whole streams. Same-topology model edits are supported; arbitrary re-topology
needs the design in [REPACK_MODELS.md](REPACK_MODELS.md).

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
Current source builds require matching executable bytes. An optional verified
stream bundle patches only the three address/length words in existing cue-table
rows after the fresh matching link. Intentional game-code changes remain outside
this command; the original loose ELF stays untouched for canonical compilation.

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
| Bounded GS texture uploads | PNG index sheets, RGBA textures and palette swatches | PSMT8/PSMT4 and CSM1 CLUTs; physical upload canvases can resize; the closed title-menu profile also changes logical dimensions and 16↔256 palette counts with a fresh-ELF compositor patch |
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

All 63 real leaves / 113 transfers retain exact bytes on no-op. A startup canvas
512×768 was grown to 512×832 and shrunk to 512×704; a two-transfer leaf also
relocated its second packet. Every edited pixel decoded through the existing
`extract_subtextures` implementation. Receipt:
`build/repack/size-formats/texture/receipt.json`. These canvas edits have extractor
proof; they are not advertised as runtime texture-resolution upgrades.

### Logical title texture upgrades

`texture-upgrade` supports the closed `title-menu-v1` profile: four background
tiles and the selected/unselected NEW GAME, LOAD GAME and OPTIONS textures in
`archive/chunk01/transient00.bin`. It changes logical TEX0 width/height and
16↔256 palette cardinality, repacks their indices and CSM1 palettes, and records
the executable changes required by the new layout. The ordinary `texture-pack`
command still preserves logical dimensions and palette counts.

The reference set comes from the title control flow. `func_001AC480` requests
module 1 through `func_001FF080`, waits for loading, initializes the selector to
0 or 1 and clamps menu movement to 0..2. The loader path
`001FF0D0 → 001FF830 → 001FF3F0 → 00200830` submits its transient DMA section.
`func_001AC7F0` supplies all four background TEX0s to `func_001ABF90` and all
three current menu TEX0s to `func_00207E40`. That sprite primitive derives the
full UV range from TEX0 TW/TH; the screen rectangles remain 256×256 for background
tiles and 256×128 for menu words. Thus a 512×256 menu image retains its screen
position and aspect. The warning/logo functions `001AB9D0`, `001ABC60` and
`001ABE10` load other modules and use different TEX0s before the title; their
references remain unchanged. Actor variants, embedded models, runtime palettes
and cross-leaf residency do not have this closed reference set and remain
unsupported for logical upgrades.

The original compiler shared TEX0 immediate fragments between menu words, so
independent dimension/format changes cannot be expressed by changing those
immediates alone. After the fresh matching source build, the profile emits an
authored equivalent compositor and ten-entry TEX0 table within the original
**556-byte function span at 0x001AC7F0**. It checks the original function hash
before patching. The table uses that same span, with no new executable or global
allocation. The interpreter compares the original and authored implementations
across all valid selectors and each initialization/return branch: call targets,
arguments, ordered task writes, stack, return address and 128-bit saved registers
must agree except for the intended TEX0s. Calls deliberately clobber volatile
registers in this check. An unchanged export/repack retains the original native
upload and boot bytes exactly.

Run the Python commands natively on arm64 macOS. `build-disc` then uses the
established x86_64 container toolchain for the fresh source build. Start with an
isolated full disc tree and export its editable views:

```sh
python3 -m tools.repack unpack-disc --iso /path/to/owned-original.iso \
  --out build/repack/title-upgrade-tree
python3 -m tools.repack texture-upgrade-views \
  --tree build/repack/title-upgrade-tree --out build/repack/title-upgrade-png
```

The export includes ten named RGBA PNGs and `spec.json`. To upgrade selected
NEW GAME from 256×128/16 colors to 512×256/256 colors, author a 512×256 PNG and
save it as `build/repack/title-upgrade-png/new-game-selected-2x.png`. Dimensions
are read from the PNG, not a separate scale flag. Save this specification as
`build/repack/title-upgrade-png/upgrade.json`; its PNG paths are relative to the
specification:

```json
{
  "profile": "title-menu-v1",
  "views": {
    "background-top-left": {"png": "background-top-left.png", "palette_size": 16},
    "background-top-right": {"png": "background-top-right.png", "palette_size": 16},
    "background-bottom-left": {"png": "background-bottom-left.png", "palette_size": 16},
    "background-bottom-right": {"png": "background-bottom-right.png", "palette_size": 16},
    "new-game-selected": {"png": "new-game-selected-2x.png", "palette_size": 256}
  }
}
```

This example explicitly reduces the four background palettes from 256 to 16
colors to make room inside the original 384 KiB GS footprint, 0x2A0000..0x300000.
It never allocates presumed spare VRAM. Omitted views retain their exact decoded
pixels. Apply the specification, accepting the requested background quantization,
then build the complete disc:

```sh
python3 -m tools.repack texture-upgrade --tree build/repack/title-upgrade-tree \
  --spec build/repack/title-upgrade-png/upgrade.json --quantize median-cut
python3 -m tools.repack build-disc --tree build/repack/title-upgrade-tree \
  --out build/repack/title-upgrade-built
```

Use `build-disc` for this profile: its fresh boot patch is required alongside the
native archive edit. The tool writes `texture-upgrades.json` beside `archive/`
and `iso/`. This strict semantic descriptor contains only the named profile,
dimensions, palette counts and native/decoded hashes. Unknown fields, duplicate
JSON keys, arbitrary patch addresses/opcodes, mismatched data and output symlinks
are rejected. Keep the descriptor with the edited tree; do not edit it by hand.

Dimensions must be powers of two from 1 to 512. The original sprite primitive
`00207E40` writes far UV endpoints as dimension × 16; 1024 becomes 0x4000 and
overflows the GS 14-bit UV component. The profile refuses that hardcoded limit
even when a 1024-wide image would fit VRAM; supporting it needs a separate
reviewed sprite primitive. The allocator reserves whole
GS pages, including for smaller logical images, with an even TBW of at least 2.
The combined layout must fit the original GS footprint. A capacity error reports
the required and available bytes. `--quantize exact` is the default and rejects
images with too many colors or alpha outside even values plus 255; native GS
alpha is 0..128. `median-cut` is an explicit lossy choice and reports changed
pixels. The existing upload geometry, DMA/VIF/GIF counts and file size stay
unchanged because the new logical layout occupies the same physical transfer.

The local proof fixture upgrades NEW GAME to 512×256 with all 256 RGBA colors
used, reduces the four backgrounds to 16 colors and preserves the other five
menu views pixel-for-pixel. The existing `export_startup` decoder reproduces all
ten views, including the authored high-resolution PNG exactly. Ten tests cover
these edits, no-op identity, compositor equivalence and rejection paths; run
`PYTHONDONTWRITEBYTECODE=1 EM_TEST_FULL=1 python3 -m unittest tools.repack.test_texture_upgrade`.
Receipts: `build/repack/texture-upgrade/receipt-256.json` and
`build/repack/texture-upgrade/test-receipt.json`; the final ten-test run is in
`build/repack/texture-upgrade/current-tests.log`. Check the loaded compositor and
all ten sampler dimensions in read-only PINE observations while capturing the
cold title:

```sh
.venv/bin/python -m tools.repack proof-title \
  --iso build/repack/title-upgrade-built/Extermination.iso \
  --texture-tree build/repack/title-upgrade-tree \
  --out build/repack/title-upgrade-proof
```

This uses the existing hidden private emulator, disabled cards, private slot 16,
shared lock and confirmed shutdown. The receipt contains function/table hashes
and decoded sampler fields, not a copied executable dump.

The 2026-10-08 runtime proof applied `title-upgrade.emmod` to the original disc
through another complete 44-stage source build. Its 171,119-byte delta pack
passed the 65-byte scan and reconstructed the intended native leaf exactly.
The installed ISO SHA-256 was
`1fad98b5b78f2d9adc69fad8717d6f773dcb95ca9996548f7a482956465532f2`;
only DATA.DAT and the approved boot function changed. All other archive leaves
and all 43 ISO9660/UDF file comparisons passed. At frame 1,410 the hidden cold
boot reached the interactive title and showed purple selected NEW GAME. PINE
confirmed the complete 556-byte authored compositor and embedded sampler table;
selected NEW GAME was width 512, height 256, PSMT8, TBW 8. The backgrounds visibly
reflect the explicitly requested 16-color reduction needed to fit the arena.
All 12 protected save states stayed unchanged, memory cards stayed disabled,
and the emulator exited. Evidence:

- `build/repack/texture-upgrade/pcsx2/title.png` and `pcsx2/proof.json`.
- `build/repack/texture-upgrade/disc-validation.json` for source and container checks.
- `build/repack/texture-upgrade/modpack-receipt.json` and `title-upgrade.emmod`.
- `build/repack/model-status-baseline/title/original.png` for the original title.

The temporary installed tree, ISO, source workspace and private emulator copy
were removed after validation; the pack, small fixture, screenshot and receipts
remain. The canonical loose tree was never edited.

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

## Music, dialogue and movies

The inventory's 12 stream files are two ADPCM cue banks, nine `MOVIE/*.PSS`
files and the MPEG-PS movie `EXTER1.DAT`. The ten movies are preserved exactly;
only MUSIC/VOICE have inverse audio conversion.

```sh
python3 -m tools.repack stream-unpack \
  --input build/repack/loose/iso/files/STREAM/MUSIC.DAT \
  --elf build/repack/loose/iso/files/SCUS_971.12 \
  --cue 63 --out build/repack/music
python3 -m tools.repack stream-unpack \
  --input build/repack/loose/iso/files/STREAM/VOICE.DAT \
  --elf build/repack/loose/iso/files/SCUS_971.12 \
  --cue 143 --out build/repack/voice
# Edit cue_063.wav / cue_143.wav: PCM16, 48000Hz, existing channel count.
python3 -m tools.repack stream-pack --tree build/repack/music \
  --out build/repack/music-bundle
python3 -m tools.repack stream-pack --tree build/repack/voice \
  --out build/repack/voice-bundle
.venv/bin/python -m tools.repack build-disc --tree build/repack/loose \
  --stream-bundle build/repack/music-bundle \
  --stream-bundle build/repack/voice-bundle --out build/repack/stream-mod
```

Repeat `--cue` to export selected IDs, or omit it to decode every cue. Music has
67 usable stereo cues plus a null row; voice has 178 mono cues plus a null row.
Their ELF tables are at `0x25DD30` / `0x25E170`, with 16-byte rows holding start
sector, start byte, byte length and loop flag. These IDs, counts and flags stay
fixed. Stereo storage alternates 1,024-byte left and right ADPCM blocks; mono
uses contiguous frames. Each 16-byte frame decodes to 28 PCM samples.

Unchanged PCM reuses native bytes exactly, including predictor choices and flags.
Edited cues are deterministically re-encoded, reset the first predictor and pad
to whole 2,048-byte sectors. For non-looping cues, the original `func_001FA790`
duration estimator subtracts half a second. Encoding therefore appends a silent
guard of 31 video ticks after rounding the requested duration up to ticks; the
extra tick covers native float32 truncation. Reported padding includes this guard
and sector rounding, so requested audio can finish before the native timeout.
Looped music bypasses that timer in `func_001F9CF0` and receives only sector
padding. Loop flags remain those of the existing cue; author a suitable boundary
for a looping replacement. This does not add cue IDs or retime scripts/subtitles.

`stream-pack` emits the complete DAT plus `cue-edits.json`, schema
`extermination-stream-bundle-v1`. It relocates every subsequent cue. `build-disc`
checks the original stream identity and boot/table hashes before compiling,
checks the packed stream hash and contiguous rows again against the fresh boot,
and patches only each row's first 12 bytes. The freshly linked baseline boot,
patched boot and changed-row receipt are separate. Sector extents and lengths
are updated in both ISO9660 and UDF when streams grow/shrink; callers cannot
request an unpaired stream resize through `pack-disc` or `pack-iso`.

The real no-op full-file hashes match the original MUSIC and VOICE. Edited
opening music cue 63 shrank from 25.088 to 12.544 stored seconds (12 requested);
voice cue 143 grew from 4.555 to 6.571 stored seconds (6 requested). The stored
lengths include the silent timeout guard. Subsequent rows moved with them. Independent
`audio_export` decoding verifies both interleave and new durations. Receipts:
`build/repack/streams/bundle-proof-v2.json` and
`build/repack/streams/test-receipt.json`.
Playback evidence is described below; a WAV round trip alone is not playback.

### Movie re-encoding requirements (design only)

A movie encoder would need a PS2-compatible MPEG-2 elementary video stream
(profile/level, dimensions, frame rate, GOP and decoder-buffer bounds compatible
with libmpeg), the PSS private audio framing and sample layout, and a muxer that
rebuilds pack/system/PES headers, SCR/PTS/DTS timing, sector packing and terminal
padding while maintaining A/V synchronization. It must also honor the movie
reader's buffering and skip/finish behavior. `func_002032C0` obtains the nine
PSS file extents by filename at runtime; resizing still requires the ISO/UDF
writer to allow and relocate those files. EXTER1's consumer must be audited
separately. The native port's `export_movie.py` is a packet-preserving remuxer,
not evidence that arbitrary re-encoded MPEG-2/PSS will work on PS2. No MPEG-2
encoder or movie inverse muxer is implemented here.

## Same-topology models

```sh
python3 -m tools.repack model-unpack \
  --input build/repack/loose/archive/chunk28/f00_id3b.bin \
  --out build/repack/player
# Edit model.gltf / its buffers while preserving _NATIVE_ID and vertex topology.
python3 -m tools.repack model-pack --tree build/repack/player \
  --out build/repack/player-edited.bin
cp build/repack/player-edited.bin build/repack/loose/archive/chunk28/f00_id3b.bin
```

Supported framed actor packets expose POSITION, TEXCOORD_0, NORMAL and existing
rigid JOINTS/WEIGHTS; explicit static packets expose colours instead of normals.
Vertex count/order, triangle indices, restart/parity bits, packet structure,
materials and native identity attributes remain fixed. Buffer/accessor repacking,
local or embedded buffers and harmless names/extras are accepted. Dropped custom
IDs, welded/reordered vertices, changed node transforms, compressed/sparse
accessors, arbitrary blended weights and new materials are refused.

The glTF geometry remains in native bone-local coordinates; identity skin nodes
are an editing view, not an assembled animated bind pose. Positions must stay
inside original per-joint bounds because posed culling bounds are not rebuilt.
This permits conservative shape tweaks, UV/normal edits, colours and reassignment
to populated existing rigid joints. The player and a static-colour model pass
unchanged byte-identical round trips and edited forward extraction. The local
player proof scales joint 7 positions within their original bounds. Full native
packet/material/VIF/VU/skinning/relocation design and limitations are in
[REPACK_MODELS.md](REPACK_MODELS.md); full re-topology is design only.

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
is identical. The latest equivalent title comparison is retained at
`build/repack/model-status-baseline/title/original.png` and
`build/repack/final-game-proof-v3/title/original.png`, with its own
`build/repack/final-title-comparison.json`. The earlier `build/repack/mod-proof.json` records both
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

## Cold gameplay, model and stream proof

`proof-gameplay` cold-boots the supplied disc, enters New Game through controller
input and captures the opening. It checks optional edited player vertices through
the live resource pointer, and saves IOP/SPU state for independent sound checks.
`--status-model` opens STATUS, captures after 60 neutral frames, and returns to
field control. The opening attaches a separate face and suppresses base head
joint 7; STATUS shows the base model. The movement gate waits for normal player
state, the script selector, UI state and fade completion.

```sh
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m tools.repack proof-gameplay \
  --iso build/repack/stream-mod/Extermination.iso \
  --model build/repack/player-edited.bin --status-model --voice-route \
  --out build/repack/game-proof
```

Omit `--model` if no player edit is installed. `--voice-route` walks the existing
battery/elevator/boxes/hill/truck/cage route using controller input only. It
captures twice after the first EE activation of dialogue cue 143; that activation
can precede hardware key-on, so the separate verifier must establish playback.
The proof prohibits RAM writes and teleports, uses the shared lock and private
cardless configuration, saves only private slot 16, and confirms shutdown.
Snapshot discovery uses the private slot number because cue patches change the
ELF CRC in PCSX2's state filename. Protected slots 01–15 stay inaccessible.

Verify playback against the actual edited stream and cue-patched boot:

```sh
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m tools.repack.stream_proof \
  --stream build/repack/music-bundle/MUSIC.DAT \
  --elf build/repack/stream-mod/cue-boot/SCUS_971.12 \
  --driver build/repack/loose/iso/files/IRX/SNDN2DRV.IRX \
  --proof build/repack/game-proof --kind music --cue 63 \
  --out build/repack/music-playback.json
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m tools.repack.stream_proof \
  --stream build/repack/voice-bundle/VOICE.DAT \
  --elf build/repack/stream-mod/cue-boot/SCUS_971.12 \
  --driver build/repack/loose/iso/files/IRX/SNDN2DRV.IRX \
  --proof build/repack/game-proof --kind voice --cue 143 \
  --out build/repack/voice-playback.json
```

The verifier requires the requested EE cue to be active, the matching IOP voice
to be active with nonzero rate/volume, and its current play address to advance
between captures. A complete currently playing SPU half-buffer must match the
replacement native bytes, allowing only the driver's known boundary flags.
Stereo music requires both voices. This proves active native playback; it does
not record or compare an audio-device output waveform.

The combined source-built showcase has SHA-256
`824821e594f8f8d721fb2642f8dd0fa5f3668517a115b903c279cb765dd3cb80`.
It includes the magenta title palette, a 20% smaller base head (1,665 joint-7
vertices), 12 seconds of replacement stereo music in cue 63 and 6 seconds of
replacement mono voice in cue 143. The stream encoder adds the documented
silent timer guard and sector padding. All 44 source stages ran fresh; the boot
and 19 overlays matched before the permitted cue-table patch. ISO9660/UDF
verification found no unrelated file changes.

Original-disc and edited-disc STATUS screenshots differ by 618 pixels, entirely
inside the head rectangle x=395..420, y=39..68; every other pixel is identical.
The native resource's three vertex probes also match the edited asset in each
opening snapshot. Screenshots are
`build/repack/model-status-baseline/model-status/original.png` and
`build/repack/final-game-proof-v3/model-status/original.png`; the numerical
comparison is `build/repack/model-edit/status-comparison.json`.

The final cold-disc route completed at frame 8,452 with zero teleports or RAM
writes. Stereo music cue 63 passed at frames 100/200/400 of the opening (both
voices, six advancing pairs); mono voice cue 143 passed in two captures at
60/120 frames after its first EE activation (frame 8,332). Both satisfy the
full currently playing SPU-half comparison. Receipts are
`build/repack/streams/music-playback.json`, `voice-playback.json`, and
`build/repack/final-game-proof-v3/proof.json`. The latter confirms emulator exit,
empty disabled memory cards and all 12 protected save states unchanged.

After verification, generated emulator copies, private state files, superseded
screenshots, compiler scratch, the isolated mod tree and temporary edited ISOs
were removed. The latest title/model screenshot pairs, port title capture,
playback IOP/SPU captures, editable format examples, compiled-output provenance
and cited receipts remain. `build/repack/followup-final-cleanup.json` records
cleanup; `followup-canonical-check.json` verifies all 674 canonical archive leaves
and 43 ISO files still match their original hashes. The build/proof commands
regenerate removed images in a new output directory.

## Native port exports and proof

The port proof uses an existing native Apple Silicon executable and its own
exporters, all read-only in the sibling repo:

```sh
.venv/bin/python -m tools.repack.port_proof \
  --iso build/repack/my-mod/Extermination.iso --out build/repack/native-proof
```

This is a **magenta-title fixture**, not an arbitrary-colour comparison: apply
the title-palette walkthrough first. It copies supporting port assets privately,
stages DATA/INDEX/boot from the supplied disc, runs the legacy extractor and
startup composer, and runs the port's `export_movie.py --iso`,
`export_streams.py --iso --elf`, `export_module_loader.py --iso --elf`, and
`export_disc_textures.py --iso --extract --assets --scratch --only font` with
explicit scratch destinations. The font exporter has no `--elf` argument; the
helper records its pinned main-checkout ELF input and verifies that it matches
the showcase disc's boot. No shared asset is overwritten and no port build runs.

The existing port executable then runs with private working directory and
`EM_HEADLESS=1 EM_STARTUP_TEST=skip EM_STARTUP_CAPTURE_DIR=...`. Its own capture
produces `capture/title_0.bmp`; the PNG is a lossless convenience conversion.
`--resume-title` permits only a checked retry after exports succeeded, verifying
all private assets, source image and protected shared input hashes. It was used
locally after macOS graphics APIs rejected the sandboxed launch; the permitted
headless retry passed. It does not bypass exporter failures.

The source-built texture/model showcase had SHA-256
`568211142de0e8950736903b16af9c5905448aaa0f791f4550f3669c642676a0`.
All six export steps passed. The native logos/movie/title-navigation fixture
passed, and its 1920×1440 capture visibly shows the magenta NEW GAME label.
`build/repack/port-proof/receipt.json` distinguishes 670 copied support assets
from 19 freshly exported assets and records unchanged shared assets, exporter
scripts, boot input and executable. Screenshot:
`build/repack/port-proof/capture/title.png`. This proves the disc texture reaches
the live port title. The model and other gameplay support exports were not all
regenerated, so this does not claim native-port gameplay proof for them.

### Exact exporter blockers and additional mod types

A source-built stream candidate was supplied to both existing exporters with
its actual cue-patched boot. Both failed before writing assets:

- `tools/export_streams.py`, `Elf.__init__`: the complete SHA-256 "is not the
  pinned SCUS-97112 boot ELF".
- `tools/export_module_loader.py`, `boot_tables`: "not the pinned boot ELF".

The tested candidate and boot hashes, full argv, source locations and failure
logs are in `build/repack/port-stream-blocker/receipt.json`. It predates the
final silent-tail refinement; that later candidate was not used for these
failure probes. The reason is unchanged: both exporters compare the whole
ELF against the canonical hash. Passing the original boot would silently use
old cue offsets, so no substitution or private exporter patch was made.
Supporting these stream mods in the port requires a port-side change.

For other types, the following are inspected export recipes, **not additional
runtime proofs**. Create a private decomp-shaped `STAGE` with the mod disc's
legacy `extract/` and `config/SCUS_971.12`; keep it separate from corrected
repacker labels. `ASSETS` must be a private scratch asset tree, and `PORT` is the
read-only sibling repo. Use `PYTHONDONTWRITEBYTECODE=1` for every command:

```sh
# Re-export both the HUD strings and the live service/presenter data.
python3 tools/export_ui.py --messages --extract "$STAGE/extract" --out-dir "$ASSETS"
python3 "$PORT/tools/export_message_data.py" --decomp "$STAGE" \
  --out "$ASSETS/message/message_data.emmd" \
  --presenters-out "$ASSETS/message/message_presenters.emmp"

# Startup audio only: STAGE/tools/audio_export.py must be a copy of our tool.
python3 "$PORT/tools/export_startup_audio.py" --decomp-root "$STAGE" \
  --out "$ASSETS/startup_audio"

# The live first-level player packet resource (no stale original RAM comparison).
python3 "$PORT/tools/export_player_model.py" --iso "$MOD_ISO" \
  --extract "$STAGE/extract" --out "$ASSETS/scene_snow/player_model.emom" --no-verify
```

`export_message_data.py` also pins the full boot hash and hardcodes the global,
help, record, cue and embedded AREA11 message-bank locations (including
`chunk15/f12_id44.bin + 0x3E800`). `export_startup_audio.py` reads only its five
startup cues/two sample sources at fixed `chunk00/f05_id05.bin`; it is not a
general sound-bank installer. Per-area SFX exporters such as
`export_area11_sfx.py` hardcode main inputs/outputs and offer no safe scratch
CLI for a general replacement. The player exporter retains packet validation
and a disc ResourceTable address check against `0xD1C1C0`; `--no-verify` skips
only stale original RAM captures. Same-topology edits can fit it, but relocated
resident addresses need wider port work. Follow the port's `docs/STARTUP.md`
for dependent pose, texture and status-model exports; do not blindly rename
legacy files to corrected archive roles.

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

To verify a completed cue-edited source build without recompiling:

```sh
PYTHONDONTWRITEBYTECODE=1 \
EM_TEST_ISO='/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
EM_STREAM_BUILD_OUTPUT=build/repack/final-built \
  .venv/bin/python -m unittest tools.repack.test_completed_stream_build -v
```

This independently reads all 43 files through ISO9660 and UDF, checks the stream
hashes and complete cue tables, restricts boot changes to permitted row fields,
and verifies unrelated files against the original. The final local pass took
6.537 seconds; receipt `build/repack/completed-stream-test-receipt.json`.

The independent UDF test oracle uses `pycdlib` in the existing project
virtual environment. The repacker has no third-party Python dependencies;
mod-pack deltas and byte scanning require the native host C99 compiler.
The full suite defaults to `Extermination-rebuilt.iso` when `EM_TEST_ISO`
is omitted; set it explicitly as above to check the original image.

The 2026-10-08 original-disc integration proof passed in 53.128 seconds,
including a one-byte control, +37/-13-byte resident resizing, addition of a new
resident role, exact second-generation archive repacking, and independent UDF
reads. All 674 existing asset leaves remain unchanged except each selected edit.
The mod-pack/title-upgrade quick run collected 183 tests: 166 passed and 17
local-disc checks were skipped by their explicit environment gates (4.834
seconds). The final targeted pack/build suite passed 38 tests; all ten title
upgrade tests passed with the local-disc gate enabled, including the later
1024-pixel UV overflow rejection. The real two-build mod-pack test and hidden
title proof also ran separately. Logs and receipts remain under `build/repack/`.

| Format | Local proof |
|---|---|
| GS texture uploads | 63 native leaves / 113 transfers unchanged after PNG round trip; edited PSMT8/PSMT4 and CLUTs re-extract; physical canvases grow/shrink and chained packets relocate |
| Logical title upgrade | Native/boot no-op exact; 2× selected word and 16↔256 palettes decode correctly; cold PS2 title and loaded compositor/sampler proof passed |
| Audio | 41 SShd leaves / 116 banks / 2,318 clips unchanged after WAV round trip; longer/shorter samples and loops decode through the existing extractor |
| Message tables | 271 bank lines and 54 bare OUTER lines unchanged; longer/shorter text and relocated styles re-extract correctly |
| Music/voice | Both complete DAT files unchanged on a no-op; edited stereo music and mono voice re-extract with relocated cue rows, interleave and playback padding |
| Models | Real skinned player and static packets unchanged on a no-op; position/colour edits re-export through existing decoders; other attributes and topology guards have synthetic checks |
| Archive growth | +37 bytes, -13 bytes and a new resident entry; every other asset unchanged, second pack identical, ISO/UDF namespaces agree |
| Mod packs | Two independent real packs reconstruct the direct-edited ISO through fresh source builds; all 43 ISO/UDF hashes match; 65-byte original-content rejection and conflicts tested |

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

## Shareable mod packs

`make-modpack` and `verify-modpack` run in native Python on arm64 macOS or
Linux. `apply-modpack` currently uses the project's arm64 macOS/Apple container
source-build driver. The byte scanner compiles its small original C99 helper
with host `cc` (Xcode Command Line Tools on macOS), cached under
`build/repack/native-delta/`. Applying a pack invokes the same fresh Apple
container source build as `build-disc`; compiler setup and the build lock are
unchanged. Verification alone does not compile the game.

```sh
# The loose tree contains edited native leaves; keep PNG/WAV/glTF projects outside it.
ISO='/Users/abe/Documents/PS2 Games/Extermination (USA).iso'
python3 -m tools.repack make-modpack --iso "$ISO" \
  --tree build/repack/loose --out build/repack/my-mod.emmod
python3 -m tools.repack verify-modpack --iso "$ISO" \
  --pack build/repack/my-mod.emmod
.venv/bin/python -m tools.repack apply-modpack --iso "$ISO" \
  --pack build/repack/my-mod.emmod --pack build/repack/another-mod.emmod \
  --out build/repack/installed
# Output: build/repack/installed/disc/Extermination.iso
```

Distribute only the verified `.emmod`. The recipient supplies their own exact
original disc and builds locally. Adding another pack means rebuilding from the
original with the complete `--pack` list; a previously modified ISO is not a
valid base. A fresh output directory is required. Same-file conflicts list the
target and both packs, even when their edits happen to agree. They fail before
installation or compilation; there is no automatic winner or subfile merge.

The deterministic container is an uncompressed canonical ZIP with a UTF-8 JSON
`manifest.json`, schema `extermination-modpack-v1`, and numbered
`changes/0000.delta` or `.full` members. The manifest binds serial `SCUS-97112`
and the complete original ISO SHA-256. Each change includes its stable target,
original-file hash (null only for a new entry), result size/hash, payload hash,
encoding and boolean `created_from_scratch`. Original leaves use corrected
`archive/...` paths; direct disc targets use `iso/STREAM/...` (without `files/`).
New resident roles use `add/chunkNN[.nN]/hh`, so independent additions are
assigned fresh ordinals at installation instead of colliding on old filenames.

Ordinary edits use the binary `delta-v1` codec after the relevant PNG, WAV,
JSON or glTF encoder produces native bytes. Its `EMDLT1` header binds base and
result size/SHA-256 and instruction count. COPY names an original offset and
length; XOR names an original range plus residual bytes; INSERT contains new
literal bytes. Base bytes are required to apply deltas. Strict validation
rejects unknown/trailing/redundant instructions, invalid ranges and output
hashes. No original template, loose manifest, full cue table or executable is
included. Limits are 4 MiB manifest, 4,096 changes, 1 GiB container/payload sum
and each result strictly below 1 GiB.

FULL is available only with an explicit authorship declaration:

```sh
# After add-entry prints its new archive path, or after a wholly new replacement:
python3 -m tools.repack make-modpack --iso "$ISO" --tree build/repack/loose \
  --authored archive/chunk04.n0/f03_idfe.bin --out build/repack/new-work.emmod
```

Use the actual path printed by `add-entry`, not the example ordinal. Repeat
`--authored` for each wholly created asset. New entries require FULL; an unused
or unchanged declaration is an error. A declaration asserts the modder owns the
entire supplied content, including its native envelope. Recoloring original
art or modifying original audio is a delta edit, not newly authored FULL work.

Every make, verify and apply scans distribution content against **every byte of
the original ISO**, rejecting any identical contiguous run of 65 bytes. The
exempt ranges are only validated delta instruction fields; XOR residuals and
INSERT literals are scanned too. ZIP framing, manifest and FULL content are
scanned, including cross-instruction/member/chunk boundaries. Logical content
concatenation is checked as well. Canonical ZIP reconstruction rejects hidden
members, comments, alternate encodings and leading/trailing content. The rolling
window scanner verifies hash matches with an exact byte comparison. This is a
conservative byte-content policy, not an authorship detector: it can reject
coincidental matches and cannot recognize a transformed copy of someone else's
work. FULL declarations remain the author's responsibility.

Stream bundles and logical texture upgrades use restricted semantic build
instructions. For streams, pass the existing bundle when making the pack:

```sh
python3 -m tools.repack make-modpack --iso "$ISO" --tree build/repack/loose \
  --stream-bundle build/repack/music-bundle --out build/repack/music.emmod
```

The stream DAT becomes a delta. Only changed cue lengths enter
`instructions["stream-edits.json"]`, schema `extermination-stream-edits-v1`;
offsets, flags and unchanged rows are reconstructed from the recipient's boot.
Music and voice instructions can stack; duplicate stream kinds conflict.
The title profile uses `instructions["texture-upgrades.json"]` with validated
dimensions, palette counts and content hashes. Its native upload is a delta.
No arbitrary patch bytes or addresses are accepted. `build-disc` independently
recreates both patches from the canonical fresh boot and refuses overlapping
changed-byte ranges. A loose tree with either descriptor must use `build-disc`;
`pack-disc` refuses it rather than silently retaining a stale boot.

Synthetic `test_delta` and `test_modpack` tests cover the 64/65 boundary, copied
bytes from untouched files, FULL/residual joins, residual/ZIP-header joins,
wrong bases, tampering, conflicts, declared additions and independent stacking.
The opt-in real-disc test makes separate title/model packs, builds their edited
tree from fresh sources, applies both packs through another fresh source build,
and compares complete ISO SHA-256 plus all 43 files through ISO9660 and UDF:

```sh
EM_TEST_MODPACK_BUILD=1 EM_TEST_ISO="$ISO" PYTHONDONTWRITEBYTECODE=1 \
  .venv/bin/python -m unittest tools.repack.test_modpack_full -v
```

This local fixture requires the previous title/model example outputs described
above. Results and source-stage provenance are retained in
`build/repack/modpack-proof/receipt.json`; large intermediates are removed.
The source compiler still needs the user's original unmatched assembly/data.

The 2026-10-08 run passed in 875.650 seconds: both builds completed all 44 stages,
with 2,214 fresh boot objects and 19 freshly linked overlays. Direct and installed
ISO SHA-256 were both
`568211142de0e8950736903b16af9c5905448aaa0f791f4550f3669c642676a0`.
All 43 ISO9660/UDF file hashes agreed; only DATA.DAT differed from the original.
The canonical loose tree stayed unchanged. The separate title and model packs
were 1,045 and 106,486 bytes. The resized music example also made and verified a
689,156-byte pack reconstructing the 314,552,320-byte DAT; its only cue instruction
is the changed length for music cue 63. Receipt:
`build/repack/modpack-stream-proof/receipt.json`.

## Native-port mod support design

[REPACK_PORT.md](REPACK_PORT.md) specifies the changes maintainers would need
for modified discs and packs. It identifies full-ELF pins, implicit original
inputs, original-address assumptions, cue coverage, corrected archive spans,
transitive asset identities and native cache invalidation. Proposed acceptance
normalizes only reviewed cue fields or regenerates the audited title compositor,
then verifies every other boot byte against the original. Actual edited tables
must feed exporters. The port remains read-only; logical title upgrades and
cue-patched boots are not claimed to work in its current exporters.

Only original tooling, synthetic test builders, and documentation belong
in commits. Before each commit, inspect the staged paths and run:

```sh
python3 tools/check_no_disassembly.py --staged
```
