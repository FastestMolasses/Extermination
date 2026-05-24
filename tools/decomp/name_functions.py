#!/usr/bin/env python3
"""
name_functions.py — heuristic function naming for the Extermination boot ELF.

Adds names to config/symbol_addrs.txt based on three independent heuristics:

  1. Standard-library signature matching (memset / memcpy / strlen / strcmp /
     strcpy patterns, etc.) — high-confidence, low yield.
  2. String-reference naming — finds lui+addiu(/lw/sw/lh/lb/...) pairs in each
     function's disassembly, recovers the absolute 32-bit target address, looks
     up the address in the ELF's extracted-string table, and proposes a name
     derived from the most distinctive nearby string.
  3. Library-banner naming — the SDK ships per-library version banner strings
     ("PsIIlibgraph2000", "PsIIlibdma  2000", etc.); functions that reference
     them are the per-library init/print helpers.

The script never overwrites existing names in symbol_addrs.txt; it only appends
new ones in a clearly-marked block. Run again any time to refresh.

Usage:
    python3 tools/decomp/name_functions.py [--dry-run] [--no-strings]
                                            [--no-stdlib] [--no-banner]
                                            [--min-string-len 12]

The script reads only project files plus the boot ELF (which is user-supplied
locally, never committed) and the splat .s disassembly. It writes only to
config/symbol_addrs.txt.
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
BOOT_ELF = REPO_ROOT / "config" / "SCUS_971.12"
SYMBOL_FILE = REPO_ROOT / "config" / "symbol_addrs.txt"
ASM_DIR = REPO_ROOT / "build" / "asm" / "matchings" / "main" / "code"

# Boot-ELF layout: PT_LOAD at file 0x300 -> vram 0x00100000, length 0x175B00.
FILE_BASE = 0x300
VRAM_BASE = 0x00100000
LOAD_LEN = 0x175B00

# Lines in splat output look like:
#   /* 738 00100438 2700043C */   lui       $a0, %hi(D_0026AE80)
#   /* 73C 0010043C 808DA464 */   ld        $a0, -0x7280($a0)
#   /* 5B0 001002B0 2400053C */  lui        $a1, %hi(D_00241020)
#   /* 5B4 001002B4 0010063C */  lui        $a2, (0x10005000 >> 16)
LUI_RE = re.compile(
    r"lui\s+\$(\w+),\s*(?:%hi\(([^)]+)\)|\((0x[0-9A-Fa-f]+)\s*>>\s*16\)|(0x[0-9A-Fa-f]+))"
)
# An instruction that uses %lo / immediate offset against a base register.
# Captures: mnemonic, dest reg, offset-expr (raw text inside parens prefix), base reg.
LO_RE = re.compile(
    r"\b(addiu|ori|lw|lh|lhu|lb|lbu|ld|sw|sh|sb|sd|lq|sq|lwc1|swc1|ldc1|sdc1)\s+\$(\w+),\s*([^,]+?)\(\$(\w+)\)"
)
# addiu form: addiu $rt, $rs, imm   OR   addiu $rt, $rs, %lo(SYM)
ADDIU_RE = re.compile(
    r"\b(addiu|ori)\s+\$(\w+),\s*\$(\w+),\s*(%lo\(([^)]+)\)|-?0x[0-9A-Fa-f]+|-?\d+)"
)
# Standalone reference to a D_XXXXXXXX symbol anywhere in a line — useful to
# capture targets even when we don't reconstruct lui+lo pairing perfectly.
DSYM_RE = re.compile(r"\bD_([0-9A-Fa-f]{6,8})\b")
HISYM_RE = re.compile(r"%(?:hi|lo)\(([A-Za-z_]\w*)\)")
GLABEL_RE = re.compile(r"^glabel\s+(\S+)")


@dataclass
class StringRef:
    vram: int
    text: str


def load_existing_symbols() -> tuple[dict[int, str], str, set[str]]:
    """Return (addr->name, full_file_text, set_of_existing_names)."""
    addrs: dict[int, str] = {}
    names: set[str] = set()
    if not SYMBOL_FILE.exists():
        return addrs, "", names
    text = SYMBOL_FILE.read_text()
    for line in text.splitlines():
        # name = 0xVRAM;  // type:...
        m = re.match(r"\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;", line)
        if not m:
            continue
        name, addr = m.group(1), int(m.group(2), 0)
        addrs[addr] = name
        names.add(name)
    return addrs, text, names


def extract_strings(min_len: int = 6) -> list[StringRef]:
    """Read the boot ELF and pull printable ASCII strings with their vram."""
    data = BOOT_ELF.read_bytes()
    out: list[StringRef] = []
    i = 0
    n = len(data)
    while i < n:
        # Find next printable run.
        j = i
        while j < n and 0x20 <= data[j] < 0x7F:
            j += 1
        if j - i >= min_len and data[j] == 0 if j < n else False:
            s = data[i:j].decode("ascii", "replace")
            # File offset i -> vram.
            file_off = i
            if FILE_BASE <= file_off < FILE_BASE + LOAD_LEN:
                vram = (file_off - FILE_BASE) + VRAM_BASE
                # Filter: require at least 3 alphabetic chars
                if sum(1 for c in s if c.isalpha()) >= 3:
                    out.append(StringRef(vram, s))
            i = j + 1
        else:
            i = j + 1
    return out


def slugify(s: str, max_len: int = 32) -> str:
    """Convert a string into a C-identifier-safe slug."""
    s = re.sub(r"[^A-Za-z0-9_]+", "_", s)
    s = re.sub(r"_+", "_", s).strip("_")
    if not s:
        return ""
    if s[0].isdigit():
        s = "_" + s
    return s[:max_len]


def is_code_file(path: Path) -> bool:
    """Splat marks code with `glabel`, data with `dlabel`. Filter out data."""
    try:
        with path.open() as fh:
            for line in fh:
                if line.startswith("glabel "):
                    return True
                if line.startswith("dlabel "):
                    return False
    except OSError:
        pass
    return False


def parse_asm_file(path: Path) -> tuple[str, int, list[int]]:
    """Return (func_name, vram, list_of_referenced_absolute_addresses).

    Sources of addresses:
      * Any D_XXXXXXXX literal anywhere on a line (splat resolved a lui/lo
        pair to this auto-symbol — its hex name IS the absolute address).
      * %hi/%lo(SYM) with SYM matching D_XXXXXXXX.
      * Bare `lui $r, (0xXXXXXXXX >> 16)` lines (FP constants etc.) — also
        try to combine with a following addiu/load.
    """
    func_name = path.stem
    try:
        func_vram = int(func_name.split("_")[-1], 16)
    except ValueError:
        func_vram = 0
    text = path.read_text(errors="replace")
    targets: list[int] = []
    lui_val: dict[str, int] = {}
    for line in text.splitlines():
        body = re.sub(r"^\s*/\*[^*]*\*/\s*", "", line)
        body = body.strip()
        if not body:
            continue
        # D_XXXXXXXX literals — splat-resolved data refs.
        for m in DSYM_RE.finditer(body):
            try:
                addr = int(m.group(1), 16)
                targets.append(addr)
            except ValueError:
                pass
        # Raw lui with full 32-bit literal (FP constants, IO addresses).
        m = LUI_RE.search(body)
        if m:
            reg = m.group(1)
            if m.group(3):
                full = int(m.group(3), 16)
                targets.append(full)
                lui_val[reg] = (full >> 16) & 0xFFFF
            elif m.group(4):
                lui_val[reg] = int(m.group(4), 16)
            elif m.group(2):
                # %hi(SYM) — handled via DSYM_RE above for D_ symbols.
                pass
            continue
        # addiu using a previously lui-loaded reg.
        am = ADDIU_RE.search(body)
        if am:
            _, _rt, rs, imm_s = am.group(1), am.group(2), am.group(3), am.group(4)
            if rs in lui_val and not imm_s.startswith("%lo"):
                try:
                    imm = (
                        int(imm_s, 16) if imm_s.startswith(("0x", "-0x")) else int(imm_s)
                    )
                    hi = lui_val[rs]
                    if imm < 0:
                        imm = imm  # keep negative
                    target = ((hi << 16) + imm) & 0xFFFFFFFF
                    targets.append(target)
                except ValueError:
                    pass
        lm = LO_RE.search(body)
        if lm:
            _, _rt, off_s, base = lm.groups()
            off_s = off_s.strip()
            if base in lui_val:
                try:
                    if off_s.startswith(("0x", "-0x")):
                        off = int(off_s, 16)
                    elif off_s.startswith("%lo"):
                        continue
                    else:
                        off = int(off_s)
                    hi = lui_val[base]
                    target = ((hi << 16) + off) & 0xFFFFFFFF
                    targets.append(target)
                except ValueError:
                    pass
    return func_name, func_vram, targets


def build_string_index(strings: list[StringRef]) -> dict[int, str]:
    return {s.vram: s.text for s in strings}


# ------------------- Stdlib signature matching -------------------

# Pattern: word-list of expected instructions in order. We pull the raw
# 32-bit opcode from splat's `/* off vram WORD */` prefix and compare a
# tuple of words.
WORD_RE = re.compile(r"/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+\*/")


def function_words(path: Path) -> list[int]:
    out = []
    for line in path.read_text(errors="replace").splitlines():
        m = WORD_RE.search(line)
        if m:
            # Splat prints the LE bytes as displayed in file: bytes-MSB-first
            # form. The actual instruction is little-endian, so to get the
            # opcode integer we need the reversed byte order.
            raw = m.group(1)
            be = int(raw, 16)
            # Re-reverse to get the LE-encoded instruction word.
            le = (
                ((be & 0xFF) << 24)
                | ((be & 0xFF00) << 8)
                | ((be & 0xFF0000) >> 8)
                | ((be & 0xFF000000) >> 24)
            )
            out.append(le)
    return out


# We avoid hard-coding exact instruction-word matches because mwcc's scheduler
# can vary even within "the same" function. Instead, classify by *mnemonic
# fingerprint*: count of each opcode/function family and approximate length.
MNEMONIC_RE = re.compile(r"^\s*([a-z][a-z0-9.]*)\b", re.MULTILINE)


def mnemonic_fingerprint(path: Path) -> tuple[int, dict[str, int]]:
    text = path.read_text(errors="replace")
    counts: dict[str, int] = {}
    total = 0
    for line in text.splitlines():
        body = re.sub(r"^\s*/\*[^*]*\*/\s*", "", line).strip()
        if not body or body.startswith((".", "glabel", "endlabel", "nonmatching")):
            continue
        m = MNEMONIC_RE.match(body)
        if not m:
            continue
        mn = m.group(1)
        counts[mn] = counts.get(mn, 0) + 1
        total += 1
    return total, counts


def detect_stdlib(path: Path) -> str | None:
    """Heuristically detect well-known leaf stdlib functions.

    Returns a candidate name, or None. Conservative: only fires on very
    distinctive patterns to keep false-positive rate near zero.
    """
    total, mn = mnemonic_fingerprint(path)
    if total == 0 or total > 60:
        return None

    # memset: tight sb loop with a byte counter, no jal calls, no fp.
    #   typical 8-14 instructions; instrs include: bne, sb, addiu, jr.
    #   no lw/sw (just sb), no lui (no globals).
    # memcpy: lb/lbu + sb pair in a loop OR lw + sw pair.
    # strlen: lb/lbu + bnez + addiu loop, ends with subtraction returning count.
    # strcmp: lbu + bne + lbu loop returning v0 = difference.
    # strcpy: lb + sb + bnez loop.

    if mn.get("jal", 0) or mn.get("jalr", 0):
        return None  # Stdlib leaves don't call out.

    if mn.get("lui", 0) > 0:
        return None  # Should not reference globals.

    has_branch = mn.get("bne", 0) + mn.get("beq", 0) + mn.get("bnez", 0) + mn.get(
        "beqz", 0
    )
    has_jr = mn.get("jr", 0)
    if not has_branch or not has_jr:
        return None

    # memset: only sb stores, no loads, exactly one branch.
    if (
        mn.get("sb", 0) >= 1
        and mn.get("sw", 0) == 0
        and mn.get("sh", 0) == 0
        and mn.get("sd", 0) == 0
        and mn.get("sq", 0) == 0
        and mn.get("lw", 0) == 0
        and mn.get("lb", 0) == 0
        and mn.get("lbu", 0) == 0
        and mn.get("lh", 0) == 0
        and mn.get("lhu", 0) == 0
        and has_branch == 1
        and total <= 12
    ):
        return "memset"

    # strlen: lbu loop, no stores, returns count.
    if (
        (mn.get("lb", 0) + mn.get("lbu", 0)) >= 1
        and mn.get("sb", 0) == 0
        and mn.get("sw", 0) == 0
        and mn.get("sh", 0) == 0
        and mn.get("sd", 0) == 0
        and mn.get("lw", 0) == 0
        and (mn.get("bnez", 0) + mn.get("bne", 0)) >= 1
        and mn.get("subu", 0) + mn.get("dsubu", 0) >= 1
        and total <= 12
    ):
        return "strlen"

    # memcpy (byte form): lb+sb loop.
    if (
        (mn.get("lb", 0) + mn.get("lbu", 0)) >= 1
        and mn.get("sb", 0) >= 1
        and mn.get("sw", 0) == 0
        and mn.get("lw", 0) == 0
        and has_branch == 1
        and total <= 14
    ):
        return "memcpy_byte"

    # strcpy: lb+sb+bnez(loaded byte) loop, no count arg.
    if (
        (mn.get("lb", 0) + mn.get("lbu", 0)) >= 1
        and mn.get("sb", 0) >= 1
        and mn.get("bnez", 0) >= 1
        and mn.get("sw", 0) == 0
        and total <= 14
    ):
        return "strcpy"

    # strcmp: paired lbu, returns diff.
    if (
        (mn.get("lb", 0) + mn.get("lbu", 0)) >= 2
        and mn.get("sb", 0) == 0
        and mn.get("subu", 0) + mn.get("dsubu", 0) >= 1
        and has_branch >= 1
        and total <= 18
    ):
        return "strcmp"

    return None


# ------------------- Naming logic -------------------


def build_string_name(text: str) -> str:
    """Derive a function-name slug from a referenced string."""
    # Strip trailing newlines, format specifiers, punctuation.
    raw = text.strip()
    # Drop printf format chars at the very end.
    raw = re.sub(r":\s*$", "", raw)
    # If the string has a clear prefix tag like "[FOO]", capture it.
    m = re.match(r"\s*\[(\w+)\]\s*(.*)", raw)
    if m:
        prefix, rest = m.group(1), m.group(2)
        return slugify(f"{prefix.lower()}_{rest}")
    return slugify(raw)


# Per-library banner strings shipped in the SDK (file offset ~0x141300).
SDK_BANNER_RE = re.compile(r"^PsIIlib(\w+?)\s*\d{4}$")


def is_sdk_banner(s: str) -> str | None:
    m = SDK_BANNER_RE.match(s.strip())
    if m:
        return m.group(1)
    return None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--no-strings", action="store_true")
    ap.add_argument("--no-stdlib", action="store_true")
    ap.add_argument("--no-banner", action="store_true")
    ap.add_argument(
        "--min-string-len",
        type=int,
        default=12,
        help="Minimum length of referenced string to use for naming",
    )
    args = ap.parse_args()

    if not BOOT_ELF.exists():
        print(f"ERROR: boot ELF not found at {BOOT_ELF}", file=sys.stderr)
        return 2
    if not ASM_DIR.exists():
        print(f"ERROR: splat asm dir not found at {ASM_DIR}", file=sys.stderr)
        return 2

    # First load everything to discover the auto block boundary, then strip
    # the auto block so a re-run is idempotent (doesn't keep the old names
    # frozen in "existing_addrs" and silently skip them on rescan).
    _, raw_text, _ = load_existing_symbols()
    AUTO_START = "// Auto-added by tools/decomp/name_functions.py"
    stripped = raw_text
    if AUTO_START in raw_text:
        idx = raw_text.find(AUTO_START)
        start = raw_text.rfind("// ===", 0, idx)
        if start != -1:
            stripped = raw_text[:start].rstrip() + "\n"
    if stripped != raw_text:
        SYMBOL_FILE.write_text(stripped)
    existing_addrs, sym_text, existing_names = load_existing_symbols()
    print(f"Loaded {len(existing_addrs)} existing symbol entries.")

    strings = extract_strings(min_len=6)
    str_index = build_string_index(strings)
    print(f"Extracted {len(strings)} ASCII strings (min_len=6) from boot ELF.")

    # Sort to allow nearest-lookup (string covers vram .. vram+len).
    sorted_vrams = sorted(str_index.keys())

    def find_string_at(addr: int) -> str | None:
        """Return the string whose range contains addr, if any."""
        # Binary search.
        import bisect

        idx = bisect.bisect_right(sorted_vrams, addr) - 1
        if idx < 0:
            return None
        base = sorted_vrams[idx]
        s = str_index[base]
        if base <= addr < base + len(s) + 1:
            return s
        return None

    new_names: dict[int, tuple[str, str]] = {}  # vram -> (name, source_comment)

    asm_files = [
        p for p in sorted(ASM_DIR.glob("func_*.s")) if is_code_file(p)
    ]
    print(f"Scanning {len(asm_files)} unnamed function .s files...")

    # ---- Pass: stdlib signatures ----
    stdlib_hits: list[tuple[int, str, str]] = []
    if not args.no_stdlib:
        for f in asm_files:
            name = f.stem
            try:
                vram = int(name.split("_")[-1], 16)
            except ValueError:
                continue
            if vram in existing_addrs:
                continue
            # Only consider SDK region (vram < 0x130000).
            if vram >= 0x130000:
                continue
            cand = detect_stdlib(f)
            if cand:
                stdlib_hits.append((vram, cand, f.stem))
        # De-dup: name the FIRST occurrence per category; suffix others.
        seen_count: dict[str, int] = {}
        for vram, cand, orig in stdlib_hits:
            seen_count[cand] = seen_count.get(cand, 0) + 1
            if seen_count[cand] == 1:
                new_name = cand
            else:
                new_name = f"{cand}_v{seen_count[cand]}"
            if new_name in existing_names:
                new_name = f"{new_name}_{vram:08x}"
            existing_names.add(new_name)
            new_names[vram] = (
                new_name,
                f"signature match ({cand}); was {orig}",
            )
        print(f"  stdlib pass: {len(stdlib_hits)} hits")

    # ---- Pass: SDK library banner ----
    banner_hits: list[tuple[int, str]] = []
    if not args.no_banner:
        # Pre-compute banner vrams.
        banner_addrs: dict[int, str] = {
            v: is_sdk_banner(s) for v, s in str_index.items() if is_sdk_banner(s)
        }
        for f in asm_files:
            name = f.stem
            try:
                vram = int(name.split("_")[-1], 16)
            except ValueError:
                continue
            if vram in existing_addrs or vram in new_names:
                continue
            _, _, targets = parse_asm_file(f)
            for t in targets:
                if t in banner_addrs:
                    lib = banner_addrs[t]
                    cand = f"sce_{lib}_print_banner"
                    suffix = 1
                    new_name = cand
                    while new_name in existing_names:
                        suffix += 1
                        new_name = f"{cand}_{suffix}"
                    existing_names.add(new_name)
                    new_names[vram] = (
                        new_name,
                        f"banner ref '{str_index[t]}'",
                    )
                    banner_hits.append((vram, lib))
                    break
        print(f"  banner pass: {len(banner_hits)} hits")

    # ---- Pass: string-reference naming ----
    string_hits: list[tuple[int, str, str]] = []
    if not args.no_strings:
        # First, gather unique string -> set of referring funcs, so we can
        # prefer strings with exactly one referrer (most distinctive).
        ref_map: dict[str, list[int]] = {}
        func_refs: dict[int, list[str]] = {}
        for f in asm_files:
            name = f.stem
            try:
                vram = int(name.split("_")[-1], 16)
            except ValueError:
                continue
            if vram in existing_addrs or vram in new_names:
                continue
            _, _, targets = parse_asm_file(f)
            seen_strings: set[str] = set()
            for t in targets:
                s = find_string_at(t)
                if s is None or len(s) < args.min_string_len:
                    continue
                # Skip noisy strings.
                if not any(c.isalpha() for c in s):
                    continue
                seen_strings.add(s)
            for s in seen_strings:
                ref_map.setdefault(s, []).append(vram)
                func_refs.setdefault(vram, []).append(s)

        # Drop strings that are referenced by too many functions (banners,
        # punctuation tokens, common format-only strings) — they aren't
        # distinctive enough to name a function after.
        SHARED_THRESHOLD = 6
        for f_vram in list(func_refs.keys()):
            func_refs[f_vram] = [
                s for s in func_refs[f_vram] if len(ref_map[s]) < SHARED_THRESHOLD
            ]
            if not func_refs[f_vram]:
                del func_refs[f_vram]

        # For each function, pick the best string: prefer those referenced by
        # only this function, and the longest such string, and skip strings
        # used as a name already.
        used_string_names: set[str] = set()
        for vram, refs in sorted(func_refs.items()):
            # Skip if already named (stdlib/banner).
            if vram in new_names or vram in existing_addrs:
                continue
            # Rank: unique > multi; among same rank, longest.
            def rank(s: str) -> tuple[int, int]:
                return (-1 if len(ref_map[s]) == 1 else 0, len(s))

            refs_sorted = sorted(refs, key=rank, reverse=True)
            chosen = None
            for s in refs_sorted:
                slug = build_string_name(s)
                if len(slug) < 4:
                    continue
                # Build candidate name.
                cand = f"sub_{slug}"
                # Disambiguate.
                final = cand
                n = 1
                while final in existing_names or final in used_string_names:
                    n += 1
                    final = f"{cand}_{n}"
                chosen = (s, final)
                break
            if chosen is None:
                continue
            s, final = chosen
            used_string_names.add(final)
            existing_names.add(final)
            new_names[vram] = (
                final,
                f"strref: {s[:60]!r}" + ("..." if len(s) > 60 else ""),
            )
            string_hits.append((vram, final, s))
        print(f"  string pass: {len(string_hits)} hits")

    if not new_names:
        print("No new names to add.")
        return 0

    # Emit appended block.
    lines = [""]
    lines.append("// ===========================================================")
    lines.append("// Auto-added by tools/decomp/name_functions.py")
    lines.append("// Sources: stdlib signature match / SDK banner ref / string ref")
    lines.append("// Names are heuristic; rename freely as analysis confirms them.")
    lines.append("// ===========================================================")
    for vram in sorted(new_names):
        name, src = new_names[vram]
        # Source comment goes on its own line so splat's symbol-line parser
        # (which looks for `attr:value` tokens after `//`) doesn't trip on
        # punctuation inside our description.
        lines.append(f"// {src}")
        lines.append(f"{name} = 0x{vram:08X};  // type:func")

    block = "\n".join(lines) + "\n"

    if args.dry_run:
        print(block)
        return 0

    # Auto block was already stripped at the top of main(); just append.
    SYMBOL_FILE.write_text(sym_text + block)
    print(
        f"Added {len(new_names)} names "
        f"(stdlib={len(stdlib_hits)}, banner={len(banner_hits)}, "
        f"strings={len(string_hits)}) to {SYMBOL_FILE.relative_to(REPO_ROOT)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
