#!/usr/bin/env python3
"""Scan OVERLAY/AREA*.BIN data sections for embedded asset signatures.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

Purpose: per-area overlays are MWo3-format code modules. Their .text sections
contain code; their .data sections normally hold per-area constants. This tool
checks whether any embedded assets (textures, CLUTs, VAGs, sound banks,
nested MWo3 blobs, geometry markers, ASCII strings, etc.) live in either
section.

Findings are reported as counts only -- no raw bytes printed.
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

# Make sibling tooling importable.
sys.path.insert(0, str(Path(__file__).resolve().parent))

from clut import find_clut_candidates  # type: ignore
from extract_textures import find_packet_starts, first_trxreg, find_image_block  # type: ignore


MWO3_HEADER_SIZE = 0x40


def parse_mwo3(data: bytes) -> dict:
    if len(data) < MWO3_HEADER_SIZE or data[:3] != b"MWo":
        raise ValueError("not an MWo3 file")
    text_size = struct.unpack_from("<I", data, 0x0C)[0]
    data_size = struct.unpack_from("<I", data, 0x10)[0]
    bss_size = struct.unpack_from("<I", data, 0x14)[0]
    load_addr = struct.unpack_from("<I", data, 0x08)[0]
    name = data[0x20:0x40].split(b"\x00", 1)[0].decode("latin1", errors="replace")
    return {
        "load_addr": load_addr,
        "text_size": text_size,
        "data_size": data_size,
        "bss_size": bss_size,
        "name": name,
        "text": data[MWO3_HEADER_SIZE:MWO3_HEADER_SIZE + text_size],
        "data": data[MWO3_HEADER_SIZE + text_size:MWO3_HEADER_SIZE + text_size + data_size],
    }


def scan_vag_headers(buf: bytes) -> list[int]:
    """Locate VAGp magic at any offset."""
    out = []
    pos = 0
    while True:
        i = buf.find(b"VAGp", pos)
        if i < 0:
            break
        out.append(i)
        pos = i + 4
    return out


def scan_sshd_headers(buf: bytes) -> list[int]:
    """SShd magic at any offset."""
    out = []
    pos = 0
    while True:
        i = buf.find(b"SShd", pos)
        if i < 0:
            break
        out.append(i)
        pos = i + 4
    return out


def scan_nested_mwo3(buf: bytes, skip_first: bool = True) -> list[int]:
    """MWo3 magic inside the buffer (other than offset 0 if skip_first)."""
    out = []
    pos = 1 if skip_first else 0
    while True:
        i = buf.find(b"MWo", pos)
        if i < 0:
            break
        # require version byte == '3' (0x33) like the outer header
        if i + 3 < len(buf) and buf[i + 3] == 0x33:
            out.append(i)
        pos = i + 3
    return out


def scan_dma_tags(buf: bytes) -> int:
    """Approximate count of GS texture-upload DMA packets."""
    return len(find_packet_starts(buf))


def scan_full_gs_texture_packets(buf: bytes) -> list[tuple[int, int, int]]:
    """Return list of (offset, trx_w, trx_h) for fully-formed GS texture packets."""
    out = []
    for start in find_packet_starts(buf):
        trx = first_trxreg(buf, start)
        if trx is None:
            continue
        tw, th = trx
        if tw == 0 or th == 0 or tw % 64 or th % 32:
            continue
        if find_image_block(buf, tw * th * 4, start) is None:
            continue
        out.append((start, tw, th))
    return out


def scan_clut_candidates(buf: bytes) -> list[int]:
    return find_clut_candidates(buf)


def scan_ascii_runs(buf: bytes, min_len: int = 6) -> list[tuple[int, int]]:
    """Return (offset, length) of printable ASCII runs of >= min_len bytes."""
    out = []
    i = 0
    n = len(buf)
    while i < n:
        if 0x20 <= buf[i] < 0x7F:
            j = i
            while j < n and 0x20 <= buf[j] < 0x7F:
                j += 1
            if j - i >= min_len:
                out.append((i, j - i))
            i = j + 1
        else:
            i += 1
    return out


def categorize_strings(strings: list[bytes]) -> dict:
    """Bucketize ascii runs into rough categories."""
    cats = {
        "path_like": 0,    # contains '/' or '.'
        "extension": 0,    # ends with common ext
        "shouty": 0,       # ALL CAPS WITH SPACES (UI labels?)
        "format_specifier": 0,  # contains %d, %s etc.
        "other": 0,
    }
    EXTS = (b".BIN", b".bin", b".PSS", b".pss", b".VAG", b".vag", b".tim",
            b".TIM", b".TXT", b".txt", b".dat", b".DAT")
    for s in strings:
        if b"%" in s and any(c in s for c in b"dsxfio"):
            cats["format_specifier"] += 1
            continue
        if any(s.endswith(e) for e in EXTS):
            cats["extension"] += 1
            continue
        if b"/" in s or b"\\" in s or s.count(b".") >= 1 and b" " not in s:
            cats["path_like"] += 1
            continue
        if all(c == 0x20 or 0x41 <= c <= 0x5A or c == ord("_") or 0x30 <= c <= 0x39 for c in s):
            cats["shouty"] += 1
            continue
        cats["other"] += 1
    return cats


def scan_one(path: Path) -> dict:
    data = path.read_bytes()
    parsed = parse_mwo3(data)
    text = parsed["text"]
    dseg = parsed["data"]

    result = {
        "file": path.name,
        "name": parsed["name"],
        "text_size": len(text),
        "data_size": len(dseg),
    }

    for section_name, buf in (("text", text), ("data", dseg)):
        sec = {
            "vag_headers": len(scan_vag_headers(buf)),
            "sshd_headers": len(scan_sshd_headers(buf)),
            "nested_mwo3": len(scan_nested_mwo3(buf, skip_first=False)),
            "dma_packet_candidates": scan_dma_tags(buf),
            "gs_texture_packets": len(scan_full_gs_texture_packets(buf)),
            "clut_candidates": len(scan_clut_candidates(buf)),
        }
        # ASCII strings: only check data section in detail.
        if section_name == "data":
            runs = scan_ascii_runs(buf, min_len=6)
            sec["ascii_runs"] = len(runs)
            strings = [buf[o:o + l] for o, l in runs]
            sec["ascii_categories"] = categorize_strings(strings)
            sec["ascii_examples_sanitized"] = [
                # Length-only descriptor; never pasted to report.
                len(s) for s in strings[:5]
            ]
        result[section_name] = sec
    return result


def main():
    p = argparse.ArgumentParser(description="Scan overlay BINs for embedded assets")
    p.add_argument("--in", dest="indir", default="extract/OVERLAY",
                   help="Directory of AREA*.BIN files (disc-derived)")
    p.add_argument("--file", action="append", default=None,
                   help="Scan a specific BIN file (overrides --in walk)")
    args = p.parse_args()

    if args.file:
        files = [Path(f) for f in args.file]
    else:
        d = Path(args.indir)
        files = sorted(d.glob("AREA*.BIN"))

    if not files:
        print("no AREA*.BIN files found", file=sys.stderr)
        sys.exit(1)

    totals = {
        "vag": 0, "sshd": 0, "nested_mwo3": 0,
        "dma_packet_cands": 0, "gs_texture_packets": 0,
        "clut_candidates": 0, "ascii_runs": 0,
    }

    print(f"{'file':<13} {'text':>6} {'data':>6} | "
          f"{'VAG':>4} {'SShd':>4} {'MWo3':>4} {'DMA?':>5} {'GSTx':>4} {'CLUT':>4} | "
          f"{'strs':>4} {'paths':>5} {'fmts':>4} {'shouty':>6} {'ext':>3} {'oth':>4}")
    print("-" * 110)
    for f in files:
        try:
            r = scan_one(f)
        except Exception as e:
            print(f"{f.name}: ERROR {e}", file=sys.stderr)
            continue
        d_sec = r["data"]
        t_sec = r["text"]
        cats = d_sec["ascii_categories"]
        # Totals — sum across both sections.
        totals["vag"] += t_sec["vag_headers"] + d_sec["vag_headers"]
        totals["sshd"] += t_sec["sshd_headers"] + d_sec["sshd_headers"]
        totals["nested_mwo3"] += d_sec["nested_mwo3"]
        totals["dma_packet_cands"] += d_sec["dma_packet_candidates"]
        totals["gs_texture_packets"] += d_sec["gs_texture_packets"]
        totals["clut_candidates"] += d_sec["clut_candidates"]
        totals["ascii_runs"] += d_sec["ascii_runs"]
        print(f"{r['file']:<13} {r['text_size']:>6} {r['data_size']:>6} | "
              f"{d_sec['vag_headers']:>4} {d_sec['sshd_headers']:>4} "
              f"{d_sec['nested_mwo3']:>4} {d_sec['dma_packet_candidates']:>5} "
              f"{d_sec['gs_texture_packets']:>4} {d_sec['clut_candidates']:>4} | "
              f"{d_sec['ascii_runs']:>4} {cats['path_like']:>5} "
              f"{cats['format_specifier']:>4} {cats['shouty']:>6} "
              f"{cats['extension']:>3} {cats['other']:>4}")

    print("-" * 110)
    print("TOTALS:")
    for k, v in totals.items():
        print(f"  {k:<22} {v}")


if __name__ == "__main__":
    main()
