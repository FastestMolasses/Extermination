#!/usr/bin/env python3
"""Reconcile the `status` column of docs/FUNCTIONS.csv with the real state.

The status is derived, never hand-edited: from each function's source marker
(src/<name>.c) and from what the linker really selects
(audit_link_provenance.audit) cross-tabulated with a fresh objdiff report.
Only the `status` column changes; vram, name, size, callers and subsystem are
left exactly as they are (port oracle tests read vram and size).

Status values:
  readable      C (no NEARMISS / INCLUDE_ASM marker, no whole-function asm
                body) whose compiled object is what links
                (route compiled_object_*), objdiff 100 %. C functions that
                wrap an inline asm statement (syscall stubs) count here.
  nearmiss      `// NEARMISS` source: body-correct readable C, not byte
                identical; the function links from the original assembly.
                Registry: docs/NEARMISS.md.
  word          whole-function `asm` body (`.word` or instruction form),
                C not recovered (src/readable/ may hold a NEARMISS
                companion). Byte exact unless the function is on
                fill_unmatched's explicit force lists, in which case it links
                from the original assembly and objdiff is below 100 %; the
                check run prints those rows ("word not objdiff 100").
  undecompiled  `// INCLUDE_ASM` stub.
  c_partial     C whose compiled object links (the boot ELF stays byte
                identical) but objdiff scores it below 100 % (e.g. the
                hoisted-%hi symbolization artifact of the expected object).
  c_asm_fallback  C without a NEARMISS marker that links from the original
                assembly (fill_unmatched's explicit size / gprel lists or an
                automatic route) and is not objdiff 100 %: its body has not
                been verified, so it is neither readable nor nearmiss.
  c_local_data  C that objdiff scores 100 % but whose local .rodata could not
                be pinned at one original address (route
                original_assembly_local_data), so it links from the original
                assembly until that data is pinned.
  merged        tail row of a splat split pair merged into its head
                function (no source file of its own; the head's source
                covers it).

Run after a fresh `tools/decomp/build.py build` and the verify_all match
stage (it writes scratch/verify_report.json; the audit refuses a stale one):

  .venv/bin/python tools/decomp/build.py build
  .venv/bin/python tools/verify_all.py --only match
  .venv/bin/python tools/decomp/reconcile_functions_csv.py          # check
  .venv/bin/python tools/decomp/reconcile_functions_csv.py --write  # update

The output is metadata only (names, counts), never original bytes.
"""
from __future__ import annotations

import argparse
from collections import Counter
import csv
import io
from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import audit_link_provenance as provenance  # noqa: E402
import build  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
CSV_PATH = ROOT / "docs" / "FUNCTIONS.csv"
REPORT = ROOT / "scratch" / "verify_report.json"
# A splat split pair merged into one function: tail -> head
# (config/symbol_addrs.txt "merged TAIL"; PROGRESS.md, commits ec5ff20 + 4478ae5).
MERGED_TAILS = {
    "func_00140004": "func_0013F770",
    "func_001C0004": "func_001BFFD0",
    "func_001C2FF0": "func_001C2770",
    "func_00200004": "func_001FFCD0",
    "func_00201000": "func_00200A40",
}
# A top-level function defined with an `asm` body (column 0, optionally
# static). Indented `asm volatile("")` barrier statements inside ordinary C
# (copy_qw4, func_00109F90) are not whole-function bodies.
WHOLE_ASM_BODY = re.compile(r"^(?:static\s+)?asm\s+(?!volatile\b|__volatile__\b)[\w\s\*]*\w\s*\(",
                            re.M)


def classify(name: str, row: dict | None) -> str:
    source = build.SRC / f"{name}.c"
    if not source.exists():
        if name in MERGED_TAILS and (build.SRC / f"{MERGED_TAILS[name]}.c").exists():
            return "merged"
        raise ValueError(f"{name}: no source and not a known merged tail")
    kind = provenance.source_kind(source)
    if kind == "include_asm":
        return "undecompiled"
    if kind == "nearmiss":
        return "nearmiss"
    lexical = re.sub(r"//[^\n]*|/\*.*?\*/", " ", source.read_text(errors="replace"),
                     flags=re.DOTALL)
    if kind == "inline_assembly" and WHOLE_ASM_BODY.search(lexical):
        return "word"
    if row is None:
        raise ValueError(f"{name}: source present but no link row")
    if row["route"].startswith("compiled_object_"):
        return "readable" if row.get("objdiff_perfect") else "c_partial"
    if row.get("objdiff_perfect") and row["route"] == "original_assembly_local_data":
        return "c_local_data"
    if row.get("objdiff_perfect"):
        raise ValueError(f"{name}: objdiff 100 % C linked via {row['route']}")
    return "c_asm_fallback"


def reconcile() -> tuple[list[dict], list[str], dict]:
    if not REPORT.exists():
        raise SystemExit(f"{REPORT} missing: run tools/verify_all.py --only match first")
    audit = provenance.audit(REPORT)
    by_name = {row["name"]: row for row in audit["rows"]}
    text = CSV_PATH.read_text()
    reader = csv.DictReader(io.StringIO(text))
    fields = reader.fieldnames
    rows = list(reader)
    for row in rows:
        row["_old"] = row["status"]
        row["status"] = classify(row["name"], by_name.get(row["name"]))
    csv_names = {row["name"] for row in rows}
    not_in_csv = [name for name in by_name if name not in csv_names]
    return rows, not_in_csv, {"fields": fields, "text": text, "audit": audit}


def render(rows: list[dict], fields: list[str]) -> str:
    out = io.StringIO()
    writer = csv.DictWriter(out, fieldnames=fields, lineterminator="\n",
                            extrasaction="ignore")
    writer.writeheader()
    writer.writerows(rows)
    return out.getvalue()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--write", action="store_true", help="rewrite docs/FUNCTIONS.csv")
    args = parser.parse_args()
    rows, not_in_csv, meta = reconcile()
    new_text = render(rows, meta["fields"])
    transitions = Counter((row["_old"], row["status"]) for row in rows
                          if row["_old"] != row["status"])
    print("status counts:", dict(sorted(Counter(r["status"] for r in rows).items())))
    print("changed rows:", sum(transitions.values()))
    for (old, new), n in sorted(transitions.items()):
        print(f"  {old} -> {new}: {n}")
    audit_rows = {r["name"]: r for r in meta["audit"]["rows"]}
    word_inexact = [r["name"] for r in rows if r["status"] == "word"
                    and not audit_rows.get(r["name"], {}).get("objdiff_perfect")]
    print("word not objdiff 100:", " ".join(word_inexact) or "none")
    for status in ("c_partial", "c_asm_fallback"):
        names = [r["name"] for r in rows if r["status"] == status]
        print(f"{status}:", " ".join(names) or "none")
    print(f"link function slots without a CSV row: {len(not_in_csv)}")
    if args.write:
        if new_text != meta["text"]:
            CSV_PATH.write_text(new_text)
            print(f"wrote {CSV_PATH.relative_to(ROOT)}")
        return 0
    return int(new_text != meta["text"])


if __name__ == "__main__":
    raise SystemExit(main())
