#!/usr/bin/env python3
"""rig_probe.py -- rig/skeleton analysis for Extermination character meshes.

Investigates how a mesh file's id-0x74 animation prefix (node parent table)
relates to the node slots its stage-2 MESH blocks actually reference via
the position-W bits (node = (W_bits & 0x3FF) >> 3; see FINDINGS.md
"Skinned-character pipeline FULLY DECODED").

Subcommands
-----------
  probe  FILE [--segment N]
      Full single-file report: every keyed-animation container header in
      the file (offset, blob id, node count, clip length, parent table +
      tree analysis), every mesh segment (block count, slot-usage
      histogram), and per-slot geometry stats (vertex count, bbox size,
      centroid) for the chosen segment.

  survey ROOT
      Walk ROOT (e.g. extract/) for *_id*.bin files, parse each file's
      animation containers, and tabulate node counts / clip lengths /
      parent tables.  Groups identical parent tables across files and
      reports per-file mesh-segment slot ranges, flagging multi-rig
      candidates (files whose max referenced slot exceeds the prefix
      node count).

  compare FILE_A FILE_B
      Side-by-side parent-table comparison of the first container of two
      files, including "is table A a prefix/suffix/subtree of table B"
      checks and half-table splits.

All offsets are file-relative.  Read-only: never writes anything.

Runs natively on arm64 macOS (pure Python 3.10+).
"""
from __future__ import annotations

import argparse
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

# Make tools/ siblings importable regardless of cwd.
sys.path.insert(0, str(Path(__file__).resolve().parent))
import extract_models as em  # noqa: E402

VERT_QW = 0x40          # one vertex record = 4 qwords (tex/ST/normal/pos+w)


# --------------------------------------------------------------------------
# Animation-container scanning (superset of extract_models.parse_id74_prefix:
# finds EVERY container header in the file, any blob id).
# --------------------------------------------------------------------------

def scan_anim_headers(d: bytes) -> list[dict]:
    """Find every keyed-animation container header in `d`.

    Header layout (FINDINGS.md "id 0x74 prefix is ANIMATION"):
        +0x00 u16 n_sections (node count), +0x02 u16 clip_len,
        +0x04 u16 0xfffe/0xffff, +0x08 u32 blob id,
        +0x20 i32 parents[n] (strict tree: -1 <= parent < index).
    """
    out = []
    # Fast anchor: parents[0] == -1 -> 4 x 0xff at hdr+0x20 (4-aligned).
    pos = -1
    while True:
        pos = d.find(b"\xff\xff\xff\xff", pos + 1)
        if pos < 0:
            break
        h = (pos & ~3) - 0x20
        if h < 0 or h + 0x20 > len(d):
            continue
        n, clip = struct.unpack_from("<2H", d, h)
        if not (1 <= n <= 64 and clip > 0):
            continue
        sentinel = struct.unpack_from("<H", d, h + 4)[0]
        if sentinel not in (0xFFFE, 0xFFFF):
            continue
        if h + 0x20 + n * 4 > len(d):
            continue
        parents = struct.unpack_from(f"<{n}i", d, h + 0x20)
        if not all(-1 <= p < i for i, p in enumerate(parents)):
            continue
        if parents[0] != -1:          # first node must be a root
            continue
        blob_id = struct.unpack_from("<I", d, h + 8)[0]
        if blob_id == 0 or blob_id > 0x10000:
            continue
        if out and out[-1]["hdr"] == h:        # dedupe overlapping anchors
            continue
        out.append({"hdr": h, "id": blob_id, "n": n, "clip": clip,
                    "parents": list(parents)})
    return out


# --------------------------------------------------------------------------
# Parent-table / tree analysis
# --------------------------------------------------------------------------

def tree_stats(parents: list[int]) -> dict:
    """Roots, per-node depth, children lists, leaves, canonical shape."""
    n = len(parents)
    children = defaultdict(list)
    roots = []
    for i, p in enumerate(parents):
        if p == -1:
            roots.append(i)
        else:
            children[p].append(i)
    depth = [0] * n
    for i, p in enumerate(parents):
        depth[i] = 0 if p == -1 else depth[p] + 1
    leaves = [i for i in range(n) if not children[i]]

    def canon(i: int) -> str:
        """Canonical (sorted) subtree-shape string -- structural identity."""
        return "(" + "".join(sorted(canon(c) for c in children[i])) + ")"

    return {"roots": roots, "children": children, "depth": depth,
            "leaves": leaves, "canon": canon}


def mirror_pairs(parents: list[int]) -> list[tuple[int, int]]:
    """Pairs of distinct sibling subtrees with identical shape (L/R limbs)."""
    st = tree_stats(parents)
    pairs = []
    for p, kids in st["children"].items():
        seen = {}
        for k in kids:
            shape = st["canon"](k)
            if shape in seen and shape != "()":
                pairs.append((seen[shape], k))
            else:
                seen[shape] = k
    return pairs


def describe_tree(parents: list[int], indent: str = "  ") -> list[str]:
    st = tree_stats(parents)
    lines = []
    lines.append(f"{indent}n={len(parents)} roots={st['roots']} "
                 f"max_depth={max(st['depth'])} leaves={len(st['leaves'])}")
    lines.append(f"{indent}parents: {parents}")
    lines.append(f"{indent}depths : {st['depth']}")
    # chains from each leaf back to root (compact skeleton picture)
    for r in st["roots"]:
        lines.extend(_tree_ascii(r, st["children"], indent + "  "))
    mp = mirror_pairs(parents)
    if mp:
        lines.append(f"{indent}mirror-symmetric sibling subtrees: {mp}")
    return lines


def _tree_ascii(node: int, children: dict, indent: str) -> list[str]:
    out = [f"{indent}{node}"]
    for c in children[node]:
        out.extend(_tree_ascii(c, children, indent + "| "))
    return out


# --------------------------------------------------------------------------
# Mesh-segment walking + slot usage
# --------------------------------------------------------------------------

def walk_segments(d: bytes) -> list[list[bytes]]:
    """Split a packed model file into mesh segments.

    Segment = run of MESH_SIG 0x820 blocks; MATRIX-descriptor blocks are
    the separators (cf. export_native._walk_meshsig_blocks, which returns
    only one segment).  Returns [segment][payload bytes (0x800)].
    """
    bounds = em.block_bounds(d)
    segs: list[list[bytes]] = [[]]
    for i, (s, e) in enumerate(bounds):
        if not i:
            continue
        desc = d[s + 0x10:s + 0x20]
        if len(desc) < 16:
            continue
        if em.is_matrix_descriptor(desc):
            segs.append([])
            continue
        if desc[8:16] == em.MESH_SIG:
            segs[-1].append(d[s + 0x20:s + 0x20 + 0x800])
    return [s for s in segs if s] if any(segs) else []


def walk_blob_blocks(d: bytes) -> tuple[list[bytes], int]:
    """Raw mesh-blob style (chunk28/f00_id3b): header {n_blocks, qwc,
    n_nodes, size}, VIF stream from +0x48.  Returns (payloads, n_nodes)."""
    n_blocks, _qwc, n_nodes, size = struct.unpack_from("<4I", d, 0)
    payloads = []
    o = 0x48
    while o + 8 <= min(size, len(d)) and len(payloads) < n_blocks:
        w0, w1 = struct.unpack_from("<2I", d, o)
        if w0 == 0x01000404 and (w1 >> 24) & 0xFF == 0x6C:
            num = (w1 >> 16) & 0xFF
            qw = 256 if num == 0 else num
            payloads.append(d[o + 8:o + 8 + qw * 16])
            o += 8 + qw * 16
        else:
            o += 4
    return payloads, n_nodes


def is_raw_blob(d: bytes) -> bool:
    return (len(d) > 0x50
            and struct.unpack_from("<I", d, 0)[0] < 0x1000
            and d[0x48:0x50] == em.MESH_SIG)


def segment_slot_stats(payloads: list[bytes]) -> dict:
    """Per-slot vertex stats for one mesh segment.

    Returns {"hist": Counter(slot -> nverts), "bbox": {slot: (min,max)},
    "nverts": total} using the verified W-bit binding."""
    hist: Counter = Counter()
    bbox: dict[int, list] = {}
    total = 0
    for payload in payloads:
        for r in range(0, len(payload) - VERT_QW + 1, VERT_QW):
            w = struct.unpack_from("<f", payload, r + 0x3C)[0]
            if abs(abs(w) - 1.0) > 0.25:
                break
            wbits = struct.unpack_from("<I", payload, r + 0x3C)[0]
            slot = (wbits & 0x3FF) >> 3
            pos = struct.unpack_from("<3f", payload, r + 0x30)
            total += 1
            hist[slot] += 1
            bb = bbox.setdefault(slot, [list(pos), list(pos)])
            for a in range(3):
                bb[0][a] = min(bb[0][a], pos[a])
                bb[1][a] = max(bb[1][a], pos[a])
    return {"hist": hist, "bbox": bbox, "nverts": total}


def fmt_hist(hist: Counter) -> list[str]:
    if not hist:
        return ["  (no vertices)"]
    lines = []
    peak = max(hist.values())
    for slot in sorted(hist):
        bar = "#" * max(1, round(40 * hist[slot] / peak))
        lines.append(f"  slot {slot:3d}: {hist[slot]:5d} {bar}")
    return lines


def slot_ranges(slots: list[int]) -> list[tuple[int, int]]:
    """Collapse a sorted slot list into contiguous (lo, hi) ranges."""
    rng = []
    for s in sorted(slots):
        if rng and s == rng[-1][1] + 1:
            rng[-1] = (rng[-1][0], s)
        else:
            rng.append((s, s))
    return rng


# --------------------------------------------------------------------------
# Subcommands
# --------------------------------------------------------------------------

def cmd_probe(args) -> int:
    d = Path(args.file).read_bytes()
    print(f"== {args.file}  ({len(d)} bytes) ==\n")

    print("-- animation containers --")
    headers = scan_anim_headers(d)
    if not headers:
        print("  none found")
    for h in headers:
        print(f"  hdr @ {h['hdr']:#08x}  id={h['id']:#04x}  "
              f"nodes={h['n']}  clip={h['clip']} frames")
        if args.trees:
            print("\n".join(describe_tree(h["parents"], "    ")))
    print()

    if is_raw_blob(d):
        payloads, n_nodes = walk_blob_blocks(d)
        segs = [payloads]
        print(f"-- raw blob: {len(payloads)} blocks, "
              f"header n_nodes={n_nodes} --")
    else:
        segs = walk_segments(d)
        print(f"-- packed model: {len(segs)} mesh segment(s) --")

    for si, payloads in enumerate(segs):
        st = segment_slot_stats(payloads)
        used = sorted(st["hist"])
        print(f"\nsegment {si}: {len(payloads)} blocks, "
              f"{st['nverts']} vertices")
        print(f"  slots used: {len(used)}  "
              f"min={used[0] if used else '-'} "
              f"max={used[-1] if used else '-'}  "
              f"ranges={slot_ranges(used)}")
        if args.hist or si == args.segment:
            print("\n".join(fmt_hist(st["hist"])))
        if si == args.segment:
            print(f"\n  per-slot bone-local geometry (segment {si}):")
            print("   slot  nvert  bbox dx    dy    dz    diag   centroid")
            for slot in used:
                mn, mx = st["bbox"][slot]
                dx, dy, dz = (mx[a] - mn[a] for a in range(3))
                diag = (dx * dx + dy * dy + dz * dz) ** 0.5
                cx, cy, cz = ((mx[a] + mn[a]) / 2 for a in range(3))
                print(f"   {slot:4d} {st['hist'][slot]:6d}  "
                      f"{dx:5.2f} {dy:5.2f} {dz:5.2f}  {diag:5.2f}  "
                      f"({cx:6.2f},{cy:6.2f},{cz:6.2f})")
    return 0


def cmd_survey(args) -> int:
    root = Path(args.root)
    table_groups: dict[tuple, list[str]] = defaultdict(list)
    rows = []
    for path in sorted(root.rglob("*_id*.bin")):
        if not path.is_file():
            continue
        try:
            d = path.read_bytes()
        except OSError:
            continue
        if em.is_level_file(path):
            continue
        headers = scan_anim_headers(d)
        if not headers and not is_raw_blob(d):
            continue
        # mesh slot usage
        if is_raw_blob(d):
            segs = [walk_blob_blocks(d)[0]]
        else:
            segs = walk_segments(d)
        seg_desc = []
        for si, payloads in enumerate(segs):
            st = segment_slot_stats(payloads)
            used = sorted(st["hist"])
            if used:
                seg_desc.append(
                    f"s{si}:{len(payloads)}blk slots {used[0]}..{used[-1]}"
                    f" ({len(used)} used)")
        rel = str(path.relative_to(root))
        for h in headers:
            key = tuple(h["parents"])
            table_groups[key].append(f"{rel}@{h['hdr']:#x}")
        prim = headers[0] if headers else None
        rows.append((rel,
                     f"{prim['n']}n/{prim['clip']}f" if prim else "-",
                     len(headers), "; ".join(seg_desc) or "-"))

    print(f"== survey of {root}: {len(rows)} files with anim prefix "
          f"or blob mesh ==\n")
    w = max((len(r[0]) for r in rows), default=10)
    print(f"{'file':<{w}}  prefix    #hdr  mesh segments (slot usage)")
    for r in rows:
        print(f"{r[0]:<{w}}  {r[1]:<9} {r[2]:>4}  {r[3]}")

    print("\n== distinct parent tables ==")
    for key, members in sorted(table_groups.items(),
                               key=lambda kv: (-len(kv[1]), len(kv[0]))):
        print(f"\n* n={len(key)} parents={list(key)}")
        print(f"  used by {len(members)}: "
              + ", ".join(members[:8])
              + (" ..." if len(members) > 8 else ""))
    return 0


def cmd_compare(args) -> int:
    da = Path(args.file_a).read_bytes()
    db = Path(args.file_b).read_bytes()
    ha = scan_anim_headers(da)
    hb = scan_anim_headers(db)
    if not ha or not hb:
        print("missing anim container in one of the files"); return 1
    pa, pb = ha[0]["parents"], hb[0]["parents"]
    print(f"A: {args.file_a} n={len(pa)}")
    print("\n".join(describe_tree(pa)))
    print(f"\nB: {args.file_b} n={len(pb)}")
    print("\n".join(describe_tree(pb)))

    small, big = (pa, pb) if len(pa) <= len(pb) else (pb, pa)
    print(f"\nprefix match (first {len(small)}): "
          f"{big[:len(small)] == small}")
    # is `small` embedded at any offset with re-based parents?
    for base in range(len(big) - len(small) + 1):
        window = big[base:base + len(small)]
        rebased = [p - base if p >= base else -1 for p in window]
        if rebased == small:
            print(f"`small` table embedded at slot offset {base} "
                  f"(parents re-based)")
    # half splits of the bigger table
    nb = len(big)
    for cut in range(1, nb):
        lo, hi = big[:cut], big[cut:]
        if all(p < cut for p in lo) and hi and hi[0] == -1 \
                and all(p == -1 or p >= cut for p in hi):
            print(f"bigger table splits cleanly at {cut}: "
                  f"[0..{cut - 1}] + rooted [{cut}..{nb - 1}]")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("probe", help="full single-file report")
    p.add_argument("file")
    p.add_argument("--segment", type=int, default=0,
                   help="segment for per-slot geometry stats (default 0)")
    p.add_argument("--hist", action="store_true",
                   help="print slot histograms for every segment")
    p.add_argument("--trees", action="store_true", default=True,
                   help="print parent-table tree analysis (default on)")
    p.set_defaults(fn=cmd_probe)

    p = sub.add_parser("survey", help="tabulate prefixes across a tree")
    p.add_argument("root")
    p.set_defaults(fn=cmd_survey)

    p = sub.add_parser("compare", help="compare two files' parent tables")
    p.add_argument("file_a")
    p.add_argument("file_b")
    p.set_defaults(fn=cmd_compare)

    args = ap.parse_args()
    return args.fn(args)


if __name__ == "__main__":
    raise SystemExit(main())
