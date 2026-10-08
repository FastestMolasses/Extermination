# Native-port integration design

This is a design, not an implemented port mod loader. The repack tools can
reconstruct edited native files and discs locally. The existing port can
demonstrate some asset edits through its exporters, but it cannot yet accept
every supported repack edit. No port source, executable, shared assets, or
original-disc inputs were changed for this audit.

Audit baseline, 2026-10-08: repack commit
`c930eeb488f8d122708674e32c992dfd2d845038`; sibling `extermination-port`
commit `f6922a309c6287698da9d81980a85522b8566657`. The source references below
are relative to those repository roots; line numbers identify the audited
versions and can move. All proposed commands and APIs in this document are
future interfaces. Current executable commands remain in [REPACK.md](REPACK.md)
and [MODDING.md](MODDING.md).

## Current checks and integration gaps

| Source and function | Current behavior | Required change |
|---|---|---|
| Port `tools/export_streams.py:85`, `Elf.__init__`; `:193`, `export` | Requires the entire pinned original boot SHA-256, then reads cue rows from that ELF and stream sectors from the chosen ISO. | Accept an explicitly validated source context; extract the actual patched cue rows. Keep the original mode strict. |
| Port `tools/export_streams.py:140`, `cue_list`; `:173`, `extents_for` | Exports selected reachable cues, including first-level and AREA01 references; writes all 68 music and 179 voice rows but only selected data extents. | Record the selected cue set and its dependency sources. A mod using an uncovered cue must trigger re-export or fail, rather than refer to absent sectors. |
| Port `tools/export_module_loader.py:208`, `boot_tables` | Requires the entire original boot hash; derives file names and several loader constants from it. | Consume the same validated actual boot as all other exporters. |
| Port `tools/export_module_loader.py:268`, `main`, especially `:310` | Derives cursors from the selected disc but requires equality to original `AREA11_SEEDS`; optional capture checks also require original descriptor/cursor equality. | Recompute and validate modded cursors and bounds. Retain original values as baseline test expectations, not a universal modified-disc predicate. Never use an old capture to seed a modified layout. |
| Port `tools/export_disc_textures.py:76`, `elf_bytes`; `:304`, `build`; `:365`, `main` | Reads `G.ELF_PATH` from the sibling main checkout, checks its original hash, and has no `--elf` option. Some subparts also read the sibling overlay directly (`part_page`, `:115`). | Make boot and overlays explicit context inputs; eliminate hidden fallback reads in modified mode. Record the actual boot digest instead of the constant currently reported at `:345`. |
| Port `tools/export_disc_state.py:282`, `first_frame` | Checks the whole boot hash, then executes original routines over reconstructed memory to derive initial objects. | Keep a verified code/data policy before execution. Use actual modified data with code proven unchanged outside explicitly approved patches; retain interpreter bounds and unsupported-instruction failures. |
| Port `tools/export_disc_textures_gs.py:258`, `FirstLevel`; `:273`, `_extract_span` | Checks a disc span by concatenating legacy `f*_id*.bin` files. | Resolve exact spans through the corrected archive manifest, including upload sections, padding and resident leaves. Merely pointing `--extract` at the new tree is insufficient. |
| Port `tools/export_disc_textures_gs.py:444`, `ResourceTable` | Models loader order and derives resource pointers from descriptors and bank ends; area nested blocks are explicitly unsupported in this model. | Reuse the calculation for supported areas with validated inputs. Refuse unsupported load paths until modelled; do not assume filename or original heap address is an identity. |
| Port `tools/export_player_model.py:66`, `build`; `:95`, `main` | Reads `chunk28/f00_id3b.bin`, checks a default absolute model address against the disc, then checks original RAM captures unless `--no-verify` is supplied. `--no-verify` does not remove the address check. | Derive the address from the selected disc; check native layout and model identity there. Original-capture byte equality belongs to unmodified regression tests. Use modified runtime observations for edited content. |
| Port `src/game/em_player_draw_live.c:37`, `em_player_draw_live_bank`; `:63`, `em_player_draw_live_unload` | Caches both successful and failed load attempts; reads a fixed relative `assets/scene_snow/player_model.emom` path. | Initially start a new process per asset-set identity. Later switching requires coordinated unload of all owners, not only this model. |
| Port `src/game/em_iop_stream.c:486`, `em_iop_stream_disc_load` | Validates EMST version, fixed row counts, and extent bounds, then retains the file in memory. | Preserve these checks; add manifest validation before loading. Cue rows, extents and stream bytes must come from one source identity. |
| Port `src/game/em_module_loader.c:575`, `em_module_loader_open` | Validates EMML magic/version, range counts, sector limits and payload bounds; uses exported descriptor/cursor words. Missing delivered ranges fail at runtime. | Preserve structural checks; validate whole-set provenance before loading and prove needed module/area coverage. |
| Port `src/em_model.c:20`, `em_model_load` | Validates EMD2/EMD3 format and allocation limits; loads baked models separately from the raw EMOM player path. | Invalidate every representation consuming a changed model, animation or material. Updating `player.emdl` alone does not establish that the live raw player path changed. |
| Port `src/main.c:385`, `:394`; `src/game/em_player_draw_live.h:38` | Loads fixed paths below the process working directory. | First integration can launch from a private complete asset-set directory. A later explicit asset-root API should replace reliance on working-directory conventions. |

No SHA-1, SHA-256, or checksum-based source validation was found in the audited
port `src/` tree. The cited native loaders check structure; exporter JSON hashes
are receipts, not presently enforced native-loader provenance. This observation
does not establish that every loader accepts every well-formed edit.

The existing repack `tools/repack/port_proof.py:103`, `capture_title`, is a
bounded title fixture: it copies support assets, re-exports a named subset,
hashes protected inputs, and launches headlessly from private scratch. At
`:159` it rejects a supplied boot differing from the font exporter's implicit
main-checkout boot. Its magenta-pixel check is specific to the title test edit.
It is neither a general modified-disc importer nor proof that copied assets
reflect modified inputs. Do not relax that helper's checks to hide a failed
integration.

`docs/FINDINGS.md:582` and `:1001` retain historical extractor/grouping and
silence-split audio inventories. Use the corrected archive definitions in
[REPACK.md](REPACK.md) and ELF cue tables for identity and playback boundaries.
The disc-derived first-frame mechanism is documented at
`docs/FINDINGS.md:16765`; it explains why a boot check protects executable
behavior as well as table layout.

## One validated input context

Introduce a shared `DiscContext` in the port exporter layer. It must be passed
to every exporter and helper that reads game data, including imported helpers
from the decomp. It owns read-only handles and validated views of:

- Exact original region/build identity and original user-disc SHA-256.
- Effective source identity: selected modified ISO hash, or the original hash
  plus an ordered list of verified modpack hashes and the resulting leaf map.
- Actual boot and overlay bytes, their lengths and hashes, parsed ELF segments,
  accepted patch-policy version, and the verified unchanged-code result.
- ISO file map and corrected archive manifest, including every native leaf,
  upload section, descriptor, resource slot and reconstructed resident address.
- Output directory, temporary directory, exporter options, and source-code
  revision/digest for the exporters and all imported decoder helpers.

The original disc remains a required local input for modified mode. A modified
disc supplied without trustworthy metadata is compared locally to that original
to derive changes; a claimed receipt is not proof on its own. Hashes identify
inputs but do not prove ownership or authorship. No baseline bytes or generated
assets are included with the tools or a redistributable receipt.

Future input contracts, native Python on arm64 macOS or Linux:

```text
port-export --original-iso ORIGINAL --modified-iso MODIFIED
            --out build/repack/port-export/RUN --profile first-level

port-export --original-iso ORIGINAL --modpack PACK [--modpack PACK ...]
            --out build/repack/port-export/RUN --profile first-level
```

The two effective-input modes are mutually exclusive. `--original-iso`, one
effective input mode, an explicit output, and a coverage profile are required;
there is no implicit `Extermination-rebuilt.iso`, main-checkout `extract/`, or
`config/SCUS_971.12` fallback. Native export does not need the PS2 cross compiler
unless the selected edit requires a fresh source build; the existing source
build remains the separate x86_64 Linux container step.

For the first modpack implementation, reuse the repacker to construct a private
effective loose tree/disc, then follow the same exporter path as modified ISO
input. This avoids a second interpretation of descriptor relocation and cue
patching. A later virtual overlay may avoid the intermediate ISO only after it
proves equivalent file contents, descriptors, virtual sector allocation and
resource addresses. Never mutate the original tree in place.

Use descriptor identity plus native role, ordinal and original hash for target
resolution, not a human display label alone. Resident identity includes chunk,
nested descriptor if any, file-table entry and resource ID. Upload identity
includes descriptor and section type/index. IDs can repeat; adding a file can
change ordinals; reflow can change offsets. An unresolved or ambiguous match is
an error. Reconstruct and validate edits before invoking an exporter.

## Boot acceptance and cue-table patches

The first supported modified-boot policy should be `scus-97112-cues-v1`. It
permits data changes only at the first three 32-bit fields of existing cue
rows. It does not accept arbitrary game-code patches.

1. Verify the local original boot against the pinned original SHA-256 and
   layout. Parse both ELF headers/program headers, rejecting ambiguous segment
   mappings, truncation, overflow and altered packaging. The port currently
   uses the original `vram - 0x100000 + 0x300` mapping. Supporting a different
   ELF package requires replacing all such readers first.
2. Resolve the music table at VRAM `0x25DD30` (68 rows) and voice table at
   `0x25E170` (179 rows) through validated LOAD segments. Approved offsets come
   from the built-in policy, never arbitrary ranges declared by a modpack.
3. Compare actual original and modified boots. Every changed byte must lie
   within bytes 0..11 of an existing 16-byte cue row. Headers, instructions,
   the fourth word (loop flag), other tables and all padding remain identical.
   Row zero remains the original null entry. Counts and identities cannot change.
4. Copy only the original bytes of these permitted fields into a temporary
   in-memory copy of the modified boot and require the full original hash.
   Also require unchanged file length. This proves that all bytes outside the
   approved fields remain original, without shipping an original-byte template.
5. Parse the actual modified rows and check their numeric bounds, nonzero
   lengths, sector alignment, byte offset equal to sector times 2048, immutable
   loop flags, contiguous tiling and final end equal to the actual corresponding
   stream length. Hash both stream files from the same effective source.
6. Record actual boot hash, baseline hash, policy version, changed row IDs and
   range offsets/hashes. Emit no original-byte arrays in a distributable report.
   Export the actual modified rows into EMST and record each selected data
   extent's actual hash and LSN. Never read original rows while using edited
   stream bytes.

This follows the existing repack constraints in
`tools/repack/streams.py:35`, `_table_offset`; `:61`, `_validate_rows`;
`:228`, `inspect_bundle`; `:256`, `apply_cue_patches`, and
`tools/repack/build_disc.py:8`, `build_disc`. The importer independently verifies
the effective files; it does not trust a bundle's original/output hash claims.
The full original ELF comparison is stronger than hashing only the main text
section, since executable behavior can depend on other unapproved tables.

A texture/TEX0 patch needs its own reviewed port acceptance policy. For
immediate edits it must bind exact instruction locations and allowed bit fields
to a known baseline, validate the resulting texture/material references and
upload closure, and prove all other instruction bits and bytes unchanged.
The bounded title-compositor replacement implemented in the repacker requires
a different rule: pin the original function hash and extent, regenerate the
authored replacement deterministically from the strict semantic descriptor,
compare the entire replacement, and prove all bytes outside that extent
unchanged. Require the original/new menu-branch draw/state/ABI oracle results
for that policy version. Neither form can reuse the cue-data exemption or
accept replacement code bytes supplied by a pack. The native translation must
actually consume the new values, or a matching explicit native data export must
be implemented;
accepting a patched PS2 ELF does not automatically patch hardcoded port C
constants. Unrecognised code or overlay modifications remain unsupported.

## Export dependencies and cache identity

Each output records its complete transitive input set. The manifest is an
invalidation graph rather than a list of whichever files happened to be
opened by the last run. A missing or unclassified dependency forces a rebuild
of the enclosing export group or an unsupported-edit error.

| Changed effective input | Minimum invalidation group |
|---|---|
| DATA/INDEX layout, sizes or entry counts | Resource table, EMML ranges/cursors, GS residency, raw model addresses, first-frame derived state, and dependent exports. Revalidate native address assumptions. |
| Native player/model leaf | Raw EMOM and every baked EMDL/status representation, bounds, textures/material dependency checks and model receipts. |
| GS upload or CLUT | All consumers of the affected GS pages across their load lifetimes, including startup atlases, raw/material paths and status exports. A pathname-only texture cache is insufficient. |
| SShd payload or layout | Relevant audio exports, bank/loader reads, resource addresses when resident sizes change, and sound registration metadata. |
| MUSIC/VOICE or cue rows | EMST row table, selected extents, LSN metadata, cue coverage and playback receipts. Re-export both row metadata and payload as a transaction. |
| Native text/decoded table | Dependent message/page exports and renderer-limit validation; derived first-frame data if the table is read there. |
| Boot/overlay | Policy check and every exporter that reads or executes it. Refuse unapproved changes before any original-code interpreter runs. |

Use a deterministic cache key over schema version, effective source hashes,
coverage profile, options, exporter/helper code digests, patch policy and the
sorted transitive dependency map. Do not key by filename, mtime, the original
disc hash alone, or a short mod name. Reusing a copied support asset is valid
only if its recorded dependency hashes and export version all match. Otherwise
re-export it or reject the incomplete asset set; never silently fill it from
the user's unrelated shared `assets/` directory.

An `asset-set.json` manifest should contain, for every output: relative path,
size, SHA-256, format/version, producer identity, dependency hashes, whether it
was rebuilt or reused, and any coverage limits. It also records actual boot
and overlay identities, corrected archive identity, modpack order, patch-policy
result and native executable hash. Do not include machine-specific absolute
paths in distributable pack metadata; local receipts may retain them.

The first launcher validates all outputs against the manifest before process
startup and starts a fresh process from the private asset-set directory.
Publication is atomic only after every export and verification succeeds. It
must reject symlinks, path traversal, output/input aliases, duplicate paths and
unlisted extra files in managed asset directories. Temporary files stay below
the explicit scratch root. Protect shared inputs with before/after hashes,
including imported exporters; set `PYTHONDONTWRITEBYTECODE=1` for every child.

This launcher check does not make the current native executable independently
verify provenance, and cannot remove a time-of-check/time-of-use race against
another local writer. A later native asset-root/manifest API should validate
the exact bytes it loads, attach an asset-set generation to every cache, and
reject mixed generations. The current `P.loaded/P.tried` model cache and the
retained EMST/EMML buffers make process restart the correct initial boundary.
Do not advertise live mod switching until audio, loader regions, GPU resources
and all model owners have coordinated teardown/reload.

## Modpack content-policy checker

The checker enforces a bounded package format and declared provenance. It is
not a determination of copyright ownership or a guarantee that arbitrary
payload bytes contain no third-party material.

- `COPY` is a source identity, offset and length referencing the recipient's
  exact locally verified original leaf. No referenced bytes are carried in
  the pack. Validate source and output bounds before allocating or copying.
- An allowed XOR residual is explicitly a delta instruction, tied to an exact
  original leaf hash and byte range. It reconstructs locally against that
  required base. Validate canonical encoding, output coverage and the resulting
  hash. This is the project's declared delta exception, not proof that the
  residual is independently authored or inherently free of original content.
- Every literal or `FULL` member requires an explicit authorship/source
  declaration attached to that exact member hash. Generic declarations must
  not silently bless unused payloads or future replacements. Disallow original
  executable/container templates as full replacement payloads; reconstruct
  preserved bytes from local originals. New authored data is still subject to
  the supported native format and runtime limits.
- Inspect decoded contents of every accepted wrapper. Compression, base64,
  nested archives, filenames, JSON fields, patch opcodes or metadata blobs
  cannot turn a forbidden literal into an exempt instruction. Prefer one
  canonical bounded archive/codec and reject unknown members, duplicate names,
  trailing bytes, arbitrary freeform binary metadata, decompression bombs,
  symbolic links and path aliases. All bytes in the pack need a schema role.
- A known local original-span comparison can flag unchanged original chunks
  embedded as literals, and canonical regeneration can reject superfluous
  instructions. The repacker's exact 65-byte scan also covers residuals and
  container framing; it can reject coincidental matches. Passing that precise
  byte policy cannot detect transformed copies or establish authorship.
  Do not label a passing result
  "legally safe". Report the precise policy passed and the declarations checked.

Deterministic creation should derive delta instructions itself from verified
old/new inputs instead of accepting arbitrary pre-encoded instruction streams.
Application revalidates the archive, schema, base hashes, instruction limits,
authorship references and output hashes independently. Pack metadata is data;
it must never supply executable hooks, shell commands, arbitrary importer
modules, or new boot-whitelist ranges.

## Acceptance plan

Quick synthetic tests should cover policy and dependency boundaries; full
BYO-disc and headless runtime proofs remain explicit slower gates.

1. Original round trip: the same original source context produces the existing
   supported exports byte-for-byte; original port reference tests retain their
   strict hash/capture checks.
2. Cue-only edits: music and voice duration grow/shrink exports use the actual
   patched rows and relocated file extents. Compare every exported row/extent
   to the effective source, then verify audible active playback and timer/loop
   behavior through the native runtime. Unselected modified cues are declared
   uncovered, not presented as tested playback.
3. Boot rejection: one instruction byte, one loop flag, row count, header,
   neighbouring table or unlisted overlay byte change fails before export.
   Fake whitelist offsets, fabricated receipts and an original boot paired with
   relocated edited streams also fail. Alternate ELF packaging remains rejected
   until all readers use parsed segments.
4. Archive relocation: grow a bank or earlier resident leaf so the player
   address/cursors move. Verify all exported pointers against the corrected
   descriptor load model, and run the actual loader/player path. No old
   `AREA11_SEEDS` or default player-address fallback may survive.
5. Corrected extraction: prove title sound/upload/resident sections are consumed
   from their manifest spans, including material spanning multiple native
   leaves. A stale legacy extraction or matching filename with wrong bytes fails.
6. Model and texture propagation: edit the player and a visible title/status
   texture; verify both raw live and derived representations, capture the
   visible edit, and report any consumer outside the supported coverage.
7. Cache isolation: changed bytes with unchanged mtime, changed exporter helper,
   a reordered conflicting mod stack and changed cue coverage all invalidate
   the right group. A tampered output, copied stale support asset or mixed
   manifest generation is rejected before launch.
8. Pack policy: test COPY/XOR reconstruction, changed base hash, offset overflow,
   incomplete/overlapping output coverage, undeclared FULL bytes, declarations
   bound to the wrong hash, encoded forbidden literals, unknown archive members,
   duplicate/case-colliding paths, expansion limits and output alias attacks.
   A passing declaration is reported as a declaration, never verified authorship.
9. Failure isolation: interrupt export before atomic publication; outputs stay
   unpublished, the original disc and shared port assets remain hash-identical,
   and no child emulator/native process remains. Run native proof headlessly
   with private save paths. Keep only local generated receipts/captures.

Implementation order: shared explicit input context and corrected archive
resolver; narrow boot policy; exporter dependency/manifest wiring; private
asset-set launcher and cache checks; original/modified native runtime proof.
Broader executable patches, new runtime bindings, complete-game coverage and
live asset switching require separate designs and evidence.
