"""Inspect and relocate the read-only UDF 1.02 bridge on a user's local disc.

This supports one physical partition, ordinary file entries with one recorded
short allocation descriptor, and matching ISO9660/UDF file extents. Unsupported
allocation schemes fail closed. All untouched descriptor bytes are preserved.
"""
from __future__ import annotations

import binascii
import struct

SECTOR = 2048


def _u16(raw, offset):
    return struct.unpack_from("<H", raw, offset)[0]


def _u32(raw, offset):
    return struct.unpack_from("<I", raw, offset)[0]


def _u64(raw, offset):
    return struct.unpack_from("<Q", raw, offset)[0]


def _tag(raw, ident=None, location=None):
    if len(raw) < 16 or _u16(raw, 2) != 2 or raw[5]:
        raise ValueError("unsupported UDF descriptor tag")
    if (sum(raw[:4]) + sum(raw[5:16])) & 255 != raw[4]:
        raise ValueError("UDF tag checksum mismatch")
    length = _u16(raw, 10)
    if 16 + length > len(raw) or binascii.crc_hqx(raw[16:16 + length], 0) != _u16(raw, 8):
        raise ValueError("UDF descriptor CRC mismatch")
    if ident is not None and _u16(raw, 0) != ident:
        raise ValueError("unexpected UDF descriptor kind")
    if location is not None and _u32(raw, 12) != location:
        raise ValueError("UDF descriptor location mismatch")
    return _u16(raw, 0)


def _retag(raw):
    length = _u16(raw, 10)
    struct.pack_into("<H", raw, 8, binascii.crc_hqx(raw[16:16 + length], 0))
    raw[4] = (sum(raw[:4]) + sum(raw[5:16])) & 255


def inspect(src, image_size: int, iso_files: list[dict]) -> dict | None:
    """Return bridge metadata, or None for a plain ISO with no UDF VRS."""
    def read(offset, size=SECTOR):
        if offset < 0 or size < 0 or offset + size > image_size:
            raise ValueError("UDF metadata outside the image")
        src.seek(offset)
        raw = src.read(size)
        if len(raw) != size:
            raise ValueError("truncated UDF metadata")
        return raw

    vrs = [read(sector * SECTOR)[1:6] for sector in range(18, min(32, image_size // SECTOR))]
    if not any(name in (b"BEA01", b"NSR02", b"NSR03", b"TEA01") for name in vrs):
        return None
    if vrs[:3] != [b"BEA01", b"NSR02", b"TEA01"]:
        raise ValueError("unsupported UDF volume-recognition sequence")
    metadata = [dict(offset=18 * SECTOR, size=3 * SECTOR, kind="volume recognition")]
    for number in (18, 19, 20):
        raw = read(number * SECTOR)
        if raw[0] != 0 or raw[6] != 1:
            raise ValueError("invalid UDF volume-recognition descriptor")
    last = image_size // SECTOR - 1
    anchors = []
    for sector in (256, last):
        raw = read(sector * SECTOR)
        _tag(raw, 2, sector)
        sequences = [dict(offset=_u32(raw, at + 4) * SECTOR, size=_u32(raw, at)) for at in (16, 24)]
        if anchors and sequences != anchors[0]["sequences"]:
            raise ValueError("UDF anchors disagree on descriptor sequences")
        anchors.append(dict(offset=sector * SECTOR, sequences=sequences))
        metadata.append(dict(offset=sector * SECTOR, size=SECTOR, kind="anchor"))
    # An N-256 backup is a different layout than this two-anchor disc.
    if last - 256 != 256:
        candidate = read((last - 256) * SECTOR)
        try:
            _tag(candidate, 2, last - 256)
        except ValueError:
            pass
        else:
            raise ValueError("three-anchor UDF layout is unsupported")
    partitions, logicals = [], []
    for sequence in anchors[0]["sequences"]:
        if sequence["size"] % SECTOR:
            raise ValueError("unaligned UDF descriptor sequence")
        metadata.append(dict(**sequence, kind="descriptor sequence"))
        found = set()
        for offset in range(sequence["offset"], sequence["offset"] + sequence["size"], SECTOR):
            raw = read(offset)
            kind = _tag(raw, location=offset // SECTOR)
            if kind in found:
                raise ValueError("duplicate UDF volume descriptor")
            found.add(kind)
            if kind == 5:
                if (_u16(raw, 22) != 0 or _u32(raw, 184) != 1 or any(raw[56:184])):
                    raise ValueError("UDF partition has unsupported allocation/free-space structures")
                partitions.append(dict(offset=offset, start=_u32(raw, 188), size=_u32(raw, 192)))
            elif kind == 6:
                if (_u32(raw, 212) != SECTOR or _u32(raw, 264) != 6 or _u32(raw, 268) != 1
                        or raw[440:446] != struct.pack("<BBHH", 1, 6, 1, 0)):
                    raise ValueError("unsupported UDF logical volume partition map")
                if _u16(raw, 256) != 0:
                    raise ValueError("UDF file set references another partition")
                logicals.append(dict(offset=offset, revision=_u16(raw, 240),
                                     file_set_size=_u32(raw, 248), file_set_block=_u32(raw, 252),
                                     integrity_size=_u32(raw, 432), integrity_offset=_u32(raw, 436) * SECTOR))
            elif kind == 7:
                if _u32(raw, 20):
                    raise ValueError("UDF unallocated-space extent tables are unsupported")
            elif kind == 8:
                break
            elif kind not in (1, 4):
                raise ValueError("unsupported UDF descriptor sequence member")
        else:
            raise ValueError("unterminated UDF descriptor sequence")
        if found != {1, 4, 5, 6, 7, 8}:
            raise ValueError("incomplete UDF descriptor sequence")
    if len(partitions) != 2 or len(logicals) != 2:
        raise ValueError("UDF requires main and reserve physical partition descriptors")
    if ({key: val for key, val in partitions[0].items() if key != "offset"}
            != {key: val for key, val in partitions[1].items() if key != "offset"}
            or {key: val for key, val in logicals[0].items() if key != "offset"}
            != {key: val for key, val in logicals[1].items() if key != "offset"}):
        raise ValueError("UDF main and reserve descriptor copies disagree")
    start, length = partitions[0]["start"], partitions[0]["size"]
    if start + length != last:
        raise ValueError("UDF partition must end immediately before the final anchor")
    logical = logicals[0]
    lvid = read(logical["integrity_offset"])
    _tag(lvid, 9, logical["integrity_offset"] // SECTOR)
    if (_u32(lvid, 28) != 1 or any(lvid[32:40]) or _u32(lvid, 72) != 1
            or _u32(lvid, 80) != 0 or _u32(lvid, 84) != length):
        raise ValueError("unsupported UDF integrity/free-space tables")
    metadata.append(dict(offset=logical["integrity_offset"], size=logical["integrity_size"], kind="integrity sequence"))
    fsd_block = logical["file_set_block"]
    fsd_offset = (start + fsd_block) * SECTOR
    fsd = read(fsd_offset)
    _tag(fsd, 256, fsd_block)
    if _u16(fsd, 408) or any(fsd[448:480]):
        raise ValueError("chained or cross-partition UDF file sets unsupported")
    metadata.append(dict(offset=fsd_offset, size=logical["file_set_size"], kind="file set"))
    iso_by_name = {entry["path"].casefold(): entry for entry in iso_files}
    if len(iso_by_name) != len(iso_files):
        raise ValueError("ISO paths collide under UDF name matching")
    files, seen, names = [], set(), set()

    def walk(block, prefix):
        if block in seen or block >= length:
            raise ValueError("aliased or out-of-bounds UDF file entry")
        seen.add(block)
        fe_offset = (start + block) * SECTOR
        raw = read(fe_offset)
        _tag(raw, 261, block)
        kind, flags = raw[27], _u16(raw, 34)
        if kind not in (4, 5) or flags & 7 or _u32(raw, 172) != 8:
            raise ValueError("UDF requires one recorded short allocation descriptor per file")
        ad = 176 + _u32(raw, 168)
        if ad + 8 > SECTOR or any(raw[112:128]):
            raise ValueError("unsupported UDF extended-attribute allocation")
        extent_length, extent_block = _u32(raw, ad), _u32(raw, ad + 4)
        size = _u64(raw, 56)
        # The original dummy file encodes its 1 GiB length without masking the
        # short-AD type bits. Preserve this existing quirk; it is never resized.
        original_dummy = kind == 5 and prefix.casefold() == "exter.bin" and size == extent_length == 1 << 30
        if ((extent_length >> 30 and not original_dummy) or extent_length != size or extent_block * SECTOR + size > length * SECTOR
                or _u64(raw, 64) != (size + SECTOR - 1) // SECTOR):
            raise ValueError("UDF allocation descriptor disagrees with file length")
        metadata.append(dict(offset=fe_offset, size=SECTOR, kind="file entry"))
        offset = (start + extent_block) * SECTOR
        if kind == 5:
            folded = prefix.casefold()
            original = iso_by_name.get(folded)
            if folded in names or original is None or original["offset"] != offset or original["size"] != size:
                raise ValueError("UDF and ISO9660 file trees disagree")
            if original_dummy and original.get("classification") != "opaque":
                raise ValueError("original 1 GiB allocation quirk is supported only for opaque EXTER.BIN")
            names.add(folded)
            files.append(dict(path=original["path"], offset=offset, size=size,
                              entry_offset=fe_offset, allocation_offset=ad))
            return
        metadata.append(dict(offset=offset, size=size, kind="directory data"))
        directory = read(offset, size)
        cursor = 0
        while cursor < size:
            if cursor + 38 > size:
                raise ValueError("truncated UDF file identifier")
            remaining = directory[cursor:]
            fi_size = (38 + _u16(remaining, 36) + remaining[19] + 3) & ~3
            if cursor + fi_size > size:
                raise ValueError("UDF file identifier exceeds directory length")
            record = remaining[:fi_size]
            _tag(record, 257, extent_block + cursor // SECTOR)
            if _u16(record, 16) != 1 or _u16(record, 28) or record[18] & (4 | 16):
                raise ValueError("unsupported UDF file identifier")
            if not record[18] & 8:
                name_at = 38 + _u16(record, 36)
                encoded = record[name_at:name_at + record[19]]
                if not encoded or encoded[0] not in (8, 16):
                    raise ValueError("unsupported UDF filename encoding")
                name = encoded[1:].decode("latin1" if encoded[0] == 8 else "utf-16-be")
                if not name or name in (".", "..") or "/" in name or "\\" in name:
                    raise ValueError("unsafe UDF filename")
                walk(_u32(record, 24), f"{prefix}/{name}" if prefix else name)
            cursor += fi_size

    walk(_u32(fsd, 404), "")
    if names != set(iso_by_name):
        raise ValueError("UDF does not contain every ISO9660 file")
    for span in metadata:
        if span["offset"] + span["size"] > image_size:
            raise ValueError("UDF metadata range exceeds the image")
        for entry in iso_files:
            if max(span["offset"], entry["offset"]) < min(span["offset"] + span["size"], entry["offset"] + entry["size"]):
                raise ValueError("UDF metadata overlaps editable ISO file data")
    revision = logical["revision"]
    return dict(version=f"{revision >> 8:x}.{revision & 255:02x}", partition_start=start, partition_size=length,
                partition_descriptors=[p["offset"] for p in partitions],
                integrity_offset=logical["integrity_offset"],
                anchors=[a["offset"] for a in anchors], files=files,
                metadata_ranges=metadata, growth_extra_sectors=1)


def patch(dst, manifest: dict | None, changes: list[dict], original_size: int, final_size: int) -> int:
    """Patch changed archive FEs and, on growth, partition size and end anchor.

    The caller updates ISO9660 volume length to the returned image size. A grown
    image reserves one new final sector for the relocated UDF end anchor.
    """
    if manifest is None or not changes:
        return final_size

    def read(offset):
        dst.seek(offset)
        raw = bytearray(dst.read(SECTOR))
        if len(raw) != SECTOR:
            raise ValueError("missing preserved UDF metadata while packing")
        _tag(raw)
        return raw

    def write(offset, raw):
        _retag(raw)
        dst.seek(offset)
        dst.write(raw)

    by_name = {entry["path"]: entry for entry in manifest["files"]}
    start = manifest["partition_start"]
    for change in changes:
        entry = by_name[change["path"]]
        size, offset = change["new_size"], change["new_offset"]
        if size >= 1 << 30 or offset % SECTOR or offset // SECTOR < start:
            raise ValueError("resized file exceeds UDF recorded short-allocation limits")
        raw = read(entry["entry_offset"])
        struct.pack_into("<QQ", raw, 56, size, (size + SECTOR - 1) // SECTOR)
        struct.pack_into("<II", raw, entry["allocation_offset"], size, offset // SECTOR - start)
        write(entry["entry_offset"], raw)
    if final_size > original_size:
        if final_size % SECTOR:
            raise ValueError("grown UDF image must be sector aligned")
        anchor_offset = final_size
        final_size += SECTOR
        length = anchor_offset // SECTOR - start
        if not 0 <= length <= 0xFFFFFFFF:
            raise ValueError("UDF physical partition length overflow")
        for offset in manifest["partition_descriptors"]:
            raw = read(offset)
            struct.pack_into("<I", raw, 192, length)
            write(offset, raw)
        raw = read(manifest["integrity_offset"])
        struct.pack_into("<I", raw, 84, length)
        write(manifest["integrity_offset"], raw)
        anchor = read(manifest["anchors"][-1])
        struct.pack_into("<I", anchor, 12, anchor_offset // SECTOR)
        write(anchor_offset, anchor)
        dst.truncate(final_size)
    return final_size
