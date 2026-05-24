#!/usr/bin/env python3
"""
name_vtables_overlays.py — second-pass heuristic naming for the Extermination
matching decomp. Sibling of name_functions.py; uses structural patterns rather
than string references.

Three passes:

  1. Overlay-dispatch naming. func_001E7780 is the area-state dispatcher: it
     reads a two-byte ID at D_00810700/D_00810701 and jal's to one of 17
     unique vrams in the overlay arena (0x823500..). We parse the
     `addiu $a0, $zero, K; beq $v1, $a0, .Lxxx; ...; .Lxxx: jal func_XXXXXX`
     structure and map each overlay entry-point vram to a name like
     `area_state_<HEX>_dispatch_entry`. The dominant area-state ID(s) for each
     vram inform the name. Both the boot-ELF references (added to
     config/symbol_addrs.txt) and the per-overlay symbol_addrs files are
     updated so splat picks up the names everywhere.

  2. Vtable-init scanning. Walks every per-function .s file looking for
     clusters of 3+ consecutive `sw $REG, +N($BASE)` where each $REG is
     loaded from a `lui+addiu %hi/%lo(SYM)` pair naming a function (or via
     a splat-resolved `D_00XXXXXX` symbol that is a function vram). Each
     such cluster is a vtable init: we name the slot-target functions
     `vtable_<BASE_TAG>_off<N>` and the host function (which contains the
     init) `init_vtable_<BASE_TAG>`. Conservative: skip if any of the
     stored targets is already named with a real (non-`sub_` / non-`func_`)
     name.

  3. Idempotent: writes its block at the end of config/symbol_addrs.txt,
     auto-stripping the previous block on re-run.

Run:
    python3 tools/decomp/name_vtables_overlays.py [--dry-run]
"""

from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SYMBOL_FILE = REPO_ROOT / "config" / "symbol_addrs.txt"
ASM_DIR = REPO_ROOT / "build" / "asm" / "matchings" / "main" / "code"
OVERLAY_DIR = REPO_ROOT / "config" / "overlays"

DISPATCHER_FILE = ASM_DIR / "func_001E7780.s"

AUTO_BLOCK_TAG = "// Auto-added by tools/decomp/name_vtables_overlays.py"

# ----- Symbol file I/O ------------------------------------------------------

NAME_RE = re.compile(r"\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;")


def load_symbol_file(path: Path) -> tuple[dict[int, str], set[str], str]:
    """Return (addr->name, name_set, raw_text)."""
    addrs: dict[int, str] = {}
    names: set[str] = set()
    if not path.exists():
        return addrs, names, ""
    text = path.read_text()
    for line in text.splitlines():
        m = NAME_RE.match(line)
        if not m:
            continue
        name, addr = m.group(1), int(m.group(2), 0)
        addrs[addr] = name
        names.add(name)
    return addrs, names, text


def strip_auto_block(text: str) -> str:
    if AUTO_BLOCK_TAG not in text:
        return text
    idx = text.find(AUTO_BLOCK_TAG)
    start = text.rfind("// ===", 0, idx)
    if start == -1:
        return text
    return text[:start].rstrip() + "\n"


# ----- Overlay-dispatch ------------------------------------------------------

ADDIU_A0_RE = re.compile(
    r"addiu\s+\$a0,\s*\$zero,\s*(0x[0-9A-Fa-f]+|-?\d+)"
)
BEQ_LABEL_RE = re.compile(
    r"beq\s+\$v1,\s*\$a0,\s*\.(L[0-9A-Fa-f]+)"
)
LABEL_RE = re.compile(r"^\s*\.(L[0-9A-Fa-f]+):")
JAL_RE = re.compile(r"jal\s+func_([0-9A-Fa-f]+)")
BEQZ_LABEL_RE = re.compile(r"beqz\s+\$v1,\s*\.(L[0-9A-Fa-f]+)")


def parse_dispatch_table() -> list[tuple[int, int]]:
    """Parse func_001E7780. Return list of (area_state_id, overlay_vram)."""
    if not DISPATCHER_FILE.exists():
        return []
    lines = DISPATCHER_FILE.read_text().splitlines()
    # Strip the /* ... */ prefix to ease parsing.
    body = []
    for ln in lines:
        body.append(re.sub(r"^\s*/\*[^*]*\*/\s*", "", ln).strip())

    # First scan top: build (id -> label) and (label -> jal_target).
    pending_id: int | None = None
    pending_beq_targets: list[tuple[int, str]] = []
    label_to_jal: dict[str, int] = {}

    for ln in body:
        m = ADDIU_A0_RE.search(ln)
        if m:
            pending_id = int(m.group(1), 0)
            continue
        m = BEQ_LABEL_RE.search(ln)
        if m and pending_id is not None:
            pending_beq_targets.append((pending_id, m.group(1)))
            pending_id = None
            continue
        # beqz $v1 path (id=0 case)
        m = BEQZ_LABEL_RE.search(ln)
        if m:
            pending_beq_targets.append((0, m.group(1)))
            continue

    # Now walk again and capture label -> first jal target after.
    cur_label: str | None = None
    for ln in body:
        lm = LABEL_RE.match(ln)
        if lm:
            cur_label = lm.group(1)
            continue
        jm = JAL_RE.search(ln)
        if jm and cur_label is not None and cur_label not in label_to_jal:
            label_to_jal[cur_label] = int(jm.group(1), 16)
            cur_label = None  # only first jal per label region

    out: list[tuple[int, int]] = []
    for aid, lbl in pending_beq_targets:
        if lbl in label_to_jal:
            out.append((aid, label_to_jal[lbl]))
    return out


def overlay_dispatch_names(
    dispatch: list[tuple[int, int]],
) -> dict[int, tuple[str, str]]:
    """Map overlay vram -> (name, comment).  Uses the lowest area-state ID
    seen as the canonical tag, and notes total caller count.
    """
    by_vram: dict[int, list[int]] = defaultdict(list)
    for aid, vram in dispatch:
        by_vram[vram].append(aid)
    names: dict[int, tuple[str, str]] = {}
    for vram, ids in by_vram.items():
        ids_sorted = sorted(set(ids))
        primary = ids_sorted[0]
        # Build a descriptive name.  Use the offset from 0x823500 as the
        # canonical role slot, and tag with the primary area-state hex.
        offset = vram - 0x00823500
        name = f"area_dispatch_off{offset:04X}_state{primary:04X}"
        ids_str = ",".join(f"0x{i:X}" for i in ids_sorted)
        comment = f"overlay-dispatch: vram 0x{vram:08X} (+0x{offset:X}), {len(ids)} caller(s), area-state IDs [{ids_str}]"
        names[vram] = (name, comment)
    return names


# ----- Vtable scan -----------------------------------------------------------

# Patterns within a per-function .s (lines have /* off vram word */ stripped):
#   lui   $rA, %hi(SYM_or_D)
#   addiu $rA, $rA, %lo(SYM_or_D)        # rA now holds function pointer
#   sw    $rA, OFF($rBASE)
# Multiple of these in a row, with increasing OFF and same rBASE, == vtable.

LUI_RE = re.compile(r"lui\s+\$(\w+),\s*%hi\(([A-Za-z_][\w]*)\)")
ADDIU_LO_RE = re.compile(
    r"addiu\s+\$(\w+),\s*\$(\w+),\s*%lo\(([A-Za-z_][\w]*)\)"
)
SW_RE = re.compile(
    r"sw\s+\$(\w+),\s*(-?0x[0-9A-Fa-f]+|-?\d+)\(\$(\w+)\)"
)


def strip_prefix(line: str) -> str:
    return re.sub(r"^\s*/\*[^*]*\*/\s*", "", line).strip()


def parse_int(s: str) -> int:
    s = s.strip()
    return int(s, 16) if s.startswith(("0x", "-0x", "+0x")) else int(s)


@dataclass
class VTHit:
    host_func_vram: int   # function performing the init
    base_reg: str         # register holding struct base ptr
    base_tag: str         # symbol or hex tag for the base
    slots: list[tuple[int, int]]  # list of (offset, func_vram)


def func_vram_set() -> set[int]:
    out: set[int] = set()
    for p in ASM_DIR.glob("func_*.s"):
        try:
            out.add(int(p.stem.split("_")[-1], 16))
        except ValueError:
            pass
    return out


def scan_vtable_inits(known_funcs: set[int]) -> list[VTHit]:
    hits: list[VTHit] = []
    files = sorted(ASM_DIR.glob("func_*.s"))
    for f in files:
        try:
            host_vram = int(f.stem.split("_")[-1], 16)
        except ValueError:
            continue
        text = f.read_text(errors="replace")
        if "sw " not in text:
            continue

        # Track recent lui values per-register (clear on register overwrite).
        # We treat each line in order.
        reg_func: dict[str, int] = {}   # reg -> resolved function vram (after lui+addiu)
        pending_lui: dict[str, str] = {}  # reg -> symbol name from lui

        # Active vtable cluster.
        cluster_base: str | None = None
        cluster_slots: list[tuple[int, int]] = []

        def flush():
            nonlocal cluster_base, cluster_slots
            if cluster_base and len(cluster_slots) >= 3:
                hits.append(VTHit(
                    host_func_vram=host_vram,
                    base_reg="",
                    base_tag=cluster_base,
                    slots=list(cluster_slots),
                ))
            cluster_base = None
            cluster_slots = []

        for raw in text.splitlines():
            body = strip_prefix(raw)
            if not body:
                continue
            # Skip directives, labels, comments.
            if body.startswith((".", "glabel", "endlabel", "nonmatching", "//")):
                # A label terminates a cluster (control flow change).
                if body.startswith(".") and body.endswith(":"):
                    flush()
                    reg_func.clear()
                    pending_lui.clear()
                continue

            # Branches/jumps terminate a cluster (different basic block).
            mn = body.split()[0] if body else ""
            if mn.startswith(("b", "j")) and mn not in ("bnez", "beqz", "blez", "bgez"):
                # Conservative: treat any 'b' or 'j' as block terminator.
                # But 'beqz'/'beq'/etc. also do. Just flush.
                pass
            if mn in ("b", "j", "jr", "jal", "jalr", "bne", "beq", "bnez",
                      "beqz", "blez", "bgez", "bgtz", "bltz", "bnel", "beql",
                      "bgezal", "bltzal"):
                flush()
                # Don't clear reg_func — function pointer may be still live in delay slot context.
                continue

            m = LUI_RE.search(body)
            if m:
                reg, sym = m.group(1), m.group(2)
                pending_lui[reg] = sym
                reg_func.pop(reg, None)
                continue

            m = ADDIU_LO_RE.search(body)
            if m:
                rt, rs, sym = m.group(1), m.group(2), m.group(3)
                if rs in pending_lui and pending_lui[rs] == sym:
                    # Resolved address; if sym matches func_XXXXXX or
                    # D_00XXXXXX where XXXXXX is a known function vram,
                    # treat it as a function pointer.
                    fvram = _resolve_func_sym(sym, known_funcs)
                    if fvram is not None:
                        reg_func[rt] = fvram
                    else:
                        reg_func.pop(rt, None)
                # consume the lui mapping for src reg only if rs != rt
                if rs != rt:
                    pending_lui.pop(rs, None)
                pending_lui.pop(rt, None)
                continue

            m = SW_RE.search(body)
            if m:
                src, off_s, base = m.group(1), m.group(2), m.group(3)
                off = parse_int(off_s)
                if src in reg_func:
                    fvram = reg_func[src]
                    if cluster_base is None:
                        cluster_base = base
                        cluster_slots = [(off, fvram)]
                    elif base == cluster_base and (
                        not cluster_slots or off > cluster_slots[-1][0]
                    ):
                        cluster_slots.append((off, fvram))
                    else:
                        flush()
                        cluster_base = base
                        cluster_slots = [(off, fvram)]
                    # The store consumes the source's function-pointer
                    # liveness for the next slot's lui+addiu sequence.
                    reg_func.pop(src, None)
                else:
                    # sw of something that isn't a known func ptr —
                    # don't break the cluster on unrelated stores.
                    pass
                continue

            # Any other instruction that writes to a register may invalidate
            # a tracked function pointer.  Conservative: look for `, $X` as
            # dest.  But this is fiddly; we just leave reg_func alone and
            # rely on lui+addiu+sw being adjacent in vtable init code (which
            # they generally are with mwcc).

        flush()
    return hits


D_HEX_RE = re.compile(r"^D_([0-9A-Fa-f]{6,8})$")
FUNC_HEX_RE = re.compile(r"^func_([0-9A-Fa-f]{6,8})$")


def _resolve_func_sym(sym: str, known_funcs: set[int]) -> int | None:
    m = FUNC_HEX_RE.match(sym)
    if m:
        v = int(m.group(1), 16)
        return v if v in known_funcs else None
    m = D_HEX_RE.match(sym)
    if m:
        v = int(m.group(1), 16)
        return v if v in known_funcs else None
    return None


# ----- Main ------------------------------------------------------------------


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--no-overlay", action="store_true")
    ap.add_argument("--no-vtable", action="store_true")
    args = ap.parse_args()

    if not ASM_DIR.exists():
        print(f"ERROR: splat asm dir missing: {ASM_DIR}", file=sys.stderr)
        return 2

    # Strip prior auto block before reading existing-symbol state.
    _, _, raw = load_symbol_file(SYMBOL_FILE)
    stripped = strip_auto_block(raw)
    if stripped != raw and not args.dry_run:
        SYMBOL_FILE.write_text(stripped)
    existing_addrs, existing_names, sym_text = load_symbol_file(SYMBOL_FILE)
    print(f"Loaded {len(existing_addrs)} existing symbol entries.")

    new_main: dict[int, tuple[str, str]] = {}
    new_overlay_per_area: dict[str, dict[int, tuple[str, str]]] = defaultdict(dict)

    # --------- Overlay-dispatch pass ---------
    overlay_count = 0
    if not args.no_overlay:
        dispatch = parse_dispatch_table()
        print(f"  overlay: parsed {len(dispatch)} dispatch entries from func_001E7780")
        names = overlay_dispatch_names(dispatch)
        # Sanity: known callable set from docs/OVERLAYS.md section 3 has 17.
        unique_vrams = sorted(names.keys())
        print(f"  overlay: {len(unique_vrams)} unique overlay vrams")
        # Determine which overlay file(s) contain each target vram. Every
        # overlay loads at 0x823500, so all targets are in the overlay
        # arena. We update every overlay's symbol_addrs file: the target
        # function exists in any overlay that provides that slot.
        # Build per-overlay function-vram set from splat configs.
        ov_funcs = _load_overlay_func_vrams()
        for vram, (name, comment) in names.items():
            if vram in existing_addrs:
                # Don't overwrite an existing main-config name.
                continue
            new_main[vram] = (name, comment)
            overlay_count += 1
            # Propagate to each overlay that actually has a function at this vram.
            for area, funcs in ov_funcs.items():
                if vram in funcs:
                    # Symbol form in overlays is `overlay_<AREA>_<name>`.
                    ov_name = f"overlay_{area}_{name}"
                    sym_path = OVERLAY_DIR / f"{area}_symbol_addrs.txt"
                    ov_addrs, ov_names_set, _ = load_symbol_file(sym_path)
                    # Skip if the user already gave it a non-default name.
                    cur = ov_addrs.get(vram)
                    if cur and not cur.startswith(f"overlay_{area}_func_"):
                        continue
                    new_overlay_per_area[area][vram] = (
                        ov_name, comment,
                    )

    # --------- Vtable pass ---------
    vtable_count = 0
    method_count = 0
    if not args.no_vtable:
        known = func_vram_set()
        hits = scan_vtable_inits(known)
        # Filter: at least 3 slots; all slot targets must be in known_funcs.
        good: list[VTHit] = []
        for h in hits:
            if len(h.slots) >= 3 and all(v in known for _, v in h.slots):
                # Drop if all slot targets already named with real names.
                all_named = all(
                    v in existing_addrs and not existing_addrs[v].startswith(
                        ("func_", "sub_")
                    )
                    for _, v in h.slots
                )
                if all_named:
                    continue
                good.append(h)
        print(f"  vtable: {len(good)} candidate vtable inits found (>=3 slots)")
        # Tag uniqueness: by base_tag. Multiple inits may target same struct;
        # disambiguate by host func vram.
        tag_seen: dict[str, int] = defaultdict(int)
        for h in good:
            tag_seen[h.base_tag] += 1
            suffix = ""
            if tag_seen[h.base_tag] > 1:
                suffix = f"_v{tag_seen[h.base_tag]}"
            base_label = h.base_tag.replace("D_", "")
            # If base is a register, the vtable is constructed on an argument
            # or local — disambiguate by host func vram so the name is unique.
            if not base_label.startswith(("00", "10", "20", "30", "40", "50", "60", "70", "80", "90")):
                base_label = f"{base_label}_at_{h.host_func_vram:08X}"
            init_name = f"init_vtable_{base_label}{suffix}"
            # Don't overwrite existing host-func name.
            if h.host_func_vram not in existing_addrs and h.host_func_vram not in new_main:
                if init_name not in existing_names:
                    new_main[h.host_func_vram] = (
                        init_name,
                        f"vtable-init: {len(h.slots)} slots into {h.base_tag}, base reg ${h.base_reg or '?'}",
                    )
                    existing_names.add(init_name)
                    vtable_count += 1
            # Slot targets.
            for off, fvram in h.slots:
                if fvram in existing_addrs or fvram in new_main:
                    continue
                slot_name = f"vtable_{base_label}{suffix}_off{off:X}"
                # Disambiguate
                n = 1
                cand = slot_name
                while cand in existing_names:
                    n += 1
                    cand = f"{slot_name}_{n}"
                new_main[fvram] = (
                    cand,
                    f"vtable slot: init at func_{h.host_func_vram:08X}, base {h.base_tag}+0x{off:X}",
                )
                existing_names.add(cand)
                method_count += 1

    if not new_main and not new_overlay_per_area:
        print("No new names to add.")
        return 0

    # Emit to symbol_addrs.txt
    if new_main:
        lines = ["", "// ===========================================================",
                 AUTO_BLOCK_TAG,
                 "// Pass 1: overlay-dispatch (func_001E7780 callers).",
                 "// Pass 2: vtable-init scan (3+ consecutive sw of func ptrs).",
                 "// Names are heuristic; rename freely as analysis confirms them.",
                 "// ==========================================================="]
        for vram in sorted(new_main):
            name, src = new_main[vram]
            lines.append(f"// {src}")
            lines.append(f"{name} = 0x{vram:08X};  // type:func")
        block = "\n".join(lines) + "\n"
        if args.dry_run:
            print("--- main symbol_addrs.txt block ---")
            print(block)
        else:
            SYMBOL_FILE.write_text(sym_text + block)

    # Per-overlay files.
    overlay_method_count = 0
    for area, entries in new_overlay_per_area.items():
        sym_path = OVERLAY_DIR / f"{area}_symbol_addrs.txt"
        _, _, raw = load_symbol_file(sym_path)
        stripped = strip_auto_block(raw)
        if stripped != raw and not args.dry_run:
            sym_path.write_text(stripped)
            raw = stripped
        elif stripped != raw:
            raw = stripped
        lines = ["", "// ===========================================================",
                 AUTO_BLOCK_TAG,
                 "// Overlay-dispatch targets resolved from boot ELF func_001E7780.",
                 "// ==========================================================="]
        for vram in sorted(entries):
            name, src = entries[vram]
            lines.append(f"// {src}")
            lines.append(f"{name} = 0x{vram:08X};  // type:func")
            overlay_method_count += 1
        block = "\n".join(lines) + "\n"
        if args.dry_run:
            print(f"--- {sym_path.name} block ---")
            print(block)
        else:
            sym_path.write_text(raw + block)

    print(
        f"Added: overlay-dispatch={overlay_count} (in main config), "
        f"overlay per-area entries={overlay_method_count}, "
        f"vtable init funcs={vtable_count}, vtable methods={method_count}"
    )
    return 0


def _load_overlay_func_vrams() -> dict[str, set[int]]:
    """Read each AREAxx_symbol_addrs.txt to get the set of function vrams
    splat has emitted for that overlay."""
    out: dict[str, set[int]] = {}
    if not OVERLAY_DIR.exists():
        return out
    for p in sorted(OVERLAY_DIR.glob("AREA*_symbol_addrs.txt")):
        area = p.stem.replace("_symbol_addrs", "")
        addrs, _, _ = load_symbol_file(p)
        out[area] = set(addrs.keys())
    # Also fold in the per-overlay splat asm-output function set.
    for area_asm_dir in sorted(REPO_ROOT.glob("build/overlays/AREA*/asm/matchings/AREA*/code")):
        # path .../build/overlays/AREAxx/asm/matchings/AREAxx/code
        # take the AREAxx after build/overlays
        for part in area_asm_dir.parts:
            if re.fullmatch(r"AREA\d+", part):
                area = part
                break
        else:
            continue
        s = out.setdefault(area, set())
        for f in area_asm_dir.glob("*.s"):
            # Stem like "overlay_AREA07_func_00823580" or "func_00823580"
            stem = f.stem
            m = re.search(r"_([0-9A-Fa-f]{6,8})$", stem)
            if m:
                s.add(int(m.group(1), 16))
    return out


if __name__ == "__main__":
    raise SystemExit(main())
