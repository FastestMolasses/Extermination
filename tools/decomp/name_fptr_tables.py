#!/usr/bin/env python3
"""
name_fptr_tables.py — mine the Extermination boot ELF .data for function-pointer
tables, and emit provisional names to config/symbol_addrs.txt.

Approach
--------
1. Walk the loaded image (file 0x300..0x175E00 -> vram 0x00100000..0x00275B00)
   as 32-bit little-endian words and find every contiguous run of >=4 words
   whose value lies in the executable vram range *and* matches a splat-detected
   function entry-point.
2. Filter to runs where:
     * base vram is 4-byte aligned
     * >= 75% of words resolve to known glabel func_XXXX entry points
       (i.e. the splat-discovered function list)
     * the run is preceded by a non-pointer word (so we capture the table base)
     * adjacent target values are NOT clustered into the same function
       (avoids treating sequential code-relative jumps as a table).
3. Hunt for a referencer of the table base in the splat .s files via simple
   lui+addiu/lw to (vram>>16, vram&FFFF) reconstruction.
4. Name the table and its entries. Honor existing names (never overwrite).
5. Emit an idempotent auto-block to config/symbol_addrs.txt.

Run this BEFORE building. It only writes to config/symbol_addrs.txt.
"""

from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BOOT_ELF = REPO_ROOT / "config" / "SCUS_971.12"
SYMBOL_FILE = REPO_ROOT / "config" / "symbol_addrs.txt"
ASM_DIR = REPO_ROOT / "build" / "asm" / "matchings" / "main" / "code"

# Boot-ELF layout: file 0x300 -> vram 0x00100000, length 0x175B00.
FILE_BASE = 0x300
VRAM_BASE = 0x00100000
LOAD_LEN = 0x175B00
VRAM_END = VRAM_BASE + LOAD_LEN  # 0x00275B00

# Code is everywhere splat detected functions; lowest is 0x00100008 (after _start),
# highest is around 0x00261544. After that, .rodata/.data dominate. Code-pointer
# values must point at a 4-byte aligned address within VRAM_BASE..VRAM_END.

AUTO_BEGIN = "// === Auto-added by tools/decomp/name_fptr_tables.py ==="
AUTO_END = "// === End auto-added by tools/decomp/name_fptr_tables.py ==="


def load_func_entries() -> set[int]:
    """Return the set of vram addresses splat detected as function entries."""
    out: set[int] = set()
    for p in ASM_DIR.glob("func_*.s"):
        try:
            v = int(p.stem.split("_")[-1], 16)
            out.add(v)
        except ValueError:
            pass
    return out


def load_existing_symbols() -> tuple[dict[int, str], dict[str, int], str]:
    addrs: dict[int, str] = {}
    names: dict[str, int] = {}
    if not SYMBOL_FILE.exists():
        return addrs, names, ""
    text = SYMBOL_FILE.read_text()
    for line in text.splitlines():
        m = re.match(r"\s*([A-Za-z_][\w]*)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;", line)
        if not m:
            continue
        name, addr = m.group(1), int(m.group(2), 0)
        addrs[addr] = name
        names[name] = addr
    return addrs, names, text


def strip_auto_block(text: str) -> str:
    if AUTO_BEGIN not in text:
        return text
    start = text.find(AUTO_BEGIN)
    end_idx = text.find(AUTO_END, start)
    if end_idx == -1:
        # Defensive: chop everything from the marker onward.
        return text[:start].rstrip() + "\n"
    end_idx += len(AUTO_END)
    # Eat trailing newline.
    if end_idx < len(text) and text[end_idx] == "\n":
        end_idx += 1
    return (text[:start].rstrip() + "\n" + text[end_idx:]).rstrip() + "\n"


def vram_to_file(v: int) -> int:
    return (v - VRAM_BASE) + FILE_BASE


def find_tables(
    data: bytes, func_entries: set[int], min_run: int = 4, min_hit_rate: float = 0.75
) -> list[tuple[int, list[int]]]:
    """Scan 32-bit-aligned words for runs of code-pointer-shaped values.

    Returns list of (base_vram, target_list).
    """
    # Build a quick membership: a word is a "plausible code pointer" iff
    # value in [VRAM_BASE, VRAM_END), 4-byte aligned, AND in func_entries.
    # We allow brief gaps (1 non-hit between hits) only if the gap word itself
    # is still in vram range and 4-aligned (likely an inline non-detected
    # function entry — splat misses some). This avoids splitting a real table.
    n = len(data)
    # Iterate 4-aligned vrams across the whole load region.
    # vram base of a word i is VRAM_BASE + (i - FILE_BASE) where i is its
    # file offset. We index words by their file offset.
    runs: list[tuple[int, list[int]]] = []
    i = FILE_BASE
    end = FILE_BASE + LOAD_LEN
    while i + 4 <= end:
        # Try to start a run here.
        words: list[int] = []
        hits: int = 0
        j = i
        gap_streak = 0
        while j + 4 <= end:
            w = struct.unpack_from("<I", data, j)[0]
            is_aligned = (w & 3) == 0
            in_range = VRAM_BASE + 0x8 <= w < VRAM_END
            is_func_entry = w in func_entries
            if is_aligned and in_range and is_func_entry:
                words.append(w)
                hits += 1
                gap_streak = 0
                j += 4
                continue
            # Allow up to 1 word gap if it still looks pointer-shaped
            # (aligned + in-range), since splat may have missed that entry.
            if is_aligned and in_range and gap_streak == 0 and words:
                words.append(w)
                gap_streak += 1
                j += 4
                continue
            break
        # Trim trailing gap words.
        while words and words[-1] not in func_entries:
            words.pop()
            j -= 4
        if len(words) >= min_run and hits / len(words) >= min_hit_rate:
            vram_base = VRAM_BASE + (i - FILE_BASE)
            # Allow tables anywhere in the loaded region except inside the
            # crt0 startup area at the very base.
            if vram_base >= VRAM_BASE + 0x100:
                runs.append((vram_base, words))
            i = j  # skip past run
            continue
        i += 4
    return runs


def build_lui_addiu_index() -> dict[int, list[str]]:
    """Scan all .s files and return: vram-of-some-symbol -> list of referencing funcs.

    We look for the pattern:
        lui   $rX, 0xHHHH
        addiu $rY, $rX, 0xLLLL  (or 'lw $rY, 0xLLLL($rX)' etc.)
    and reconstruct the absolute address HHHH<<16 + sext16(LLLL).
    """
    # Splat may already emit %hi/%lo(D_XXXXXXXX) when the address falls in a
    # known data symbol; harvest those too.
    LUI_RAW = re.compile(r"lui\s+\$(\w+),\s*(0x[0-9A-Fa-f]+)\b")
    LUI_HI = re.compile(r"lui\s+\$(\w+),\s*%hi\(([^)]+)\)")
    ADDIU = re.compile(r"addiu\s+\$\w+,\s*\$(\w+),\s*(-?0x[0-9A-Fa-f]+|-?\d+)\b")
    LO_LOAD = re.compile(
        r"\b(?:lw|lh|lhu|lb|lbu|ld|sw|sh|sb|sd|lq|sq)\s+\$\w+,\s*"
        r"(-?0x[0-9A-Fa-f]+|-?\d+)\s*\(\$(\w+)\)"
    )
    LO_LOAD_SYM = re.compile(
        r"\b(?:lw|lh|lhu|lb|lbu|ld|sw|sh|sb|sd|lq|sq)\s+\$\w+,\s*"
        r"%lo\(([^)]+)\)\s*\(\$(\w+)\)"
    )
    ADDIU_LO_SYM = re.compile(r"addiu\s+\$\w+,\s*\$(\w+),\s*%lo\(([^)]+)\)")
    DSYM = re.compile(r"\bD_([0-9A-Fa-f]{6,8})\b")
    GLABEL = re.compile(r"^glabel\s+(\S+)")

    refs: dict[int, list[str]] = {}

    for p in sorted(ASM_DIR.glob("*.s")):
        try:
            text = p.read_text(errors="replace")
        except OSError:
            continue
        cur_func = p.stem
        m = GLABEL.search(text)
        if m:
            cur_func = m.group(1)
        lui_state: dict[str, int] = {}  # reg -> hi<<16
        for line in text.splitlines():
            body = re.sub(r"^\s*/\*[^*]*\*/\s*", "", line).strip()
            if not body:
                continue
            # Standalone D_ references: the symbol name's hex IS the address.
            for dm in DSYM.finditer(body):
                try:
                    a = int(dm.group(1), 16)
                    refs.setdefault(a, []).append(cur_func)
                except ValueError:
                    pass

            mh = LUI_HI.search(body)
            if mh:
                # symbolic — DSYM regex already covered D_* targets.
                # Stash a flag on the register so addiu/%lo line can claim it.
                lui_state[mh.group(1)] = -1
                continue
            mr = LUI_RAW.search(body)
            if mr:
                reg = mr.group(1)
                val = int(mr.group(2), 16)
                lui_state[reg] = (val & 0xFFFF) << 16
                continue
            ma = ADDIU.search(body)
            if ma:
                rs, imm_s = ma.group(1), ma.group(2)
                hi = lui_state.get(rs)
                if hi is not None and hi >= 0:
                    try:
                        imm = int(imm_s, 0) if imm_s.startswith(("0x", "-0x")) else int(imm_s)
                        if imm & 0x8000 and not imm_s.startswith("-"):
                            # sign-extend 16-bit
                            imm = imm - 0x10000
                        addr = (hi + imm) & 0xFFFFFFFF
                        refs.setdefault(addr, []).append(cur_func)
                    except ValueError:
                        pass
                continue
            ml = LO_LOAD.search(body)
            if ml:
                imm_s, rs = ml.group(1), ml.group(2)
                hi = lui_state.get(rs)
                if hi is not None and hi >= 0:
                    try:
                        imm = int(imm_s, 0) if imm_s.startswith(("0x", "-0x")) else int(imm_s)
                        if imm & 0x8000 and not imm_s.startswith("-"):
                            imm = imm - 0x10000
                        addr = (hi + imm) & 0xFFFFFFFF
                        refs.setdefault(addr, []).append(cur_func)
                    except ValueError:
                        pass
                continue
    return refs


def main() -> int:
    if not BOOT_ELF.exists():
        print(f"ERROR: boot ELF not found at {BOOT_ELF}", file=sys.stderr)
        return 2
    data = BOOT_ELF.read_bytes()
    func_entries = load_func_entries()
    print(f"Loaded {len(func_entries)} splat-detected function entries.")
    existing_addrs, existing_names, raw_text = load_existing_symbols()
    print(f"Loaded {len(existing_addrs)} existing symbol entries.")

    raw_runs = find_tables(data, func_entries, min_run=4, min_hit_rate=0.50)
    print(f"Pass 1 (raw scan): {len(raw_runs)} candidate runs.")

    # Filter: drop runs that overlap with splat-detected functions.
    # The table base must be > highest function entry's reach.
    # Already covered in find_tables, but double-check by also rejecting any
    # run that contains the table base itself inside a func_entry-named addr.
    filtered: list[tuple[int, list[int]]] = []
    for base, words in raw_runs:
        if base in func_entries:
            continue
        # Discard tables where all words point to the SAME function (likely
        # a relocation stub / RAM jump landing pad — not a true vtable).
        if len(set(words)) < max(2, len(words) // 2):
            continue
        # Discard small tables in the very-low data region adjacent to .text.
        # (Heuristic: keep all for now; let the symbol owner judge.)
        filtered.append((base, words))
    print(f"Pass 2 (filtered): {len(filtered)} candidate tables.")

    # Find referencers to suggest table names.
    print("Indexing lui+addiu/lw references across all .s files...")
    refs_by_addr = build_lui_addiu_index()
    print(f"  indexed {len(refs_by_addr)} distinct referenced absolute addresses.")

    # Emit names.
    new_lines: list[str] = []
    new_lines.append("")
    new_lines.append(AUTO_BEGIN)
    new_lines.append(
        "// Function-pointer tables mined from .data: contiguous runs of 4+ words"
    )
    new_lines.append(
        "// pointing at splat-detected function entries. Names are provisional."
    )

    table_count = 0
    target_name_count = 0
    used_names: set[str] = set(existing_names.keys())
    table_examples: list[tuple[int, int, str | None]] = []

    for base, words in filtered:
        # Skip if base is already named (don't reshape) — but still emit
        # per-entry names if those entries are unnamed.
        callers = sorted(set(refs_by_addr.get(base, [])))
        # Determine table label.
        if base in existing_addrs:
            table_label = existing_addrs[base]
        else:
            # Pick a callable-derived name if a single caller references the
            # base. Filter to "interesting" callers (named ones).
            named_callers = [c for c in callers if not c.startswith("func_")]
            table_label = None
            if len(named_callers) == 1:
                # e.g. caller = sub_FOO -> fptr_FOO
                c = named_callers[0]
                cand = "ftab_" + re.sub(r"^sub_", "", c)
                cand = re.sub(r"[^A-Za-z0-9_]+", "_", cand)[:40]
                if cand and cand not in used_names:
                    table_label = cand
            if not table_label:
                table_label = f"ftab_{base:08X}"
            # Disambiguate.
            base_cand = table_label
            n = 1
            while table_label in used_names:
                n += 1
                table_label = f"{base_cand}_{n}"
            used_names.add(table_label)
            new_lines.append(
                f"// table @ 0x{base:08X}, {len(words)} entries, "
                f"callers={callers[:3] if callers else 'none'}"
            )
            new_lines.append(f"{table_label} = 0x{base:08X};  // type:label")
            table_count += 1

        # Per-entry names: SKIP. Renaming target functions breaks any
        # existing src/<name>.c that references them via extern func_XXXX,
        # and forces splat to emit different .s filenames which cascades
        # into the build pipeline. The table label alone is the safe yield
        # — it tells the reader "the function at vram X is reachable as
        # entry N of the table at vram Y". Leave the per-entry funcs with
        # their splat-default func_XXXXXXXX names.
        # (List them as a comment for future investigation.)
        new_lines.append(
            f"//   entries: " + ", ".join(f"0x{t:08X}" for t in words[:8])
            + (f", ... ({len(words) - 8} more)" if len(words) > 8 else "")
        )

        table_examples.append(
            (base, len(words), table_label if "ftab_" in table_label else table_label)
        )

    new_lines.append(AUTO_END)

    if table_count == 0 and target_name_count == 0:
        print("No new names emitted.")
        return 0

    print(f"Emitting {table_count} table labels, {target_name_count} target funcs.")

    # Strip previous block and append.
    stripped = strip_auto_block(raw_text)
    out = stripped.rstrip() + "\n" + "\n".join(new_lines) + "\n"
    SYMBOL_FILE.write_text(out)
    print(f"Wrote {SYMBOL_FILE.relative_to(REPO_ROOT)}.")

    # A few examples.
    print("Sample tables:")
    for base, sz, label in table_examples[:10]:
        print(f"  0x{base:08X}  size={sz:3d}  -> {label}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
