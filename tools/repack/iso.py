"""Lossless, independent ISO9660 file tree for user-supplied PS2 images.

The image is partitioned into editable file bytes and preserved non-file
spans. Packing needs that tree only, never the original image. This intentionally
supports the single-volume, 2048-byte ISO9660/UDF bridge used by SCUS-97112;
alternate ISO descriptors, interleaving, aliases and multi-extent files fail closed.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import struct
import tempfile

from . import udf

SECTOR = 2048
SCHEMA = "extermination-iso-v1"
RESIZABLE = {"DATA/DATA.DAT", "DATA/INDEX.IDX"}
BUFFER = 8 << 20
ROOT = Path(__file__).resolve().parents[2]
OUTPUT_ROOT = ROOT / "build" / "repack"


def _safe_output(path: Path) -> Path:
    resolved, root = Path(path).resolve(), OUTPUT_ROOT.resolve()
    if not root.is_relative_to(ROOT) or not resolved.is_relative_to(root) or resolved == root:
        raise ValueError("choose an output beneath this worktree's build/repack/")
    return resolved


def _both(buf: bytes, at: int, width: int = 4) -> int:
    little = int.from_bytes(buf[at:at + width], "little")
    big = int.from_bytes(buf[at + width:at + 2 * width], "big")
    if little != big:
        raise ValueError(f"ISO both-endian fields disagree at +{at:#x}")
    return little


def _path(name: str) -> str:
    path = PurePosixPath(name)
    if (not name or path.is_absolute() or "\\" in name or
            any(x in (".", "..", "") for x in name.split("/"))):
        raise ValueError(f"unsafe ISO/tree path: {name!r}")
    return str(path)


def _inside(root: Path, name: str) -> Path:
    candidate = root / _path(name)
    if not candidate.resolve().is_relative_to(root.resolve()):
        raise ValueError(f"tree path escapes root: {name!r}")
    return candidate


def _digest(handle, offset: int, size: int) -> str:
    handle.seek(offset)
    h = hashlib.sha256()
    remaining = size
    while remaining:
        block = handle.read(min(BUFFER, remaining))
        if not block:
            raise ValueError("unexpected end of image")
        h.update(block)
        remaining -= len(block)
    return h.hexdigest()


def _copy(handle, destination, offset: int, size: int) -> None:
    handle.seek(offset)
    while size:
        block = handle.read(min(BUFFER, size))
        if not block:
            raise ValueError("unexpected end of input")
        destination.write(block)
        size -= len(block)


def _classify(path: str) -> str:
    if path == "SCUS_971.12" or path.startswith("OVERLAY/"):
        return "rebuilt-source"
    if path in RESIZABLE:
        return "known-archive"
    if path.startswith(("MOVIE/", "STREAM/")) or path == "EXTER1.DAT":
        return "stream"
    return "opaque"


def inventory(image: Path) -> dict:
    """Validate the supported ISO layout and describe every file and table."""
    image = Path(image)
    size = image.stat().st_size
    if size < 18 * SECTOR or size % SECTOR:
        raise ValueError("expected a sector-aligned ISO9660 image")
    files, directories, descriptors = [], [], []
    seen_dirs, seen_paths = set(), set()
    protected = [(0, 16 * SECTOR, "system area")]
    with image.open("rb") as src:
        pvd = None
        for sector in range(16, min(size // SECTOR, 256)):
            src.seek(sector * SECTOR)
            vd = src.read(SECTOR)
            if vd[1:7] != b"CD001\x01":
                raise ValueError("not a supported ISO9660 volume descriptor")
            descriptors.append({"sector": sector, "type": vd[0]})
            protected.append((sector * SECTOR, (sector + 1) * SECTOR, "descriptor"))
            if vd[0] == 255:
                break
            if vd[0] != 1 or pvd is not None:
                raise ValueError("only one primary descriptor, without supplementary/boot volumes, is supported")
            pvd = vd
            pvd_offset = sector * SECTOR
        else:
            raise ValueError("missing ISO descriptor terminator")
        if pvd is None:
            raise ValueError("missing primary volume descriptor")
        if _both(pvd, 128, 2) != SECTOR:
            raise ValueError("ISO logical block size must be 2048")
        if _both(pvd, 120, 2) != 1 or _both(pvd, 124, 2) != 1:
            raise ValueError("multi-volume ISO sets are unsupported")
        volume_sectors = _both(pvd, 80)
        if volume_sectors * SECTOR > size:
            raise ValueError("declared ISO volume extends beyond image")
        table_size = _both(pvd, 132)
        tables = []
        for at, endian, optional in ((140, "little", False), (144, "little", True),
                                      (148, "big", False), (152, "big", True)):
            sector = int.from_bytes(pvd[at:at + 4], endian)
            if not sector and optional:
                continue
            if not sector or sector * SECTOR + table_size > volume_sectors * SECTOR:
                raise ValueError("invalid ISO path-table extent")
            tables.append({"sector": sector, "size": table_size, "endian": endian,
                           "optional": optional, "pointer_offset": pvd_offset + at})
            protected.append((sector * SECTOR, sector * SECTOR + table_size, "path table"))

        def record(raw: bytes, offset: int) -> tuple[int, int, int, bytes]:
            if len(raw) < 34 or raw[0] != len(raw) or 33 + raw[32] > len(raw):
                raise ValueError(f"malformed directory record at {offset:#x}")
            if raw[1] or raw[26] or raw[27] or raw[25] & 0x80:
                raise ValueError("extended-attribute/interleaved/multi-extent files unsupported")
            if _both(raw, 28, 2) != 1:
                raise ValueError("directory record references another ISO volume")
            extent, length = _both(raw, 2), _both(raw, 10)
            if extent * SECTOR + length > volume_sectors * SECTOR:
                raise ValueError("directory extent outside declared ISO volume")
            return extent * SECTOR, length, raw[25], raw[33:33 + raw[32]]

        def walk(raw: bytes, where: int, prefix: str) -> None:
            offset, length, flags, _ = record(raw, where)
            if not flags & 2 or length == 0 or offset in seen_dirs:
                raise ValueError("invalid or aliased ISO directory")
            seen_dirs.add(offset)
            directories.append({"path": prefix.rstrip("/") or "/", "offset": offset, "sector": offset // SECTOR,
                                "size": length, "record": where})
            protected.append((offset, offset + length, "directory"))
            src.seek(offset)
            content = src.read(length)
            at = 0
            while at < length:
                n = content[at]
                if not n:
                    at = (at // SECTOR + 1) * SECTOR
                    continue
                if at + n > length or at // SECTOR != (at + n - 1) // SECTOR:
                    raise ValueError("directory record crosses a sector boundary")
                entry = content[at:at + n]
                location = offset + at
                extent, file_size, entry_flags, name = record(entry, location)
                at += n
                if name in (b"\x00", b"\x01"):
                    continue
                try:
                    raw_name = name.decode("ascii")
                except UnicodeDecodeError as exc:
                    raise ValueError("non-ASCII ISO9660 filename") from exc
                path = _path(prefix + raw_name.split(";", 1)[0])
                if path in seen_paths:
                    raise ValueError(f"duplicate normalized ISO path: {path}")
                seen_paths.add(path)
                if entry_flags & 2:
                    walk(entry, location, path + "/")
                else:
                    files.append({"path": path, "iso_name": raw_name, "offset": extent, "sector": extent // SECTOR,
                                  "size": file_size, "records": [location],
                                  "classification": _classify(path)})

        n = pvd[156]
        walk(pvd[156:156 + n], pvd_offset + 156, "")
        expected_directories = {d["path"]: d["sector"] for d in directories}
        reference_entries = None
        for table in tables:
            src.seek(table["sector"] * SECTOR)
            raw = src.read(table["size"])
            entries, at = [], 0
            while at < len(raw):
                if at + 8 > len(raw):
                    raise ValueError("truncated ISO path-table entry")
                length = raw[at]
                end = at + 8 + length + length % 2
                if not length or raw[at + 1] or end > len(raw):
                    raise ValueError("invalid ISO path-table entry")
                extent = int.from_bytes(raw[at + 2:at + 6], table["endian"])
                parent = int.from_bytes(raw[at + 6:at + 8], table["endian"])
                name = raw[at + 8:at + 8 + length]
                if not entries:
                    if name != b"\x00" or parent != 1:
                        raise ValueError("invalid ISO path-table root")
                    path = "/"
                else:
                    if not 1 <= parent <= len(entries):
                        raise ValueError("invalid ISO path-table parent index")
                    try:
                        component = name.decode("ascii")
                    except UnicodeDecodeError as exc:
                        raise ValueError("non-ASCII path-table name") from exc
                    path = _path(entries[parent - 1]["path"].strip("/") + "/" + component
                                 if entries[parent - 1]["path"] != "/" else component)
                entries.append({"path": path, "sector": extent, "parent_index": parent})
                at = end
            if {e["path"]: e["sector"] for e in entries} != expected_directories:
                raise ValueError("ISO path table disagrees with directory tree")
            if len(entries) != len(directories) or (reference_entries is not None and entries != reference_entries):
                raise ValueError("ISO path-table copies disagree")
            reference_entries = entries
            table["entries"] = entries
        udf_manifest = udf.inspect(src, size, files)
        # Ensure no directory/metadata block can be overwritten by editable files.
        intervals = sorted(protected + [(f["offset"], f["offset"] + f["size"], f["path"])
                                        for f in files if f["size"]])
        for previous, current in zip(intervals, intervals[1:]):
            if previous[1] > current[0]:
                raise ValueError(f"overlapping ISO extents: {previous[2]} / {current[2]}")
        udf_intervals = []
        if udf_manifest:
            for extent in udf_manifest["metadata_ranges"]:
                start, end = extent["offset"], extent["offset"] + extent["size"]
                for entry in files:
                    if entry["size"] and start < entry["offset"] + entry["size"] and entry["offset"] < end:
                        raise ValueError(f"UDF metadata overlaps ISO file {entry['path']}")
                udf_intervals.append((start, end, "UDF metadata"))
        for entry in files:
            entry["sha256"] = _digest(src, entry["offset"], entry["size"])
            # Growth may consume only that file's trailing sector padding.
            entry["capacity"] = ((entry["size"] + SECTOR - 1) // SECTOR) * SECTOR
            next_protected = min((start for start, _, _ in intervals + udf_intervals if start > entry["offset"]),
                                 default=volume_sectors * SECTOR)
            entry["capacity"] = min(entry["capacity"], next_protected - entry["offset"])
            padding_size = entry["capacity"] - entry["size"]
            src.seek(entry["offset"] + entry["size"])
            padding = src.read(padding_size)
            entry["sector_padding_size"] = padding_size
            entry["sector_padding_zero"] = not any(padding)
        digest = _digest(src, 0, size)
    files.sort(key=lambda f: f["offset"])
    counts = {}
    for entry in files:
        category = entry["classification"]
        counts[category] = counts.get(category, 0) + 1
    return {"schema": SCHEMA, "image_size": size, "image_sha256": digest,
            "sector_size": SECTOR, "volume_sectors": volume_sectors,
            "pvd_offset": pvd_offset, "descriptors": descriptors,
            "path_tables": tables, "directories": directories,
            "udf": udf_manifest,
            "files": files, "classification_counts": counts,
            "layout_rules": [
                "2048-byte sectors; file extents and directories start on sector boundaries.",
                "Directory records: +2 extent and +10 byte length are u32 little then big endian; +28 volume sequence is u16 both endian.",
                "PVD +80 volume sectors is u32 both endian; +120/+124 set size/sequence and +128 block size are u16 both endian.",
                "PVD +132 path-table byte size is u32 both endian; +140/+144 little-endian and +148/+152 big-endian path-table sector pointers.",
                "Path-table entries: byte name length, byte extended-attribute length, u32 directory extent, u16 parent index, name and even-byte padding; endian follows table.",
                "Directory records start with byte record length; +25 flags, +32 identifier length, +33 identifier; zero record length advances to next sector.",
                "Directory topology, path tables, timestamps, system-use bytes, system area, gaps and sector padding are preserved verbatim.",
                "Only DATA/DATA.DAT and DATA/INDEX.IDX may change size; oversized files append at sector boundary and update directory extent/length and volume size.",
                "A UDF bridge, when present, mirrors every file extent/size; growth also updates UDF allocation descriptors, partition/integrity sizes, anchor location and descriptor CRC/checksum.",
                "Streams and opaque files remain whole editable files; stream cue tables in the executable are not rewritten."]}


def unpack(image: Path, out_dir: Path) -> dict:
    """Write loose files, non-file spans and a complete reconstruction manifest."""
    image, out_dir = Path(image), _safe_output(out_dir)
    if out_dir.exists() and any(out_dir.iterdir()):
        raise ValueError(f"unpack destination must be empty: {out_dir}")
    manifest = inventory(image)
    out_dir.mkdir(parents=True, exist_ok=True)
    spans, cursor = [], 0
    with image.open("rb") as src:
        for index, entry in enumerate(manifest["files"]):
            if entry["offset"] > cursor:
                spans.append({"offset": cursor, "size": entry["offset"] - cursor,
                              "path": f"metadata/span{index:04d}.bin"})
            path = _inside(out_dir, "files/" + entry["path"])
            path.parent.mkdir(parents=True, exist_ok=True)
            with path.open("wb") as dst:
                _copy(src, dst, entry["offset"], entry["size"])
            cursor = max(cursor, entry["offset"] + entry["size"])
        if cursor < manifest["image_size"]:
            spans.append({"offset": cursor, "size": manifest["image_size"] - cursor,
                          "path": "metadata/trailer.bin"})
        for span in spans:
            path = _inside(out_dir, span["path"])
            path.parent.mkdir(parents=True, exist_ok=True)
            with path.open("wb") as dst:
                _copy(src, dst, span["offset"], span["size"])
            span["sha256"] = _digest(src, span["offset"], span["size"])
    manifest["spans"] = spans
    manifest["source_image"] = str(image.resolve())
    (out_dir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def _write_both(dst, offset: int, value: int) -> None:
    if not 0 <= value <= 0xffffffff:
        raise ValueError("ISO u32 field overflow")
    dst.seek(offset)
    dst.write(struct.pack("<I", value) + struct.pack(">I", value))


def pack(tree: Path, out_image: Path, overrides: dict[str, Path] | None = None) -> dict:
    """Reassemble a loose tree; permitted resized archive files update ISO tables.

    Override keys use unversioned paths, e.g. ``DATA/DATA.DAT``. Output is
    atomically replaced only after reconstruction and structural verification.
    """
    tree = Path(tree)
    if Path(out_image).is_symlink():
        raise ValueError("refusing to overwrite an output symlink")
    out_image = _safe_output(out_image)
    manifest = json.loads((tree / "manifest.json").read_text())
    if manifest.get("schema") != SCHEMA or manifest.get("sector_size") != SECTOR:
        raise ValueError("unsupported ISO manifest")
    if out_image.resolve().is_relative_to(tree.resolve()):
        raise ValueError("output ISO must be outside the loose input tree")
    original = Path(manifest.get("source_image", tree))
    if (out_image.is_symlink() or out_image.resolve() == original.resolve() or
            (out_image.exists() and original.exists() and out_image.samefile(original))):
        raise ValueError("refusing to overwrite the original image or an output symlink")
    overrides = {_path(key.lstrip("/")): Path(value) for key, value in (overrides or {}).items()}
    names = {entry["path"] for entry in manifest["files"]}
    if overrides.keys() - names:
        raise ValueError(f"unknown ISO overrides: {sorted(overrides.keys() - names)}")
    plan, inputs = [], []
    final_size = manifest["image_size"]
    # The manifest must partition the entire original image without overlap.
    intervals = [(entry["offset"], entry["offset"] + entry["size"])
                 for entry in manifest["files"] + manifest["spans"] if entry["size"]]
    cursor = 0
    for start, end in sorted(intervals):
        if start != cursor or end < start:
            raise ValueError("manifest extents do not exactly tile the original image")
        cursor = end
    if cursor != final_size:
        raise ValueError("manifest image size disagrees with its extents")
    for entry in manifest["files"]:
        path = overrides.get(entry["path"], _inside(tree, "files/" + entry["path"]))
        size = path.stat().st_size
        if size > 0xffffffff:
            raise ValueError("single ISO9660 file exceeds u32 length")
        if size != entry["size"] and entry["path"] not in RESIZABLE:
            raise ValueError(f"size changes unsupported for {entry['path']}; executable/stream internal tables are not rewritten")
        offset = entry["offset"]
        if size > entry["capacity"]:
            offset = ((final_size + SECTOR - 1) // SECTOR) * SECTOR
            final_size = offset + ((size + SECTOR - 1) // SECTOR) * SECTOR
        plan.append((entry, path, offset, size))
        inputs.append(path.resolve())
    spans = []
    for span in manifest["spans"]:
        path = _inside(tree, span["path"])
        if path.stat().st_size != span["size"]:
            raise ValueError("preserved ISO metadata span changed length")
        with path.open("rb") as src:
            if _digest(src, 0, span["size"]) != span["sha256"]:
                raise ValueError("preserved ISO metadata span changed bytes")
        spans.append((span, path))
        inputs.append(path.resolve())
    if out_image.resolve() in inputs or (out_image.exists() and any(out_image.samefile(path) for path in inputs)):
        raise ValueError("output ISO aliases an input")
    out_image.parent.mkdir(parents=True, exist_ok=True)
    fd, temporary = tempfile.mkstemp(prefix=".repack-", suffix=".iso", dir=out_image.parent)
    try:
        with os.fdopen(fd, "w+b") as dst:
            dst.truncate(final_size)
            for span, path in spans:
                dst.seek(span["offset"])
                with path.open("rb") as src:
                    _copy(src, dst, 0, span["size"])
            changes = []
            for entry, path, offset, size in plan:
                dst.seek(offset)
                with path.open("rb") as src:
                    _copy(src, dst, 0, size)
                if size != entry["size"] or offset != entry["offset"]:
                    for record in entry["records"]:
                        _write_both(dst, record + 2, offset // SECTOR)
                        _write_both(dst, record + 10, size)
                    changes.append({"path": entry["path"], "old_offset": entry["offset"],
                                    "new_offset": offset, "old_size": entry["size"], "new_size": size})
            if manifest.get("udf"):
                final_size = udf.patch(dst, manifest["udf"], changes, manifest["image_size"], final_size)
                dst.truncate(final_size)
            if final_size != manifest["image_size"]:
                _write_both(dst, manifest["pvd_offset"] + 80, final_size // SECTOR)
        verified = inventory(Path(temporary))
        if {f["path"] for f in verified["files"]} != names:
            raise ValueError("packed ISO directory names changed unexpectedly")
        for entry, path, offset, size in plan:
            actual = next(f for f in verified["files"] if f["path"] == entry["path"])
            if actual["offset"] != offset or actual["size"] != size:
                raise ValueError(f"packed ISO record disagrees for {entry['path']}")
            with path.open("rb") as src:
                if _digest(src, 0, size) != actual["sha256"]:
                    raise ValueError(f"packed ISO bytes disagree for {entry['path']}")
        os.replace(temporary, out_image)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)
    return {"image_size": final_size, "image_sha256": verified["image_sha256"],
            "unchanged": verified["image_sha256"] == manifest["image_sha256"],
            "resized_files": changes, "files": verified["files"]}
