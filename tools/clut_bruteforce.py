#!/usr/bin/env python3
"""Brute-force CLUT (palette) recovery for Extermination textures.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing. All PNG output
lands under a git-ignored directory.

THE PROBLEM
-----------
Extermination's textures are 8-bit indexed (PSMT8): each texel is an index
into a 256-entry RGBA palette (a CLUT). The game does NOT ship CLUTs as GS
upload packets (confirmed by tools/vram_residency.py: zero CLUT GS uploads
disc-wide); the palettes live as raw 1024-byte [R G B A]*256 blobs scattered
through model / level files, and the engine-side binding (which blob colours
which sheet) is built at runtime from boot-ELF data we can't yet trace without
PCSX2. So our texture exports come out grayscale (identity ramp).

THE IDEA (no emulator needed)
-----------------------------
Collect EVERY 1024-byte CLUT-shaped blob on disc into a candidate pool. For a
given texture sheet (8-bit indices), apply every candidate CLUT and SCORE the
resulting RGBA image by how "natural" / coherent it looks. The correct palette
should produce a far more spatially-coherent image than a wrong one, because
(a) textures are locally smooth -- neighbouring texels map to similar colours
-- and (b) the game's PSMT8 indices are luminance-ordered (documented), so the
correct CLUT is itself roughly luminance-monotonic.

COHERENCE METRIC
----------------
The primary score is SPATIAL SMOOTHNESS: the mean absolute RGB difference
between horizontally- and vertically-adjacent texels of the decoded image,
restricted to NON-CONSTANT texel pairs (so vast flat padding regions of an
atlas don't dominate). Lower = smoother = more natural. We also compute, as
diagnostics / tie-breakers:

  * palette monotonicity: how luminance-monotone the CLUT is when walked in
    index order (the indices are luminance-ordered, so a correct CLUT's
    luminance should rise roughly monotonically with index);
  * used-palette compactness: a natural texture uses a connected band of the
    palette, not scattered entries;
  * a degeneracy guard: a CLUT that collapses the image to ~one colour scores
    artificially smooth, so results whose decoded image has too few distinct
    colours are penalised.

This is a HEURISTIC recovery, not a proof. A confident NEGATIVE result (scores
flat / ambiguous across candidates) is as valuable as a positive one: it tells
us coherence scoring cannot disambiguate the palette and the binding truly
needs the engine.

USAGE
    python3 tools/clut_bruteforce.py --list-sheets
    python3 tools/clut_bruteforce.py --dbp 14592 --top 3 --out scratch/clut_bf
    python3 tools/clut_bruteforce.py --all-dbps --top 3 --out scratch/clut_bf
"""
from __future__ import annotations

import argparse
import hashlib
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

from clut import (  # noqa: E402
    find_clut_candidates,
    identity_grayscale_clut,
    psmct8_csm1_swizzle,
    write_png_rgba,
)
from vram_residency import ResidencyMap  # noqa: E402


# ---------------------------------------------------------------------------
# Candidate-CLUT pool
# ---------------------------------------------------------------------------
@dataclass
class ClutCand:
    """One unique candidate CLUT blob and where it was found on disc."""
    sha1: str
    blob: bytes                       # 1024 bytes, raw [R G B A]*256
    locations: list[tuple[str, int]]  # (rel_path, file_offset)


def collect_clut_pool(root: Path,
                      min_alpha_80: int = 100,
                      min_distinct_rgb: int = 32) -> list[ClutCand]:
    """Walk `root` and return the deduped pool of candidate CLUT blobs."""
    pool: dict[str, ClutCand] = {}
    for p in sorted(root.rglob("*.bin")):
        try:
            d = p.read_bytes()
        except OSError:
            continue
        rel = str(p.relative_to(root))
        for off in find_clut_candidates(d, min_alpha_80=min_alpha_80,
                                        min_distinct_rgb=min_distinct_rgb):
            blob = d[off:off + 1024]
            h = hashlib.sha1(blob).hexdigest()
            c = pool.get(h)
            if c is None:
                c = pool[h] = ClutCand(h, blob, [])
            c.locations.append((rel, off))
    return list(pool.values())


# ---------------------------------------------------------------------------
# Coherence scoring
# ---------------------------------------------------------------------------
def _luma(r: int, g: int, b: int) -> float:
    return 0.299 * r + 0.587 * g + 0.114 * b


def index_histogram(indices: bytes) -> list[int]:
    """Count of each of the 256 index values used in the texture."""
    h = [0] * 256
    for b in indices:
        h[b] += 1
    return h


def palette_monotonicity(clut_rgba: bytes, hist: list[int]) -> float:
    """Fraction of adjacent USED-index pairs whose luminance is non-decreasing.

    The PSMT8 indices are luminance-ordered, so for the correct CLUT the
    luminance of consecutive *used* palette entries should mostly rise. 1.0 =
    perfectly monotone, ~0.5 = random. Only entries the texture actually uses
    are considered.
    """
    used = [i for i in range(256) if hist[i] > 0]
    if len(used) < 2:
        return 0.0
    lum = [_luma(clut_rgba[i * 4], clut_rgba[i * 4 + 1], clut_rgba[i * 4 + 2])
           for i in used]
    nondec = sum(1 for a, b in zip(lum, lum[1:]) if b >= a - 1.0)
    return nondec / (len(lum) - 1)


def collect_adjacent_pairs(indices: bytes, width: int, height: int,
                           max_pairs: int = 400_000) -> list[tuple[int, int]]:
    """All adjacent (H + V) texel index pairs whose two indices DIFFER.

    Equal-index pairs carry no colour information for ANY palette (a flat run
    decodes flat under every CLUT), so counting them would let a mostly-flat
    atlas region make every palette look equally smooth -- the degenerate trap.
    We keep only transitions. To bound the cost on a 786K-texel sheet, pairs
    are uniformly subsampled to `max_pairs` (the statistics are stable well
    below the full set). This list is palette-INDEPENDENT, so it is computed
    once per sheet and reused for all candidates.
    """
    pairs: list[tuple[int, int]] = []
    for y in range(height):
        row = y * width
        for x in range(width - 1):
            a = indices[row + x]
            b = indices[row + x + 1]
            if a != b:
                pairs.append((a, b))
    for y in range(height - 1):
        row = y * width
        nrow = row + width
        for x in range(width):
            a = indices[row + x]
            b = indices[nrow + x]
            if a != b:
                pairs.append((a, b))
    if len(pairs) > max_pairs:
        step = len(pairs) / max_pairs
        pairs = [pairs[int(i * step)] for i in range(max_pairs)]
    return pairs


def coherence_scores(pairs: list[tuple[int, int]],
                     clut_rgba: bytes) -> tuple[float, float]:
    """Score a CLUT against pre-collected adjacent-index pairs.

    Returns (mean_color_delta, idx_color_correlation):

    * mean_color_delta -- mean abs-RGB distance over transition pairs. Lower =
      smoother. NOTE: this is minimised by a flat/gray palette, so it is a
      tie-breaker, NOT the primary discriminator.

    * idx_color_correlation -- Pearson r between |index delta| and |colour
      delta| across the transition pairs. This is the PRIMARY discriminator and
      is degeneracy-robust: for a CORRECT luminance-ordered palette, a small
      index step maps to a small colour step and a large index step to a large
      colour step, so r is strongly positive. A RANDOM palette destroys that
      relationship (r ~ 0). A FLAT/gray palette has near-zero colour-delta
      variance, so r is undefined/near-zero -- it cannot fake a high r. Thus
      ranking by HIGH correlation rewards true structure without being fooled
      by desaturation (the flaw a pure-smoothness score has).
    """
    tbl = [(clut_rgba[i * 4], clut_rgba[i * 4 + 1], clut_rgba[i * 4 + 2])
           for i in range(256)]
    n = len(pairs)
    if n == 0:
        return 0.0, 0.0
    sum_cd = 0.0
    sx = sy = sxx = syy = sxy = 0.0
    for a, b in pairs:
        ca, cb = tbl[a], tbl[b]
        cd = abs(ca[0] - cb[0]) + abs(ca[1] - cb[1]) + abs(ca[2] - cb[2])
        idx_d = abs(a - b)
        sum_cd += cd
        sx += idx_d
        sy += cd
        sxx += idx_d * idx_d
        syy += cd * cd
        sxy += idx_d * cd
    mean_cd = sum_cd / n
    cov = sxy - sx * sy / n
    vx = sxx - sx * sx / n
    vy = syy - sy * sy / n
    denom = (vx * vy) ** 0.5
    corr = (cov / denom) if denom > 1e-9 else 0.0
    return mean_cd, corr


def palette_chroma(clut_rgba: bytes, used: list[int]) -> float:
    """Mean per-entry chroma (max-min of RGB) over used entries.

    ~0 for a grayscale palette, large for a vivid one. Used to flag whether the
    winning palette actually produces COLOUR or just another grayscale.
    """
    if not used:
        return 0.0
    tot = 0
    for i in used:
        r, g, b = clut_rgba[i * 4], clut_rgba[i * 4 + 1], clut_rgba[i * 4 + 2]
        tot += max(r, g, b) - min(r, g, b)
    return tot / len(used)


def distinct_colors(indices_used: list[int], clut_rgba: bytes) -> int:
    """Number of distinct RGB triples the used indices map to under this CLUT.

    A degenerate CLUT that maps everything to ~one colour scores artificially
    smooth; this lets us penalise it.
    """
    s = set()
    for i in indices_used:
        s.add(clut_rgba[i * 4:i * 4 + 3])
    return len(s)


@dataclass
class ScoreResult:
    cand: ClutCand
    swizzled: bool          # was the CSM1 entry-swap applied to the blob?
    correlation: float      # |idx delta| vs |colour delta| Pearson r (PRIMARY, higher better)
    smoothness: float       # mean adjacent-RGB diff over transition pairs (tie-break, lower)
    monotonicity: float     # 0..1 luminance-monotone over used indices (higher better)
    chroma: float           # mean per-entry chroma over used entries (vividness)
    n_distinct: int         # distinct colours among used indices (degeneracy guard)


def score_candidate(pairs: list[tuple[int, int]],
                    hist: list[int], used: list[int],
                    cand: ClutCand, swizzled: bool) -> ScoreResult:
    blob = psmct8_csm1_swizzle(cand.blob) if swizzled else cand.blob
    sm, corr = coherence_scores(pairs, blob)
    mono = palette_monotonicity(blob, hist)
    chroma = palette_chroma(blob, used)
    nd = distinct_colors(used, blob)
    return ScoreResult(cand, swizzled, corr, sm, mono, chroma, nd)


# Minimum distinct colours for a result to count as non-degenerate. A natural
# texture that uses N indices will, under the correct palette, light up most of
# them as visibly distinct colours; a degenerate palette collapses them.
MIN_DISTINCT_FRACTION = 0.20


# ---------------------------------------------------------------------------
# Per-sheet brute force
# ---------------------------------------------------------------------------
def get_sheet_pixels(rmap: ResidencyMap, dbp: int,
                     prefer_dir: Path | None = None):
    """Resolve a DBP to a decoded PSMT8 sheet. Returns (width, height, indices, upload)."""
    up = rmap.resolve_dbp(dbp, prefer_dir=prefer_dir)
    if up is None:
        return None
    dec = rmap.decode(up)
    if dec is None:
        return None
    w, h, px = dec
    return w, h, px, up


def brute_force_sheet(indices: bytes, width: int, height: int,
                      pool: list[ClutCand],
                      try_swizzle: bool = True) -> list[ScoreResult]:
    """Score every candidate CLUT against one sheet. Returns results sorted
    best-first.

    Ranking primary key is the index/colour-delta CORRELATION (descending) --
    the degeneracy-robust discriminator (see ``coherence_scores``). Candidates
    failing the distinct-colour degeneracy guard are pushed to the back so a
    palette that collapses the image cannot win on a spurious correlation.
    """
    hist = index_histogram(indices)
    used = [i for i in range(256) if hist[i] > 0]
    min_distinct = max(2, int(len(used) * MIN_DISTINCT_FRACTION))
    pairs = collect_adjacent_pairs(indices, width, height)

    results: list[ScoreResult] = []
    for cand in pool:
        results.append(score_candidate(pairs, hist, used, cand, swizzled=False))
        if try_swizzle:
            results.append(score_candidate(pairs, hist, used, cand, swizzled=True))

    def key(r: ScoreResult):
        degenerate = r.n_distinct < min_distinct
        # Non-degenerate first; then highest correlation; then smoothest.
        return (degenerate, -r.correlation, r.smoothness)

    results.sort(key=key)
    return results, len(pairs)


def sheet_index_stats(indices: bytes) -> dict:
    """Diagnostics about the index image itself (palette-independent)."""
    hist = index_histogram(indices)
    used = [i for i in range(256) if hist[i] > 0]
    # Adjacent-index delta on the raw indices: documented to be ~7-22 for
    # luminance-ordered indices vs ~85 for random. Re-measure here.
    return {
        "n_texels": len(indices),
        "n_used_indices": len(used),
        "index_min": used[0] if used else None,
        "index_max": used[-1] if used else None,
    }


# ---------------------------------------------------------------------------
# Reporting helpers
# ---------------------------------------------------------------------------
def confidence_gap(results: list[ScoreResult]) -> dict:
    """Quantify how clearly the winning CORRELATION stands out from the field.

    A large z-distance of the top correlation above the mean = confident winner;
    ~0 = flat distribution = coherence scoring cannot disambiguate the palette.
    """
    cs = sorted((r.correlation for r in results), reverse=True)
    if len(cs) < 3:
        return {"best": 0.0, "second": 0.0, "median": 0.0, "mean": 0.0,
                "std": 0.0, "z": 0.0, "gap_to_second": 0.0}
    best = cs[0]
    second = cs[1]
    median = cs[len(cs) // 2]
    mean = sum(cs) / len(cs)
    var = sum((x - mean) ** 2 for x in cs) / len(cs)
    std = var ** 0.5
    return {
        "best": best,
        "second": second,
        "median": median,
        "mean": mean,
        "std": std,
        "z": ((best - mean) / std) if std > 0 else 0.0,   # std above mean
        "gap_to_second": (best - second),
    }


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Brute-force CLUT recovery by "
                                            "image-coherence scoring")
    p.add_argument("--in", dest="root", default="extract",
                   help="extraction root (default: extract)")
    p.add_argument("--out", default="scratch/clut_bf",
                   help="output dir for top-N PNGs (default: scratch/clut_bf)")
    p.add_argument("--dbp", type=int, action="append",
                   help="DBP of a sheet to brute-force (repeatable)")
    p.add_argument("--all-dbps", action="store_true",
                   help="brute-force every distinct texture DBP")
    p.add_argument("--prefer-dir",
                   help="prefer sheet uploads from this chunk dir when "
                        "resolving the DBP")
    p.add_argument("--top", type=int, default=3,
                   help="write the top-N candidates as PNGs (default: 3)")
    p.add_argument("--no-swizzle", action="store_true",
                   help="do not also try the CSM1 entry-swap form of each CLUT")
    p.add_argument("--min-chroma", type=float, default=0.0,
                   help="only consider candidate CLUTs whose mean per-entry "
                        "chroma exceeds this (force a COLOUR palette; default 0 "
                        "= no filter). Use to test whether a vivid palette can "
                        "ever produce a coherent image for a sheet.")
    p.add_argument("--list-sheets", action="store_true",
                   help="list resolvable texture DBPs and exit")
    args = p.parse_args(argv)

    root = Path(args.root)
    rmap = ResidencyMap.load_or_scan(root)

    if args.list_sheets:
        s = rmap.stats()
        print("Resolvable texture DBPs (each may have many uploads):")
        for dbp in s["texture_dbps"]:
            ups = rmap.candidates(dbp)
            dirs = sorted({u.chunk_dir() for u in ups})
            print(f"  DBP {dbp:6d}: {len(ups)} uploads, "
                  f"{ups[0].width}x{ups[0].height}, dirs={dirs[:5]}"
                  f"{' ...' if len(dirs) > 5 else ''}")
        return 0

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    print("Collecting disc-wide CLUT candidate pool ...", flush=True)
    pool = collect_clut_pool(root)
    n_locs = sum(len(c.locations) for c in pool)
    print(f"  {len(pool)} unique CLUT blobs ({n_locs} total locations)")
    if args.min_chroma > 0:
        all_idx = list(range(256))
        pool = [c for c in pool
                if palette_chroma(c.blob, all_idx) >= args.min_chroma]
        print(f"  filtered to {len(pool)} candidates with chroma >= "
              f"{args.min_chroma}")
    print()

    prefer = Path(args.prefer_dir) if args.prefer_dir else None
    dbps: list[int]
    if args.all_dbps:
        dbps = rmap.stats()["texture_dbps"]
    elif args.dbp:
        dbps = args.dbp
    else:
        print("nothing to do: pass --dbp N, --all-dbps, or --list-sheets")
        return 1

    for dbp in dbps:
        got = get_sheet_pixels(rmap, dbp, prefer_dir=prefer)
        if got is None:
            print(f"DBP {dbp}: could not resolve / decode a sheet -- skipped")
            continue
        w, h, indices, up = got
        stats = sheet_index_stats(indices)
        print(f"=== DBP {dbp}  ({w}x{h}, from {up.src_rel}) ===")
        print(f"    texels={stats['n_texels']}  distinct indices="
              f"{stats['n_used_indices']}  range=[{stats['index_min']},"
              f"{stats['index_max']}]")

        results, npairs = brute_force_sheet(indices, w, h, pool,
                                             try_swizzle=not args.no_swizzle)
        gap = confidence_gap(results)
        print(f"    transition pairs scored: {npairs}")
        print(f"    correlation  best={gap['best']:.3f}  2nd={gap['second']:.3f}"
              f"  median={gap['median']:.3f}  mean={gap['mean']:.3f}"
              f"  std={gap['std']:.3f}")
        print(f"    confidence   z(best above mean)={gap['z']:.2f}  "
              f"gap_to_2nd={gap['gap_to_second']:.3f}")

        # Write the top-N candidate decodes plus the grayscale reference.
        gray = identity_grayscale_clut()
        write_png_rgba(out_dir / f"dbp{dbp}_w{w}x{h}_REF_gray.png",
                       w, h, _apply(indices, gray))
        for rank, r in enumerate(results[:args.top]):
            blob = (psmct8_csm1_swizzle(r.cand.blob) if r.swizzled
                    else r.cand.blob)
            loc0 = r.cand.locations[0]
            tag = f"{loc0[0].replace('/', '_')}@{loc0[1]:#x}"
            sw = "_csm1" if r.swizzled else ""
            name = (f"dbp{dbp}_rank{rank}_corr{r.correlation:+.3f}"
                    f"_chr{r.chroma:05.1f}_mono{r.monotonicity:.2f}"
                    f"_nd{r.n_distinct}{sw}_{tag}.png")
            write_png_rgba(out_dir / name, w, h, _apply(indices, blob))
            print(f"    rank {rank}: corr={r.correlation:+.3f} "
                  f"sm={r.smoothness:6.1f} mono={r.monotonicity:.2f} "
                  f"chroma={r.chroma:5.1f} distinct={r.n_distinct} "
                  f"swizzled={r.swizzled} <- {loc0[0]} @ {loc0[1]:#x}"
                  + (f" (+{len(r.cand.locations)-1} more locs)"
                     if len(r.cand.locations) > 1 else ""))
        print()

    print(f"PNGs written to {out_dir}/ (git-ignored).")
    return 0


def _apply(indices: bytes, clut_rgba: bytes) -> bytes:
    """Local fast CLUT apply (avoids importing apply_clut's per-pixel slice)."""
    tbl = [clut_rgba[i * 4:i * 4 + 4] for i in range(256)]
    out = bytearray(len(indices) * 4)
    for i, idx in enumerate(indices):
        out[i * 4:i * 4 + 4] = tbl[idx]
    return bytes(out)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
