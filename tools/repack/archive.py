"""Lossless DATA.DAT / INDEX.IDX editing for a locally supplied SCUS-97112 disc.

Descriptors are retained verbatim in an ignored template. Loose spans partition
DATA.DAT without guessing at asset-internal padding. Relocation labels are
relative to the resident base (+0x14), as in the game's loaders. Section counts
are the two halfwords at +0x0C and the word at +0x10, not flag bits.
"""
from __future__ import annotations

import hashlib
import json
import os
import struct
import tempfile
from pathlib import Path
from typing import BinaryIO

ROOT = Path(__file__).resolve().parents[2]
OUTPUT_ROOT = ROOT / "build" / "repack"
SECTOR = 0x800
NESTED_BASE = 0x100
NESTED_STRIDE = 0x70
SCHEMA = "extermination-archive-v1"
COPY_SIZE = 1024 * 1024


class ArchiveError(ValueError):
    """Invalid archive, unsafe destination, or unsupported structural edit."""


def _u32(raw: bytes, offset: int) -> int:
    return struct.unpack_from("<I", raw, offset)[0]


def _put32(raw: bytearray, offset: int, value: int) -> None:
    if not 0 <= value <= 0xFFFFFFFF:
        raise ArchiveError("rebuilt archive exceeds a 32-bit descriptor field")
    struct.pack_into("<I", raw, offset, value)


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with Path(path).open("rb") as stream:
        while block := stream.read(COPY_SIZE):
            digest.update(block)
    return digest.hexdigest()


def safe_output(path: Path) -> Path:
    """Resolve a generated output beneath this worktree's build/repack only."""
    resolved = Path(path).resolve()
    root = OUTPUT_ROOT.resolve()
    if not root.is_relative_to(ROOT.resolve()) or not resolved.is_relative_to(root):
        raise ArchiveError("generated output must stay under this worktree's build/repack/")
    if resolved == root:
        raise ArchiveError("choose a child of build/repack/ as the output")
    return resolved


def _input(tree: Path, relative: str) -> Path:
    if not isinstance(relative, str) or not relative:
        raise ArchiveError("manifest contains an invalid input path")
    path = Path(relative)
    if path.is_absolute() or ".." in path.parts:
        raise ArchiveError("manifest contains an unsafe relative path")
    resolved = (tree / path).resolve()
    if not resolved.is_relative_to(tree.resolve()) or not resolved.is_file():
        raise ArchiveError(f"loose input is absent or escapes the tree: {relative}")
    return resolved


def _distinct_outputs(outputs: list[Path], inputs: list[Path]) -> None:
    if len(set(outputs)) != len(outputs):
        raise ArchiveError("DATA.DAT and INDEX.IDX destinations must differ")
    for output in outputs:
        if output.is_dir():
            raise ArchiveError("file destination is a directory")
        for source in inputs:
            if output == source.resolve() or (output.exists() and os.path.samefile(output, source)):
                raise ArchiveError("output aliases an input; choose a separate destination")


def _descriptor(raw: bytes, position: int, capacity: int, label: str,
                sector: int, nested: int | None, data_size: int) -> dict:
    ident, offset, size = struct.unpack_from("<III", raw, position)
    sound_count, transient_count, resident_count = struct.unpack_from("<HHI", raw, position + 12)
    resident, nested_count, file_count = struct.unpack_from("<III", raw, position + 20)
    section_count = sound_count + transient_count + resident_count
    table = 0x20 + 8 * section_count
    if table + 4 * file_count > capacity:
        raise ArchiveError(f"{label}: descriptor tables exceed their fixed slot")
    if resident > size or offset + size > data_size:
        raise ArchiveError(f"{label}: descriptor lies outside DATA.DAT")
    if nested is not None and nested_count:
        raise ArchiveError(f"{label}: recursive nested descriptors are unsupported")
    if any(value % SECTOR for value in (offset, resident)):
        raise ArchiveError(f"{label}: transfer start is not sector aligned")
    sections = []
    resident_cursor = 0
    for number in range(section_count):
        at = position + 0x20 + 8 * number
        section_offset, section_size = struct.unpack_from("<II", raw, at)
        kind = ("sound" if number < sound_count else "transient"
                if number < sound_count + transient_count else "resident_dma")
        if kind == "resident_dma":
            # The runtime uploads these consecutively; its offset word is unused.
            span_offset = resident + resident_cursor
            offset_mode = "resident_relative" if section_offset == resident_cursor else "preserve"
            resident_cursor += section_size
            limit = size
        else:
            span_offset = section_offset
            offset_mode = "block_relative"
            limit = resident
            if section_offset % SECTOR:
                raise ArchiveError(f"{label}: upload start is not sector aligned")
        if span_offset + section_size > limit:
            raise ArchiveError(f"{label}: section exceeds its upload/resident range")
        sections.append(dict(number=number, kind=kind, table_offset=at,
                             offset=section_offset, size=section_size,
                             span_offset=span_offset, offset_mode=offset_mode))
    entries = []
    for number in range(file_count):
        at = position + table + number * 4
        word = _u32(raw, at)
        relative = word & 0xFFFFFF
        if relative > size - resident:
            raise ArchiveError(f"{label}: relocation points past the resident range")
        if entries and relative <= entries[-1]["offset"]:
            raise ArchiveError(f"{label}: overlapping or unsorted relocation entries")
        entries.append(dict(number=number, id=word >> 24, offset=relative, table_offset=at))

    files = []

    def add(start: int, end: int, name: str, kind: str) -> None:
        if end < start:
            raise ArchiveError(f"{label}: overlapping loose spans")
        if end > start:
            files.append(dict(path=f"{label}/{name}.bin", kind=kind, offset=start, size=end - start))

    cursor = 0
    padding_number = 0
    for section in sorted((s for s in sections if s["kind"] != "resident_dma"),
                          key=lambda s: (s["span_offset"], s["number"])):
        start, length = section["span_offset"], section["size"]
        if start < cursor:
            raise ArchiveError(f"{label}: overlapping upload sections are unsupported")
        if start > cursor:
            add(cursor, start, f"padding{padding_number:02d}", "padding")
            padding_number += 1
        add(start, start + length, f"{section['kind']}{section['number']:02d}", section["kind"])
        cursor = start + length
    if cursor < resident:
        add(cursor, resident, f"padding{padding_number:02d}", "padding")
        padding_number += 1
    if entries:
        if entries[0]["offset"]:
            add(resident, resident + entries[0]["offset"], f"padding{padding_number:02d}", "padding")
        for number, entry in enumerate(entries):
            end = resident + entries[number + 1]["offset"] if number + 1 < len(entries) else size
            add(resident + entry["offset"], end, f"f{number:02d}_id{entry['id']:02x}", "resident")
    else:
        add(resident, size, "resident", "opaque")
    if sum(file["size"] for file in files) != size:
        raise ArchiveError(f"{label}: loose spans do not cover the region")
    return dict(label=label, sector=sector, nested=nested, id=ident,
                descriptor_offset=position, offset=offset, size=size,
                resident_offset=resident, nested_count=nested_count,
                sound_count=sound_count, transient_count=transient_count,
                resident_dma_count=resident_count, section_table_offset=position + 0x20,
                file_table_offset=position + table, entries=entries, sections=sections, files=files)


def parse_index(raw: bytes, data_size: int) -> tuple[list[dict], list[dict]]:
    """Validate descriptor bounds and return lossless, resident-correct metadata."""
    if not raw or len(raw) % SECTOR:
        raise ArchiveError("INDEX.IDX must contain complete 0x800-byte sectors")
    if not isinstance(data_size, int) or data_size < 0 or data_size > 0xFFFFFFFF:
        raise ArchiveError("invalid DATA.DAT length")
    regions = []
    for sector in range(len(raw) // SECTOR):
        at = sector * SECTOR
        count = _u32(raw, at + 0x18)
        if NESTED_BASE + count * NESTED_STRIDE > SECTOR:
            raise ArchiveError(f"chunk{sector:02d}: nested descriptors exceed the index sector")
        capacity = NESTED_BASE if count else SECTOR
        regions.append(_descriptor(raw, at, capacity, f"chunk{sector:02d}", sector, None, data_size))
        for nested in range(count):
            position = at + NESTED_BASE + nested * NESTED_STRIDE
            regions.append(_descriptor(raw, position, NESTED_STRIDE,
                                       f"chunk{sector:02d}.n{nested}", sector, nested, data_size))
    layout = []
    cursor = 0
    for region in sorted((r for r in regions if r["size"]), key=lambda r: r["offset"]):
        if region["offset"] < cursor:
            raise ArchiveError("DATA.DAT regions overlap")
        if region["offset"] > cursor:
            layout.append(dict(kind="gap", path=f"padding/data_{cursor:08x}.bin",
                               offset=cursor, size=region["offset"] - cursor))
        layout.append(dict(kind="region", label=region["label"], offset=region["offset"], size=region["size"]))
        cursor = region["offset"] + region["size"]
    if cursor < data_size:
        layout.append(dict(kind="gap", path=f"padding/data_{cursor:08x}.bin", offset=cursor, size=data_size - cursor))
    return regions, layout


def _copy_span(source: BinaryIO, output: BinaryIO, size: int, digest=None) -> str:
    local = hashlib.sha256()
    while size:
        block = source.read(min(size, COPY_SIZE))
        if not block:
            raise ArchiveError("input ended before its declared length")
        output.write(block)
        local.update(block)
        if digest is not None:
            digest.update(block)
        size -= len(block)
    return local.hexdigest()


def unpack_archive(data_path: Path, index_path: Path, out_dir: Path) -> dict:
    """Extract every byte to a new loose tree; original inputs are read-only."""
    data_path, index_path = Path(data_path).resolve(), Path(index_path).resolve()
    out_dir = safe_output(out_dir)
    if out_dir.exists() and (not out_dir.is_dir() or any(out_dir.iterdir())):
        raise ArchiveError("unpack destination must be a new or empty directory")
    if data_path.is_relative_to(out_dir) or index_path.is_relative_to(out_dir):
        raise ArchiveError("unpack destination contains an input")
    raw = index_path.read_bytes()
    data_size = data_path.stat().st_size
    regions, layout = parse_index(raw, data_size)
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "index.template.bin").write_bytes(raw)
    by_label = {region["label"]: region for region in regions}
    digest = hashlib.sha256()
    with data_path.open("rb") as source:
        for item in layout:
            if item["kind"] == "gap":
                spans = [item]
                base = 0
            else:
                region = by_label[item["label"]]
                spans = region["files"]
                base = region["offset"]
            for span in spans:
                target = out_dir / span["path"]
                target.parent.mkdir(parents=True, exist_ok=True)
                source.seek(base + span["offset"])
                with target.open("xb") as output:
                    span["sha256"] = _copy_span(source, output, span["size"], digest)
    manifest = dict(schema=SCHEMA, sector_size=SECTOR, index_template="index.template.bin",
                    index_sha256=hashlib.sha256(raw).hexdigest(), index_size=len(raw),
                    data_sha256=digest.hexdigest(), data_size=data_size,
                    source_paths=dict(data=str(data_path), index=str(index_path)),
                    regions=regions, data_layout=layout)
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def _without_hashes(value):
    if isinstance(value, dict):
        return {key: _without_hashes(item) for key, item in value.items() if key != "sha256"}
    if isinstance(value, list):
        return [_without_hashes(item) for item in value]
    return value



def _additions(tree: Path, regions: list[dict], raw: bytes) -> tuple[dict[str, list[dict]], Path | None]:
    path = tree / "edits.json"
    if path.is_symlink():
        raise ArchiveError("archive edit metadata must not be a symlink")
    if not path.exists():
        return {}, None
    path = _input(tree, "edits.json")
    edits = json.loads(path.read_text())
    if not isinstance(edits, dict) or set(edits) != {"schema", "additions"} or edits["schema"] != "extermination-archive-edits-v1":
        raise ArchiveError("invalid archive edits schema")
    if not isinstance(edits["additions"], list):
        raise ArchiveError("archive additions must be a list")
    by_label = {r["label"]: r for r in regions}
    result = {}
    for edit in edits["additions"]:
        if not isinstance(edit, dict) or set(edit) != {"region", "id", "path"}:
            raise ArchiveError("invalid archive addition")
        if not isinstance(edit["region"], str):
            raise ArchiveError("archive addition region must be a label")
        region = by_label.get(edit["region"])
        if region is None or not region["entries"]:
            raise ArchiveError("new entries require an existing resident file table")
        group = result.setdefault(region["label"], [])
        ident = edit["id"]
        if type(ident) is not int or not 0 <= ident <= 255:
            raise ArchiveError("new resident ID must fit one byte")
        if ident in {e["id"] for e in region["entries"] + group}:
            raise ArchiveError("new resident ID duplicates a role already in the region")
        ordinal = len(region["entries"]) + len(group)
        expected = f"{region['label']}/f{ordinal:02d}_id{ident:02x}.bin"
        if edit["path"] != expected:
            raise ArchiveError("new resident path does not match its table ordinal and ID")
        capacity = NESTED_STRIDE if region["nested"] is not None else NESTED_BASE if region["nested_count"] else SECTOR
        position = region["file_table_offset"] + ordinal * 4
        if position + 4 > region["descriptor_offset"] + capacity:
            raise ArchiveError("new resident entry exceeds the fixed INDEX descriptor slot")
        if any(raw[position:position + 4]):
            raise ArchiveError("new resident entry would overwrite nonzero unknown INDEX bytes")
        _input(tree, expected)
        group.append(dict(path=expected, kind="resident", id=ident, table_offset=position))
    return result, path


def add_entry(tree_dir: Path, region_label: str, ident: int, source: Path) -> dict:
    """Append a resident role entry using verified zero slack in its INDEX slot."""
    tree = safe_output(tree_dir)
    manifest = json.loads(_input(tree, "manifest.json").read_text())
    raw = _input(tree, manifest["index_template"]).read_bytes()
    if hashlib.sha256(raw).hexdigest() != manifest["index_sha256"]:
        raise ArchiveError("index template changed")
    regions, _ = parse_index(raw, manifest["data_size"])
    additions, edit_path = _additions(tree, regions, raw)
    region = next((r for r in regions if r["label"] == region_label), None)
    if region is None or not region["entries"]:
        raise ArchiveError("choose a region with an existing resident file table")
    if type(ident) is not int or not 0 <= ident <= 255:
        raise ArchiveError("new resident ID must fit one byte")
    ordinal = len(region["entries"]) + len(additions.get(region_label, []))
    relative = f"{region_label}/f{ordinal:02d}_id{ident:02x}.bin"
    if (tree / relative).is_symlink():
        raise ArchiveError("new resident destination must not be a symlink")
    target = safe_output(tree / relative)
    if not target.is_relative_to(tree):
        raise ArchiveError("new resident destination escapes the tree")
    if target.exists():
        raise ArchiveError("new resident destination already exists")
    source = Path(source).resolve()
    if not source.is_file() or source.stat().st_size == 0:
        raise ArchiveError("new resident source must be a nonempty file")
    edits = json.loads(edit_path.read_text()) if edit_path else dict(schema="extermination-archive-edits-v1", additions=[])
    edit = dict(region=region_label, id=ident, path=relative)
    edits["additions"].append(edit)
    old_edits = edit_path.read_bytes() if edit_path else None
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open("xb") as stream:
        stream.write(source.read_bytes())
    destination = tree / "edits.json"

    def replace_edits(data):
        fd, name = tempfile.mkstemp(prefix=".edits-", dir=tree)
        try:
            with os.fdopen(fd, "wb") as stream:
                stream.write(data)
            os.replace(name, destination)
        finally:
            Path(name).unlink(missing_ok=True)

    try:
        replace_edits((json.dumps(edits, indent=2) + "\n").encode())
        _additions(tree, regions, raw)
    except BaseException:
        target.unlink()
        if old_edits is None:
            destination.unlink(missing_ok=True)
        else:
            replace_edits(old_edits)
        raise
    return edit

def pack_archive(tree_dir: Path, out_data: Path, out_index: Path) -> dict:
    """Rebuild byte-sized edits and appended resident entries with aligned transfers.

    Alignment is explicit padding outside payload spans. Asset-internal offsets
    remain the editor's responsibility; ambiguous interior DMA boundaries reject.
    """
    tree = Path(tree_dir).resolve()
    manifest_path = _input(tree, "manifest.json")
    manifest = json.loads(manifest_path.read_text())
    if not isinstance(manifest, dict):
        raise ArchiveError("archive manifest must be an object")
    if manifest.get("schema") != SCHEMA or manifest.get("sector_size") != SECTOR:
        raise ArchiveError("unsupported archive manifest schema")
    template_path = _input(tree, manifest.get("index_template", ""))
    raw = template_path.read_bytes()
    if len(raw) != manifest.get("index_size") or hashlib.sha256(raw).hexdigest() != manifest.get("index_sha256"):
        raise ArchiveError("index template changed; restore it before packing")
    regions, layout = parse_index(raw, manifest.get("data_size"))
    if (_without_hashes(manifest.get("regions")) != regions
            or _without_hashes(manifest.get("data_layout")) != layout):
        raise ArchiveError("manifest structure changed; edit loose files, not descriptor metadata")
    additions, edit_path = _additions(tree, regions, raw)
    index = bytearray(raw)
    by_label = {region["label"]: region for region in regions}
    sizes = {}
    inputs = [manifest_path, template_path] + ([edit_path] if edit_path else [])
    source_paths = manifest.get("source_paths", {})
    if not isinstance(source_paths, dict):
        raise ArchiveError("invalid original source paths in manifest")
    for value in source_paths.values():
        if not isinstance(value, str):
            raise ArchiveError("invalid original source path in manifest")
        original = Path(value)
        if original.is_file():
            inputs.append(original.resolve())
    changed_files = []
    ordered = []
    for region in regions:
        ordered.extend(region["files"])
        ordered.extend(additions.get(region["label"], []))
    ordered.extend(item for item in layout if item["kind"] == "gap")
    for span in ordered:
        source = _input(tree, span["path"])
        inputs.append(source)
        size = source.stat().st_size
        if size == 0:
            raise ArchiveError(f"{span['path']}: removing a loose span is unsupported")
        sizes[span["path"]] = size
        if size != span.get("size", 0):
            changed_files.append(dict(path=span["path"], old_size=span.get("size", 0), new_size=size))
    out_data, out_index = safe_output(out_data), safe_output(out_index)
    _distinct_outputs([out_data, out_index], inputs)

    def region_map(region: dict):
        starts, ends = {0: 0}, {0: 0}
        cursor, spans = 0, []
        for span in region["files"]:
            if span["kind"] in ("sound", "transient") or span["offset"] == region["resident_offset"]:
                padding = (-cursor) % SECTOR
                if padding:
                    spans.append(dict(zero=padding))
                    cursor += padding
            starts[span["offset"]] = cursor
            spans.append(span)
            cursor += sizes[span["path"]]
            ends[span["offset"] + span["size"]] = cursor
        starts.setdefault(region["size"], cursor)
        ends.setdefault(region["size"], cursor)

        def remap(offset: int, *, end=False) -> int:
            boundaries = ends if end else starts
            if offset in boundaries:
                return boundaries[offset]
            for span in region["files"]:
                if span["offset"] < offset < span["offset"] + span["size"]:
                    if sizes[span["path"]] != span["size"]:
                        raise ArchiveError(f"{span['path']}: resized span contains an interior section boundary")
                    return starts[span["offset"]] + offset - span["offset"]
            raise ArchiveError("descriptor boundary is outside its loose spans")

        for span in additions.get(region["label"], []):
            span["rebuilt_offset"] = cursor
            spans.append(span)
            cursor += sizes[span["path"]]
        return remap, cursor, spans

    cursor = 0
    write_order = []
    moved_regions = []
    data_boundaries = {0: 0}
    for item in layout:
        if item["kind"] == "gap":
            data_boundaries[item["offset"]] = cursor
            write_order.append(item)
            cursor += sizes[item["path"]]
            data_boundaries[item["offset"] + item["size"]] = cursor
            continue
        padding = (-cursor) % SECTOR
        if padding:
            write_order.append(dict(zero=padding))
            cursor += padding
        data_boundaries[item["offset"]] = cursor
        region = by_label[item["label"]]
        remap, size, spans = region_map(region)
        resident = remap(region["resident_offset"])
        at = region["descriptor_offset"]
        if resident % SECTOR:
            # An upload-only region's empty resident base is still a sector
            # anchor. Its trailing padding belongs to the descriptor itself.
            if region["resident_offset"] != region["size"]:
                raise ArchiveError(f"{region['label']}: rebuilt resident start is not sector aligned")
            padding = (-resident) % SECTOR
            spans.append(dict(zero=padding))
            resident += padding
            size += padding
        _put32(index, at + 4, cursor)
        _put32(index, at + 8, size)
        _put32(index, at + 20, resident)
        for section in region["sections"]:
            start = remap(section["span_offset"])
            end = start if section["size"] == 0 else remap(section["span_offset"] + section["size"], end=True)
            if section["offset_mode"] == "block_relative":
                if start % SECTOR:
                    raise ArchiveError("rebuilt upload is not sector aligned")
                _put32(index, section["table_offset"], start)
            elif section["offset_mode"] == "resident_relative":
                _put32(index, section["table_offset"], start - resident)
            _put32(index, section["table_offset"] + 4, end - start)
        for entry in region["entries"]:
            relative = remap(region["resident_offset"] + entry["offset"]) - resident
            if relative > 0xFFFFFF:
                raise ArchiveError(f"{region['label']}: resident relocation exceeds the 24-bit limit")
            _put32(index, entry["table_offset"], entry["id"] << 24 | relative)
        extra = additions.get(region["label"], [])
        for entry in extra:
            relative = entry["rebuilt_offset"] - resident
            if relative > 0xFFFFFF:
                raise ArchiveError(f"{region['label']}: new resident offset exceeds the 24-bit limit")
            _put32(index, entry["table_offset"], entry["id"] << 24 | relative)
        _put32(index, at + 0x1C, len(region["entries"]) + len(extra))
        if region["offset"] != cursor or region["size"] != size:
            moved_regions.append(dict(label=region["label"], offset=cursor, size=size))
        cursor += size
        data_boundaries[item["offset"] + item["size"]] = cursor
        write_order.extend(spans)
    # Keep the physical DATA file sector sized; the last leaf's exact length
    # remains in its region size, with any new tail a separate padding span.
    padding = (-cursor) % SECTOR
    if padding:
        write_order.append(dict(zero=padding))
        cursor += padding
    data_boundaries[manifest["data_size"]] = cursor
    if cursor > 0xFFFFFFFF:
        raise ArchiveError("DATA.DAT exceeds the 32-bit archive size limit")
    # Empty top descriptors can still point at the first nested region. Keep
    # their anchors consistent when preceding regions move.
    for region in regions:
        if region["size"]:
            continue
        offset = region["offset"]
        if offset not in data_boundaries:
            if changed_files:
                raise ArchiveError(f"{region['label']}: empty descriptor has an interior anchor; cannot reflow")
            relocated = offset
        else:
            relocated = data_boundaries[offset]
        _put32(index, region["descriptor_offset"] + 4, relocated)
        if relocated != offset:
            moved_regions.append(dict(label=region["label"], offset=relocated, size=0))
    parse_index(bytes(index), cursor)
    pending = []
    data_digest = hashlib.sha256()
    try:
        for output in (out_data, out_index):
            output.parent.mkdir(parents=True, exist_ok=True)
            handle, name = tempfile.mkstemp(prefix=f".{output.name}.", dir=output.parent)
            os.close(handle)
            pending.append(Path(name))
        with pending[0].open("wb") as output:
            for span in write_order:
                if "zero" in span:
                    block = bytes(span["zero"])
                    output.write(block)
                    data_digest.update(block)
                    continue
                with _input(tree, span["path"]).open("rb") as source:
                    _copy_span(source, output, sizes[span["path"]], data_digest)
                    if source.read(1):
                        raise ArchiveError("loose file grew while packing")
        pending[1].write_bytes(index)
        os.replace(pending[0], out_data)
        os.replace(pending[1], out_index)
    finally:
        for temporary in pending:
            temporary.unlink(missing_ok=True)
    return dict(data_path=str(out_data), index_path=str(out_index), data_size=cursor,
                index_size=len(index), data_sha256=data_digest.hexdigest(),
                index_sha256=hashlib.sha256(index).hexdigest(), resized_files=changed_files,
                moved_regions=moved_regions, added_entries=sum(map(len, additions.values())))
