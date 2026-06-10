#!/usr/bin/env python3
"""Function call-graph builder for the Extermination decomp.

Reads the LOCAL, git-ignored splat disassembly tree
(build/asm/matchings/main/code/*.s — generated on the user's machine from
their own legal disc copy; never committed) and extracts, per function:

  - vram address and instruction count
  - call edges:  `jal <sym>` plus `j <sym>` tail calls (non-local targets)
  - data references: every `%hi(SYM)` / `%lo(SYM)` symbol

The graph is written as JSON to scratch/callgraph.json (git-ignored). Only
project annotation (names, addresses, edge lists) ever leaves this tool;
no instruction bytes or disassembly text are emitted.

Subcommands
-----------
  build     Scan the .s tree and write the JSON graph.
  verify    Compare per-function distinct-caller counts against
            docs/FUNCTIONS.csv's `callers` column; report mismatches.
  strings   Resolve each function's data refs to NUL-terminated ASCII
            strings in the locally-extracted boot ELF
            (elf/SCUS_971.12.elf, git-ignored) and write
            scratch/func_strings.json.

Runs natively on arm64 macOS (pure Python, no deps).
"""

import argparse
import csv
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_DIR = ROOT / "build" / "asm" / "matchings" / "main" / "code"
CSV_PATH = ROOT / "docs" / "FUNCTIONS.csv"
ELF_PATH = ROOT / "elf" / "SCUS_971.12.elf"
OUT_PATH = ROOT / "scratch" / "callgraph.json"
STRINGS_OUT = ROOT / "scratch" / "func_strings.json"

# Boot ELF layout (pinned in CLAUDE.md): one loadable PROGBITS section,
# file 0x300..0x175E00 -> vram 0x00100000..0x00275B00.
LOAD_VRAM = 0x00100000
LOAD_FOFF = 0x300
LOAD_SIZE = 0x175B00

GLABEL_RE = re.compile(r"^(?:glabel|dlabel)\s+(\S+)")
INSN_RE = re.compile(
    r"^\s*/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+[0-9A-Fa-f]{8}\s*\*/\s+(\S+)\s*(.*)$"
)
CALL_RE = re.compile(r"^(jal|j)\s")
HILO_RE = re.compile(r"%(?:hi|lo)\(([^)+]+)")


def parse_asm_file(path):
    """Parse one splat .s file -> (name, vram, n_insns, callees, datarefs)."""
    name = None
    vram = None
    n = 0
    is_data = False
    callees = set()
    datarefs = set()
    for line in path.read_text(errors="replace").splitlines():
        m = GLABEL_RE.match(line)
        if m and name is None:
            name = m.group(1)
            is_data = line.startswith("dlabel")
            continue
        m = INSN_RE.match(line)
        if not m:
            continue
        insn_vram, mnem, args = int(m.group(1), 16), m.group(2), m.group(3)
        if vram is None:
            vram = insn_vram
        n += 1
        if mnem in ("jal", "j"):
            target = args.split(",")[0].strip()
            if target and not target.startswith("."):
                callees.add(target)
        for sym in HILO_RE.findall(args):
            datarefs.add(sym.strip())
    return name, vram, n, callees, datarefs, is_data


def _generic(name):
    """True for auto-generated names (func_XXXXXXXX / sub_*)."""
    return name.startswith("func_") or name.startswith("sub_")


def build_graph(asm_dir):
    funcs = {}
    by_vram = {}
    for path in sorted(asm_dir.glob("*.s")):
        name, vram, n, callees, datarefs, is_data = parse_asm_file(path)
        if name is None or vram is None:
            continue
        # Renamed functions can leave a stale func_XXXXXXXX.s twin behind;
        # dedup by vram, preferring the human-assigned name.
        old = by_vram.get(vram)
        if old is not None:
            if _generic(name) and not _generic(old):
                continue  # keep existing, better-named node
            del funcs[old]
        by_vram[vram] = name
        funcs[name] = {
            "vram": vram,
            "insns": n,
            "is_data": is_data,
            "callees": sorted(callees),
            "datarefs": sorted(datarefs),
        }
    # Canonicalize call targets that still use an alias of a deduped vram
    # (e.g. old func_ name in one file, new name in another). Aliases share
    # a vram, so map name -> canonical via a second pass over all symbols.
    alias = {}
    for name, info in funcs.items():
        alias[name] = name
        alias[f"func_{info['vram']:08X}"] = name
    for info in funcs.values():
        info["callees"] = sorted({alias.get(c, c) for c in info["callees"]})
    # reverse edges (distinct callers)
    callers = defaultdict(set)
    for name, info in funcs.items():
        for callee in info["callees"]:
            callers[callee].add(name)
    for name, info in funcs.items():
        info["callers"] = sorted(callers.get(name, ()))
    return funcs


def cmd_build(args):
    funcs = build_graph(Path(args.asm_dir))
    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(funcs, indent=0, sort_keys=True))
    n_edges = sum(len(f["callees"]) for f in funcs.values())
    print(f"{len(funcs)} functions, {n_edges} call edges -> {out}")


def load_graph(path):
    return json.loads(Path(path).read_text())


def cmd_verify(args):
    funcs = load_graph(args.graph)
    by_vram = {f["vram"]: (name, f) for name, f in funcs.items()}
    mismatches = 0
    rows = 0
    missing = 0
    with open(args.csv, newline="") as fh:
        for row in csv.DictReader(fh):
            rows += 1
            vram = int(row["vram"], 16)
            ent = by_vram.get(vram)
            if ent is None:
                missing += 1
                print(f"MISSING in graph: {row['vram']} {row['name']}")
                continue
            name, f = ent
            got = len(f["callers"])
            want = int(row["callers"])
            if got != want:
                mismatches += 1
                if mismatches <= args.limit:
                    print(
                        f"caller-count mismatch {row['vram']} {name}: "
                        f"csv={want} graph={got}"
                    )
    print(f"{rows} CSV rows: {missing} missing, {mismatches} caller-count mismatches")


def read_cstr(data, off, maxlen=256):
    end = data.find(b"\0", off, off + maxlen)
    if end <= off:
        return None
    raw = data[off:end]
    try:
        s = raw.decode("ascii")
    except UnicodeDecodeError:
        return None
    if len(s) < 4 or not all(32 <= ord(c) < 127 or c in "\n\r\t" for c in s):
        return None
    return s


def cmd_strings(args):
    funcs = load_graph(args.graph)
    data = Path(args.elf).read_bytes()
    sym_re = re.compile(r"^(?:D|jtbl)_([0-9A-Fa-f]{8})$")
    out = {}
    for name, f in funcs.items():
        hits = []
        for sym in f["datarefs"]:
            m = sym_re.match(sym)
            if not m:
                continue
            addr = int(m.group(1), 16)
            if not (LOAD_VRAM <= addr < LOAD_VRAM + LOAD_SIZE):
                continue
            s = read_cstr(data, LOAD_FOFF + (addr - LOAD_VRAM))
            if s:
                hits.append([f"0x{addr:08X}", s])
        if hits:
            out[name] = hits
    Path(args.out).write_text(json.dumps(out, indent=0, sort_keys=True))
    print(f"{len(out)} functions reference ASCII strings -> {args.out}")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    sub = ap.add_subparsers(dest="cmd", required=True)

    b = sub.add_parser("build", help="scan .s tree, write JSON graph")
    b.add_argument("--asm-dir", default=str(ASM_DIR))
    b.add_argument("--out", default=str(OUT_PATH))
    b.set_defaults(func=cmd_build)

    v = sub.add_parser("verify", help="check FUNCTIONS.csv callers column")
    v.add_argument("--graph", default=str(OUT_PATH))
    v.add_argument("--csv", default=str(CSV_PATH))
    v.add_argument("--limit", type=int, default=40)
    v.set_defaults(func=cmd_verify)

    s = sub.add_parser("strings", help="resolve data refs to ASCII strings")
    s.add_argument("--graph", default=str(OUT_PATH))
    s.add_argument("--elf", default=str(ELF_PATH))
    s.add_argument("--out", default=str(STRINGS_OUT))
    s.set_defaults(func=cmd_strings)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    sys.exit(main())
