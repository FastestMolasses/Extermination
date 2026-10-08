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

These tools use standard-library Python on **native arm64 macOS**. The same
commands work on Linux with Python 3.10 or newer; no Rosetta, compiler,
container, emulator, mounted disc, or external Python package is needed.
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
files in `loose/iso/files/`. Rebuilt ELF/overlay files can be placed there
explicitly, after their own decomp verification.

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

Size-changing edits must preserve **2,048-byte alignment**. The CD reader
addresses sectors; arbitrary byte growth would silently load the wrong
data. Append or remove complete sectors while keeping the asset itself
valid. The packer moves subsequent regions, fixes their absolute offsets,
updates region lengths, resident starts, upload section bounds, and
resident relocation offsets. Table counts stay fixed because adding or
removing descriptors or table entries is outside this version's scope.
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
not fix those internal structures or prove game compatibility. Streams
and opaque files are copied as-is in the supported workflow. There is no
inverse glTF/EMDL/EMCL encoder here, and no real-mod boot is claimed.

## Verification

```sh
# Quick synthetic format, mutation, resize, and validation checks.
PYTHONDONTWRITEBYTECODE=1 .venv/bin/python -m unittest discover \
  -s tools/repack -p 'test_*.py'

# Full local-disc archive + ISO round trip and negative controls.
PYTHONDONTWRITEBYTECODE=1 EM_TEST_FULL=1 \
EM_TEST_ISO='/Users/abe/Documents/PS2 Games/Extermination (USA).iso' \
  .venv/bin/python -m unittest discover -s tools/repack -p 'test_*.py'
```

The independent UDF test oracle uses `pycdlib` in the existing project
virtual environment. The repacker itself has no external dependencies.
The full suite defaults to `Extermination-rebuilt.iso` when `EM_TEST_ISO`
is omitted; set it explicitly as above to check the original image.

The completed original-disc proof took 152.325 seconds. The final quick
suite passed 25 checks in 1.398 seconds, with the full-disc test skipped by
default. The full run passed every check, including the documented CLI
workflow. Hashes of the original and rebuilt outputs are equal:

| Output | SHA-256 (original and rebuilt) |
|---|---|
| DATA.DAT | SHA-256 `b6dc43cf8c75dce4c980211f12e11dde61fb280206642c237af729e518e6a323` |
| INDEX.IDX | SHA-256 `bed4390e7686bf2c46d4ab81fcbec4c9c42d2604ffd4d9bee39647ca43b398fc` |
| Full original ISO | SHA-256 `b6fdb617d2438d3bf1163553e46760365f056e5428030550943142f9c5cc6efe` |

`build/repack/test-receipt.json` records per-file original/packed hashes
for all 43 ISO files, their independent UDF reads, and all 674 loose archive
spans. The single-byte control changed only DATA byte 7,393,280 and ISO
byte 1,082,101,760; INDEX stayed identical. Growing and shrinking
`chunk04.n0/f00_id43.bin` by one sector each unpacked to the exact edited
tree, with all 673 other loose files unchanged and all unrelated ISO/UDF
files unchanged. `build/repack/archive-controls.json` additionally records
real sound-upload growth, resident-DMA shrinkage and empty-descriptor
anchor relocation. Quick synthetic checks cover those table types,
malformed inputs, source aliases, and false UDF anchor detection.

The tests remove their large intermediate images and loose trees; receipts
remain. Regenerate an editable tree and deliverable ISO with the commands
above. PCSX2 was not run; loading a real asset mod is the optional remaining
step. No game files under `src/`, legacy extractors, native-port files, or emulator
state were changed.

Only original tooling, synthetic test builders, and documentation belong
in commits. Before each commit, inspect the staged paths and run:

```sh
python3 tools/check_no_disassembly.py --staged
```
