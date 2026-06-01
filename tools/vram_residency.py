#!/usr/bin/env python3
"""Extermination (SCUS-97112) global GS VRAM residency map.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing. The scan cache it
writes lands under ``scratch/`` (git-ignored — see project ``.gitignore``).

WHY THIS EXISTS
---------------
A GS texture upload names its destination by a VRAM word-address: the
``BITBLTBUF`` register's ``DBP`` (Destination Buffer Pointer). A mesh material
names the texture it samples by the same address, encoded as ``sheet_field =
(m0 >> 15) & 0x3FFF`` and mapped to a DBP via the documented affine relation
(see ``extract_subtextures.py`` / ``docs/FINDINGS.md``).

The per-level / per-character exporters resolve a material's DBP by scanning
*that file's chunk directory* (and, as a fallback, the whole tree ad-hoc).
That misses the common case where the texture for one file's geometry is
uploaded by a DIFFERENT file ("cross-file texture residency"): a shared/UI
sheet, a sibling level's carrier file, a model's texture sibling. The honest
limitation noted in the docs is ~631 materials that reference a texture
uploaded elsewhere, plus per-level "4+ DBP" cases that leave extra sheets gray.

This module builds ONE residency map for the whole disc: every GS texture
upload found anywhere under ``extract/``, indexed by DBP, so any DBP can be
resolved disc-wide regardless of which file references it.

WHAT IT RECORDS
---------------
For every GS upload packet (``BITBLTBUF`` + ``TRXREG`` [+ ``TRXPOS``] +
IMAGE-mode GIF/DMA transfer) it records an ``Upload``:

    dbp        GS destination base pointer (BITBLTBUF bits 32..45)
    dbw        destination buffer width / 64 (BITBLTBUF bits 48..53)
    dpsm       destination pixel storage mode (BITBLTBUF bits 56..61)
    tw, th     TRXREG transfer region, in DPSM texels
    dsax,dsay  TRXPOS destination offset within the buffer (0 for full sheets)
    src_file   source file name
    src_path   absolute source path (relative form stored in the cache)
    offset     file byte offset of the BITBLTBUF row
    trxreg_off file byte offset of the TRXREG row (anchor for decode)
    width/height  decoded PSMT8 texture dims (tw*2 x th*2 for PSMCT32->PSMT8)
    clut       True if this looks like a CLUT upload (small PSMCT32), not a
               texture sheet — recorded and tagged, NOT discarded

THE MAP + LOOKUP
----------------
``ResidencyMap.by_dbp`` is ``DBP -> [Upload, ...]``. The same DBP is reused
across many frames/levels (VRAM is only 4 MB), so each DBP may have several
uploads (double-buffering / reuse). ``resolve_dbp(dbp, ...)`` returns the best
candidate:

  1. exact-DBP candidates, ranked by (PSM match, dims match, same chunk dir,
     larger sheet, lower offset) — the consumer can pass `prefer_dir`,
     `want_psm`, `want_dims` to disambiguate;
  2. if no exact match, the nearest-DBP candidate within `snap_tol` VRAM words
     (the affine ``sheet_field -> DBP`` map is only exact on the universal slot
     trio, so neighbouring addresses are the same physical sheet);
  3. None if nothing resolves — the consumer falls back to a gray placeholder.

``decode(upload)`` reuses ``extract_subtextures.decode_transfer`` to produce
the PSMT8 pixels, so this module owns no duplicate decode logic.

CACHE
-----
Walking every file each run is expensive. The scan is cached to
``scratch/vram_residency.json`` (git-ignored). The cache is keyed by the set
of input files + their mtimes/sizes; a changed corpus invalidates it. Use
``--rebuild`` to force a fresh scan.

USAGE
    python3 tools/vram_residency.py --scan            # build/refresh the map
    python3 tools/vram_residency.py --scan --rebuild  # force a fresh scan
    python3 tools/vram_residency.py --resolve 14592   # look up one DBP
    python3 tools/vram_residency.py --report          # summary stats

As a library:
    from vram_residency import ResidencyMap
    m = ResidencyMap.load_or_scan(Path("extract"))
    up = m.resolve_dbp(14592, prefer_dir=Path("extract/chunk21"))
    if up: w, h, pixels = m.decode(up)
"""
from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass, asdict
from pathlib import Path

# Sibling-module import that works regardless of how this script is launched.
_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

import extract_subtextures as est  # noqa: E402  (scan_transfers / decode_transfer)


# Default scratch cache location. scratch/ is git-ignored (see .gitignore).
DEFAULT_CACHE = _HERE.parent / "scratch" / "vram_residency.json"
CACHE_VERSION = 2

# A CLUT upload is a small PSMCT32 region (a 256-entry palette is 16x16 in
# PSMCT32, or a 256x1 / 1x256 strip). Texture SHEETS are always 256-wide in
# PSMCT32 (DBW=4) and tall. Tag anything that is palette-shaped as a CLUT.
CLUT_MAX_TEXELS = 256          # a 256-entry CLUT
CLUT_MAX_DIM = 64             # neither axis exceeds this for a CLUT

# When no exact-DBP upload exists, snap to the nearest upload within this many
# VRAM words. The universal trio is exact; neighbouring slots (e.g. material
# DBP 14562 vs upload 14592) are the same physical sheet a few words apart.
DEFAULT_SNAP_TOL = 64


# ---------------------------------------------------------------------------
# BITBLTBUF / TRXPOS field decode
#
# The 16-byte GIF A+D row is [8 bytes DATA][8 bytes ADDR]. A GS register write
# has ADDR>>8 == 0 and ADDR&0xFF == register id. BITBLTBUF (0x50) packs:
#   SBP  [0:13]  SBW [16:21]  SPSM [24:29]
#   DBP  [32:45] DBW [48:53]  DPSM [56:61]
# TRXPOS (0x51) packs DSAX [32:42], DSAY [48:58] (dest offset) among others.
# ---------------------------------------------------------------------------
def _bitbltbuf_fields(data_qw: int) -> tuple[int, int, int]:
    """Return (DBP, DBW, DPSM) from a BITBLTBUF data qword."""
    dbp = (data_qw >> 32) & 0x3FFF
    dbw = (data_qw >> 48) & 0x3F
    dpsm = (data_qw >> 56) & 0x3F
    return dbp, dbw, dpsm


def _trxpos_dest(data_qw: int) -> tuple[int, int]:
    """Return (DSAX, DSAY) destination offset from a TRXPOS data qword."""
    dsax = (data_qw >> 32) & 0x7FF
    dsay = (data_qw >> 48) & 0x7FF
    return dsax, dsay


@dataclass
class Upload:
    """One GS texture/CLUT upload found in a file."""

    dbp: int
    dbw: int
    dpsm: int
    tw: int                 # TRXREG transfer width (DPSM texels)
    th: int                 # TRXREG transfer height
    dsax: int               # TRXPOS dest X offset within the buffer
    dsay: int               # TRXPOS dest Y offset
    src_rel: str            # path relative to the scan root (cache-stable)
    offset: int             # file offset of the BITBLTBUF row
    trxreg_off: int         # file offset of the TRXREG row (decode anchor)
    clut: bool              # palette-shaped upload (tagged, not discarded)

    @property
    def width(self) -> int:
        """Decoded PSMT8 texture width (PSMCT32 transfer -> 2x per axis)."""
        return self.tw * 2

    @property
    def height(self) -> int:
        return self.th * 2

    def chunk_dir(self) -> str:
        """The chunk directory name (parent dir) of this upload's source."""
        parts = Path(self.src_rel).parts
        return parts[0] if parts else ""


def _is_clut_shaped(tw: int, th: int) -> bool:
    """A small PSMCT32 region the size of a palette, not a texture sheet."""
    if tw <= CLUT_MAX_DIM and th <= CLUT_MAX_DIM:
        return True
    # 256x1 / 1x256 linear palette strips
    if min(tw, th) <= 1 and tw * th <= CLUT_MAX_TEXELS:
        return True
    return False


# ---------------------------------------------------------------------------
# Scanning
# ---------------------------------------------------------------------------
def scan_file(path: Path, root: Path) -> list[Upload]:
    """Find every GS upload in one file.

    Builds on ``extract_subtextures.scan_transfers`` (which already pairs
    BITBLTBUF+TRXREG with the right validity checks) and additionally recovers
    the BITBLTBUF DBW/DPSM and the TRXPOS dest offset, and tags CLUT-shaped
    uploads. Returns a list of ``Upload`` records.
    """
    try:
        d = path.read_bytes()
    except OSError:
        return []
    out: list[Upload] = []
    try:
        src_rel = str(path.relative_to(root))
    except ValueError:
        src_rel = path.name
    for t in est.scan_transfers(d, path.name):
        # est.Transfer gives dbp/tw/th/trxreg_off. Recover the BITBLTBUF row
        # (it precedes the TRXREG; est anchors on the BITBLTBUF then walks
        # forward to TRXREG, so re-find the BITBLTBUF row owning this TRXREG).
        bb_off = _find_owning_bitbltbuf(d, t.trxreg_off)
        if bb_off is None:
            # Shouldn't happen (est found the pair), but be defensive.
            dbw = dpsm = 0
            bb_off = t.trxreg_off
            dsax = dsay = 0
        else:
            data_qw = int.from_bytes(d[bb_off:bb_off + 8], "little")
            _dbp, dbw, dpsm = _bitbltbuf_fields(data_qw)
            dsax, dsay = _scan_trxpos(d, bb_off, t.trxreg_off)
        out.append(Upload(
            dbp=t.dbp, dbw=dbw, dpsm=dpsm, tw=t.tw, th=t.th,
            dsax=dsax, dsay=dsay, src_rel=src_rel,
            offset=bb_off, trxreg_off=t.trxreg_off,
            clut=_is_clut_shaped(t.tw, t.th),
        ))
    return out


def _find_owning_bitbltbuf(d: bytes, trxreg_off: int) -> int | None:
    """Walk back from a TRXREG row to the BITBLTBUF (0x50) row that opened the
    transfer. est.scan_transfers anchors on BITBLTBUF and finds TRXREG within
    ~8 rows after it, so the BITBLTBUF is within 8 rows BEFORE the TRXREG."""
    for q in range(trxreg_off, max(-16, trxreg_off - 16 * 8) - 1, -16):
        if q < 0:
            break
        if est._gs_regwrite(d, q) == 0x50:
            return q
    return None


def _scan_trxpos(d: bytes, bb_off: int, trxreg_off: int) -> tuple[int, int]:
    """Find a TRXPOS (0x51) row between the BITBLTBUF and TRXREG and return its
    dest offset; (0, 0) if none (full-sheet upload to the buffer origin)."""
    for q in range(bb_off + 16, trxreg_off + 16, 16):
        if q + 16 > len(d):
            break
        if est._gs_regwrite(d, q) == 0x51:
            return _trxpos_dest(int.from_bytes(d[q:q + 8], "little"))
    return 0, 0


# ---------------------------------------------------------------------------
# The map
# ---------------------------------------------------------------------------
class ResidencyMap:
    """Global GS VRAM residency: DBP -> list of uploads, disc-wide."""

    def __init__(self, root: Path, uploads: list[Upload]):
        self.root = root
        self.uploads = uploads
        self.by_dbp: dict[int, list[Upload]] = {}
        for u in uploads:
            self.by_dbp.setdefault(u.dbp, []).append(u)

    # ---- construction -----------------------------------------------------
    @classmethod
    def scan(cls, root: Path, pattern: str = "*.bin") -> "ResidencyMap":
        """Walk every file under `root` matching `pattern` and build the map."""
        uploads: list[Upload] = []
        for p in sorted(root.rglob(pattern)):
            uploads.extend(scan_file(p, root))
        return cls(root, uploads)

    @classmethod
    def load_or_scan(cls, root: Path, cache_path: Path = DEFAULT_CACHE,
                     rebuild: bool = False,
                     pattern: str = "*.bin") -> "ResidencyMap":
        """Return a map from cache if the corpus is unchanged, else scan + cache."""
        sig = _corpus_signature(root, pattern)
        if not rebuild and cache_path.is_file():
            try:
                cached = json.loads(cache_path.read_text())
            except (OSError, ValueError):
                cached = None
            if (cached and cached.get("version") == CACHE_VERSION
                    and cached.get("signature") == sig
                    and cached.get("root") == str(root)):
                uploads = [Upload(**u) for u in cached["uploads"]]
                return cls(root, uploads)
        m = cls.scan(root, pattern)
        m.save_cache(cache_path, sig, pattern)
        return m

    def save_cache(self, cache_path: Path, sig: str | None = None,
                   pattern: str = "*.bin") -> None:
        if sig is None:
            sig = _corpus_signature(self.root, pattern)
        cache_path.parent.mkdir(parents=True, exist_ok=True)
        cache_path.write_text(json.dumps({
            "version": CACHE_VERSION,
            "root": str(self.root),
            "signature": sig,
            "uploads": [asdict(u) for u in self.uploads],
        }))

    # ---- lookup -----------------------------------------------------------
    def resolve_dbp(self, dbp: int, *,
                    prefer_dir: Path | None = None,
                    want_psm: int | None = None,
                    want_dims: tuple[int, int] | None = None,
                    include_clut: bool = False,
                    snap_tol: int = DEFAULT_SNAP_TOL) -> Upload | None:
        """Return the best upload for `dbp`, or None.

        Exact-DBP candidates are tried first, ranked by how well they match the
        caller's hints (PSM, decoded dims, same chunk dir), then by sheet size
        and file offset. If no exact match exists, the nearest-DBP upload within
        `snap_tol` VRAM words is used (the affine sheet_field->DBP map is exact
        only on the universal slot trio, so neighbouring addresses are the same
        physical sheet). CLUT uploads are excluded unless `include_clut`.
        """
        cands = self.candidates(dbp, include_clut=include_clut)
        if not cands and snap_tol > 0:
            # Snap to the nearest DBP that has a (texture) upload.
            best_d = None
            best_dist = snap_tol + 1
            for d in self.by_dbp:
                dist = abs(d - dbp)
                if dist < best_dist:
                    near = self.candidates(d, include_clut=include_clut)
                    if near:
                        best_dist = dist
                        best_d = d
            if best_d is None:
                return None
            cands = self.candidates(best_d, include_clut=include_clut)
        if not cands:
            return None
        prefer_name = prefer_dir.name if prefer_dir else None
        return min(cands, key=lambda u: self._rank(
            u, prefer_name, want_psm, want_dims))

    def candidates(self, dbp: int, *, include_clut: bool = False) -> list[Upload]:
        """All uploads at exactly `dbp` (textures only unless include_clut)."""
        ups = self.by_dbp.get(dbp, [])
        if include_clut:
            return list(ups)
        return [u for u in ups if not u.clut]

    @staticmethod
    def _rank(u: Upload, prefer_name: str | None,
              want_psm: int | None,
              want_dims: tuple[int, int] | None) -> tuple:
        """Sort key: smaller is better. Match the caller's hints first (PSM,
        decoded dims, same chunk dir), then break ties by source path + file
        offset.

        The same DBP is reused across many levels (VRAM is 4 MB), so a slot
        holds several differently-sized sheets; only the caller's context
        (prefer_dir / want_dims) can pick the right one. Absent a hint we use a
        STABLE path/offset ordering rather than "largest sheet" -- the largest
        sheet at a reused slot is usually an unrelated level's, whereas the
        lowest-sorted path is deterministic and reproduces the validated
        per-asset bindings (e.g. the player's 3rd sheet DBP 14592 ->
        chunk04.n2/f00_id44.bin)."""
        psm_miss = 0 if (want_psm is None or u.dpsm == want_psm) else 1
        dims_miss = 0
        if want_dims is not None:
            dims_miss = 0 if (u.width, u.height) == tuple(want_dims) else 1
        dir_miss = 0 if (prefer_name and u.chunk_dir() == prefer_name) else 1
        return (psm_miss, dims_miss, dir_miss, u.src_rel, u.offset)

    # ---- decode -----------------------------------------------------------
    def decode(self, u: Upload) -> tuple[int, int, bytes] | None:
        """Decode an upload to (width, height, PSMT8 pixels) via
        extract_subtextures.decode_transfer. Returns None on failure."""
        path = self.root / u.src_rel
        try:
            d = path.read_bytes()
        except OSError:
            return None
        t = est.Transfer(Path(u.src_rel).name, u.dbp, u.tw, u.th, u.trxreg_off,
                         src_path=path)
        if not est.decode_transfer(d, t):
            return None
        return t.width, t.height, t.pixels

    # ---- stats ------------------------------------------------------------
    def stats(self) -> dict:
        tex = [u for u in self.uploads if not u.clut]
        clut = [u for u in self.uploads if u.clut]
        dbps = sorted(self.by_dbp)
        tex_dbps = sorted({u.dbp for u in tex})
        return {
            "total_uploads": len(self.uploads),
            "texture_uploads": len(tex),
            "clut_uploads": len(clut),
            "distinct_dbps": len(dbps),
            "distinct_texture_dbps": len(tex_dbps),
            "dbps": dbps,
            "texture_dbps": tex_dbps,
        }


# ---------------------------------------------------------------------------
# Corpus signature (cache invalidation)
# ---------------------------------------------------------------------------
def _corpus_signature(root: Path, pattern: str) -> str:
    """A cheap fingerprint of (file, size, mtime) for every input file."""
    import hashlib
    h = hashlib.sha1()
    for p in sorted(root.rglob(pattern)):
        try:
            st = p.stat()
        except OSError:
            continue
        h.update(str(p.relative_to(root)).encode())
        h.update(f":{st.st_size}:{int(st.st_mtime)}".encode())
    return h.hexdigest()


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
def _print_report(m: ResidencyMap) -> None:
    s = m.stats()
    print(f"GS VRAM residency map  (root: {m.root})")
    print(f"  total uploads     : {s['total_uploads']}")
    print(f"  texture uploads   : {s['texture_uploads']}")
    print(f"  CLUT uploads      : {s['clut_uploads']}")
    print(f"  distinct DBPs     : {s['distinct_dbps']}")
    print(f"  distinct tex DBPs : {s['distinct_texture_dbps']}")
    print(f"  texture DBPs      : {s['texture_dbps']}")
    print("  per-DBP uploads:")
    for dbp in s["dbps"]:
        ups = m.by_dbp[dbp]
        tex = [u for u in ups if not u.clut]
        clut = [u for u in ups if u.clut]
        sample = ups[0]
        tag = " [CLUT]" if sample.clut else ""
        dims = f"{sample.width}x{sample.height}"
        dirs = sorted({u.chunk_dir() for u in ups})
        print(f"    DBP {dbp:6d}{tag}: {len(tex)} tex + {len(clut)} clut "
              f"uploads, {dims}, dirs={dirs[:6]}"
              f"{' ...' if len(dirs) > 6 else ''}")


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination global GS VRAM "
                                            "residency map")
    p.add_argument("--in", dest="root", default="extract",
                   help="extraction root to scan (default: extract)")
    p.add_argument("--cache", default=str(DEFAULT_CACHE),
                   help=f"cache JSON path (default: {DEFAULT_CACHE})")
    p.add_argument("--rebuild", action="store_true",
                   help="force a fresh scan even if the cache is valid")
    p.add_argument("--scan", action="store_true",
                   help="build/refresh the map (writes the cache)")
    p.add_argument("--report", action="store_true",
                   help="print summary statistics")
    p.add_argument("--resolve", type=int, metavar="DBP",
                   help="resolve one DBP and print the chosen upload")
    p.add_argument("--prefer-dir",
                   help="for --resolve: prefer uploads from this chunk dir")
    args = p.parse_args(argv)

    root = Path(args.root)
    if not root.is_dir():
        print(f"error: not a directory: {root}", file=sys.stderr)
        return 2
    cache = Path(args.cache)

    m = ResidencyMap.load_or_scan(root, cache, rebuild=args.rebuild)

    did = False
    if args.scan:
        print(f"scanned {len(m.uploads)} uploads -> cache {cache}")
        did = True
    if args.report or (not args.resolve and not args.scan):
        _print_report(m)
        did = True
    if args.resolve is not None:
        prefer = Path(args.prefer_dir) if args.prefer_dir else None
        u = m.resolve_dbp(args.resolve, prefer_dir=prefer)
        if u is None:
            print(f"DBP {args.resolve}: NO upload found disc-wide")
        else:
            exact = "exact" if u.dbp == args.resolve else f"snapped<-{u.dbp}"
            print(f"DBP {args.resolve}: {u.src_rel} @ {u.offset:#x} "
                  f"({u.width}x{u.height}, DPSM={u.dpsm:#x}, "
                  f"{'CLUT' if u.clut else 'texture'}, {exact})")
        did = True
    if not did:
        _print_report(m)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
