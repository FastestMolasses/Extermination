#!/usr/bin/env python3
"""Read-only audit of which boot function objects the current linker selects.

Unlike an objdiff score, this distinguishes ordinary C from inline assembly
and every fallback used by fill_unmatched. It reads the existing artifacts;
it does not compile, assemble, strip, copy, or link anything. JSON output
contains metadata only, never original instructions or asset bytes.
"""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path
import re
import struct

import build
import fill_unmatched as filler
import link


class Elf:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        if self.data[:6] != b"\x7fELF\x01\x01":
            raise ValueError(f"not an ELF32 little-endian object: {path}")
        offset = struct.unpack_from("<I", self.data, 32)[0]
        stride, count, strings = struct.unpack_from("<HHH", self.data, 46)
        self.headers = [struct.unpack_from("<10I", self.data, offset+i*stride)
                        for i in range(count)]
        names = self.contents(strings)
        self.named = {self.string(names, h[0]): i
                      for i, h in enumerate(self.headers)}

    @staticmethod
    def string(data: bytes, offset: int) -> str:
        return data[offset:data.index(b"\0", offset)].decode("ascii", "replace")

    def contents(self, index: int) -> bytes:
        header = self.headers[index]
        return self.data[header[4]:header[4]+header[5]]

    def section(self, name: str) -> bytes:
        index = self.named.get(name)
        return self.contents(index) if index is not None else b""

    def symbol(self, table: int, index: int) -> str:
        header = self.headers[table]
        offset = header[4] + index*(header[9] or 16)
        name = struct.unpack_from("<I", self.data, offset)[0]
        return self.string(self.contents(header[6]), name)

    def text_relocations(self) -> list[tuple[int, int, str]]:
        text_index = self.named.get(".text")
        entries = []
        for header in self.headers:
            if header[1] != 9 or header[7] != text_index:  # SHT_REL, sh_info
                continue
            for offset in range(header[4], header[4]+header[5], header[9] or 8):
                position, info = struct.unpack_from("<II", self.data, offset)
                entries.append((position, info & 255,
                                self.symbol(header[6], info >> 8)))
        return sorted(entries)


def prepared_text(obj: Elf, slot: int) -> tuple[bytes, list[tuple[int, int, str]]]:
    """Emulate only strip_sections' instruction/relocation transformations.

    Copying preserves instructions except address-named GPREL16 fixups and
    zero-padding to the original function slot. Symbol/section table layout
    and binding changes do not affect this comparison.
    """
    text = bytearray(obj.section(".text"))
    remaining = []
    for position, kind, symbol in obj.text_relocations():
        match = re.fullmatch(r"(?:D_|func_)([0-9A-Fa-f]{4,8})", symbol)
        applied = False
        if kind == 7 and match and position+4 <= len(text):  # R_MIPS_GPREL16
            gp_offset = int(match[1], 16)-0x0027D370
            word = struct.unpack_from("<I", text, position)[0]
            addend = (word & 65535)
            if addend >= 32768:
                addend -= 65536
            final = gp_offset+addend
            if -32768 <= gp_offset <= 32767 and -32768 <= final <= 32767:
                struct.pack_into("<I", text, position,
                                 (word & 0xFFFF0000) | (final & 65535))
                applied = True
        if not applied and (slot <= 0 or position < slot):
            remaining.append((position, kind, symbol))
    if slot > 0:
        text = text[:slot] + bytes(max(0, slot-len(text)))
    return bytes(text), sorted(remaining)


def source_kind(path: Path) -> str:
    if not path.exists():
        return "missing"
    source = path.read_text(errors="replace")
    first = next((line.strip() for line in source.splitlines() if line.strip()), "")
    if first.startswith("// INCLUDE_ASM"):
        return "include_asm"
    if first.startswith("// NEARMISS"):
        return "nearmiss"
    # Remove comments and literal contents before recognizing actual asm
    # tokens. A prose mention of assembly must not misclassify readable C.
    lexical = re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                     " ", source, flags=re.DOTALL)
    if re.search(r"\b(?:asm|__asm|__asm__)\b", lexical):
        return "inline_assembly"
    return "ordinary_c"


def audit(match_report: Path | None = None) -> dict:
    names = link.sorted_functions()
    if names != filler.all_asm_functions():
        raise ValueError("link.py and fill_unmatched.py disagree on function order")
    slots = filler.compute_slot_sizes()
    rows = []
    for name in names:
        source = build.SRC / f"{name}.c"
        source_type = source_kind(source)
        obj_path = filler.OBJ_DIR / f"{name}.o"
        output_path = filler.FILLER_DIR / f"{name}.o"
        obj = Elf(obj_path) if obj_path.exists() else None
        output = Elf(output_path) if output_path.exists() else None
        # This is assemble_one's actual precedence. In particular a NEARMISS
        # cannot be rescued by an old build/obj object or a perfect old score.
        if name in filler.GPREL_FORCE_ASM:
            route = "original_assembly_explicit_gprel"
        elif name in filler.SIZE_DRIFT_FORCE_ASM:
            route = "original_assembly_explicit_size"
        elif source_type in ("include_asm", "nearmiss"):
            route = f"original_assembly_{source_type}"
        elif obj and any(obj.section(s) for s in (".rodata", ".data", ".sdata")):
            route = "original_assembly_local_data"
        elif obj and slots[name] > 0 and len(obj.section(".text")) > slots[name]:
            route = "original_assembly_automatic_size"
        elif obj:
            route = f"compiled_object_{source_type}"
        else:
            route = "original_assembly_missing_object"
        row = {"name": name, "source": source_type, "route": route,
               "slot_bytes": slots[name], "object_exists": obj is not None,
               "filler_exists": output is not None,
               "object_text_bytes": len(obj.section(".text")) if obj else 0,
               "filler_text_bytes": len(output.section(".text")) if output else 0,
               "source_newer_than_object": bool(obj and source.exists() and
                                                source.stat().st_mtime_ns > obj_path.stat().st_mtime_ns)}
        if obj and output and route.startswith("compiled_object_"):
            text, relocations = prepared_text(obj, slots[name])
            row["filler_matches_prepared_object_text"] = text == output.section(".text")
            row["filler_matches_prepared_object_relocations"] = relocations == output.text_relocations()
        rows.append(row)
    counts = Counter(row["route"] for row in rows)
    sizes = Counter()
    for row in rows:
        sizes[row["route"]] += row["slot_bytes"]
    configured = {unit["name"] for unit in json.loads(build.OBJDIFF_JSON.read_text())["units"]}
    units = set(build.units())
    listed = [Path(line.strip()).stem for line in link.OBJ_LIST.read_text().splitlines()
              if line.strip().startswith("build/filler/")]
    result = {
        "scope": "canonical boot function slots; excludes separate data-region and overlay objects",
        "interpretation": "current selection policy plus existing object evidence; not proof of readable-C semantic fidelity",
        "function_count": len(rows), "slot_bytes": sum(slots[name] for name in names),
        "link_object_list_matches_order": listed == names,
        "source_counts_all_src": dict(sorted(Counter(source_kind(p) for p in build.SRC.glob("*.c")).items())),
        "source_counts_link_functions": dict(sorted(Counter(row["source"] for row in rows).items())),
        "build_unit_count": len(units), "objdiff_config_unit_count": len(configured),
        "objdiff_config_only": sorted(configured-units), "build_units_only": sorted(units-configured),
        "route_counts": dict(sorted(counts.items())),
        "route_slot_bytes": dict(sorted(sizes.items())),
        "copied_text_mismatches": [row["name"] for row in rows if row.get("filler_matches_prepared_object_text") is False],
        "copied_relocation_mismatches": [row["name"] for row in rows if row.get("filler_matches_prepared_object_relocations") is False],
        "missing_fillers": [row["name"] for row in rows if not row["filler_exists"]],
        "rows": rows,
    }
    if match_report:
        report = json.loads(match_report.read_text())
        reported = {unit["name"]: unit for unit in report["units"]}
        if set(reported) != units:
            raise ValueError("objdiff report unit list is stale; rerun verify_all.py --only match")
        report_time = match_report.stat().st_mtime_ns
        for name in units:
            for directory in ("obj", "expected"):
                path = build.BUILD / directory / f"{name}.o"
                if path.stat().st_mtime_ns > report_time:
                    raise ValueError(f"objdiff report predates {path}")
        classification = Counter()
        for row in rows:
            unit = reported.get(row["name"])
            if unit is None:
                continue
            measures = unit["measures"]
            perfect = (measures.get("matched_functions", 0) ==
                       measures.get("total_functions", 0) > 0)
            row["objdiff_perfect"] = perfect
            row["objdiff_fuzzy_match_percent"] = measures.get("fuzzy_match_percent", 0)
            row["objdiff_code_bytes"] = int(measures.get("total_code", 0))
            classification[f"{row['route']}_{'perfect' if perfect else 'partial'}"] += 1
        result["objdiff_report"] = str(match_report)
        result["objdiff_route_counts"] = dict(sorted(classification.items()))
        result["copied_ordinary_c_nonperfect"] = [
            row["name"] for row in rows if row["route"] == "compiled_object_ordinary_c"
            and row.get("objdiff_perfect") is False]
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, help="write metadata JSON (prefer build/)")
    parser.add_argument("--match-report", type=Path,
                        help="optionally cross-tabulate a fresh verify_all objdiff report")
    args = parser.parse_args()
    result = audit(args.match_report)
    if args.output:
        args.output.write_text(json.dumps(result, indent=2)+"\n")
    print(json.dumps({key: value for key, value in result.items() if key != "rows"}, indent=2))
    return int(bool(result["copied_text_mismatches"] or result["copied_relocation_mismatches"] or
                    result["missing_fillers"] or not result["link_object_list_matches_order"]))


if __name__ == "__main__":
    raise SystemExit(main())
