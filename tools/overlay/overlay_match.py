#!/usr/bin/env python3
"""tools/overlay/overlay_match.py -- per-function matching harness for the
runtime overlays (host side; compiles in the exterm-toolchain container).

Why this exists
---------------
The overlay pipeline (tools/overlay/) links each splat piece of an overlay and
checks the whole .BIN. It has no per-function objdiff units, and splat splits
many overlay functions in two: every overlay is linked at 0x00823500 although
its code really starts 0x40 later (the MWo3 header is loaded first), so each
intra-overlay call target lands 0x40 into the called function and splat opens
a new "function" there. This harness regroups the splat pieces into the real
functions, builds a relocation-free expected object per function from the
user's own extracted overlay (never committed), compiles a candidate C file
with the requested CodeWarrior build, resolves the candidate's relocations to
absolute addresses the same way the overlay link does, and scores it with
objdiff-cli.

Naming: functions keep their splat/link names (func_overlay_AREAxx_<link
vram>). The runtime address of the code is link vram + 0x40; `list` prints
both.

Usage (repo root, host python with rabbitizer):
  tools/overlay/overlay_match.py list  AREA01
  tools/overlay/overlay_match.py prep  AREA01            # merged .s + expected objects
  tools/overlay/overlay_match.py check AREA01 C [C...] [--show]
      C = path to a .c whose stem is the function's link name. The compiler
      comes from its '// COMPILER:' line (default mwccmips 991202), flags from
      '// CFLAGS:' (default '-O4,p -sdatathreshold 0').

Scratch lives in build/ovlmatch/<AREA>/ (git-ignored).
"""
from __future__ import annotations

import argparse
import json
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ARENA = 0x00823500
HDR = 0x40
GP = 0x0027D370
IMAGE = "exterm-toolchain"
MWCC = {
    "mwcc233": "tools/mwccps2-233/mwccps2.exe",
    "mwcc24": "tools/mwccps2-24/mwccps2.exe",
    "mwcc30": "tools/mwccps2-30/mwccps2.exe",
    "mwcc301": "tools/mwccps2-301/mwccps2.exe",
}
DEFAULT_EXE = "tools/mwccps2/mwccmips.exe"
DEFAULT_FLAGS = "-O4,p -sdatathreshold 0"


# --------------------------------------------------------------------- pieces

def area_paths(area: str) -> dict:
    b = ROOT / "build" / "overlays" / area
    return {
        "asm": b / "asm" / "matchings" / area / "code",
        "bin": ROOT / "extract" / "OVERLAY" / f"{area}.BIN",
        "work": ROOT / "build" / "ovlmatch" / area,
        "src": ROOT / "src" / "overlays" / area,
    }


def piece_vram(path: Path) -> int:
    m = re.search(r"([0-9A-Fa-f]{8})$", path.stem)
    return int(m.group(1), 16)


def load_bin(area: str) -> tuple[bytes, int, int]:
    data = area_paths(area)["bin"].read_bytes()
    text_size = struct.unpack_from("<I", data, 0x0C)[0]
    return data, ARENA, ARENA + text_size


def word_at(data: bytes, vram: int) -> int:
    return struct.unpack_from("<I", data, HDR + vram - ARENA)[0]


def is_return_or_jump(w: int) -> bool:
    op = w >> 26
    if op == 0 and (w & 0x3F) == 0x08:  # jr
        return True
    return op == 0x02  # j


def true_functions(area: str) -> list[dict]:
    """Group splat pieces into real functions.

    A piece starts a real function when it is the first piece, when it opens
    with a stack-frame allocation, or when the words before it are a return
    or jump, its delay slot, and only zero padding. Everything else is a
    fake split (an intra-overlay call target 0x40 into a real function)."""
    p = area_paths(area)
    data, text_lo, text_hi = load_bin(area)
    pieces = sorted((piece_vram(f), f.stem) for f in p["asm"].glob("*.s"))
    funcs: list[dict] = []
    for i, (v, name) in enumerate(pieces):
        w = word_at(data, v)
        start = i == 0 or (w >> 16) == 0x27BD and (w & 0x8000)
        if not start:
            k = v - 4
            while k >= text_lo and word_at(data, k) == 0:
                k -= 4
            # k is the last nonzero word before v; it can be the delay slot
            # (then k-4 is the jump) or itself the jump with a nop delay slot
            if k - 4 >= text_lo and is_return_or_jump(word_at(data, k - 4)):
                start = True
            elif is_return_or_jump(word_at(data, k)) and k + 4 < v:
                start = True
        if start:
            funcs.append({"name": name, "vram": v, "pieces": [name]})
        else:
            funcs[-1]["pieces"].append(name)
    for i, f in enumerate(funcs):
        end = funcs[i + 1]["vram"] if i + 1 < len(funcs) else text_hi
        f["slot_end"] = end
        # function body end: drop trailing zero padding, keep a delay-slot nop
        k = end - 4
        while k > f["vram"] and word_at(data, k) == 0:
            k -= 4
        if k - 4 >= f["vram"] and word_at(data, k) == 0 and False:
            pass
        body_end = k + 4
        if is_return_or_jump(word_at(data, k)):
            body_end = k + 8  # jump whose delay slot is a nop
        f["end"] = min(body_end, end)
        f["size"] = f["end"] - f["vram"]
        f["runtime"] = f["vram"] + HDR
    return funcs


def c_status(area: str, name: str) -> str:
    src = area_paths(area)["src"] / f"{name}.c"
    if not src.exists():
        return "asm"
    head = src.read_text(errors="replace")
    if head.lstrip().startswith("// NEARMISS"):
        return "nearmiss"
    if re.search(r"^\s*asm\s+\w[\w\s\*]*\(", head, re.M):
        return "asm-void"
    return "C"


# ----------------------------------------------------------------- ELF bits

def build_elf(text: bytes, sym: str) -> bytes:
    """Minimal ELF32 LE MIPS relocatable: .text + one global function symbol."""
    shstr = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    strtab = b"\0" + sym.encode() + b"\0"
    sym0 = b"\0" * 16
    sym1 = struct.pack("<IIIBBH", 1, 0, len(text), 0x12, 0, 1)
    symtab = sym0 + sym1
    off = 0x34
    text_off = off
    off += len(text)
    off = (off + 3) & ~3
    symtab_off = off
    off += len(symtab)
    strtab_off = off
    off += len(strtab)
    shstr_off = off
    off += len(shstr)
    off = (off + 3) & ~3
    shoff = off

    def sh(name, typ, flags, offset, size, link=0, info=0, align=1, entsize=0):
        return struct.pack("<IIIIIIIIII", name, typ, flags, 0, offset, size,
                           link, info, align, entsize)

    shdrs = (sh(0, 0, 0, 0, 0) +
             sh(1, 1, 6, text_off, len(text), align=16) +
             sh(7, 2, 0, symtab_off, len(symtab), link=3, info=1, align=4,
                entsize=16) +
             sh(15, 3, 0, strtab_off, len(strtab)) +
             sh(23, 3, 0, shstr_off, len(shstr)))
    ehdr = (b"\x7fELF\x01\x01\x01" + b"\0" * 9 +
            struct.pack("<HHIIIIIHHHHHH", 1, 8, 1, 0, 0, shoff, 0x20921101,
                        0x34, 0, 0, 40, 5, 4))
    out = bytearray(ehdr)
    out += text
    out += b"\0" * (symtab_off - len(out))
    out += symtab + strtab + shstr
    out += b"\0" * (shoff - len(out))
    out += shdrs
    return bytes(out)


_ADDR_SYM = re.compile(
    r"^(?:D|func|jtbl|L)_(?:overlay_[A-Z0-9]+_)?([0-9A-Fa-f]{6,8})$")


def resolve_object(obj: Path, func: str, vram: int) -> tuple[bytes, list[str]]:
    """Apply the candidate's relocations with the function placed at `vram`.

    Address-encoded names resolve to their address (as link_overlay.py does);
    GPREL16 uses $gp = 0x0027D370. A local .rodata section is placed at the
    address its first HI16/LO16 pair implies from the ORIGINAL bytes (so a
    jump table can be checked); its bytes are returned in the notes."""
    from elftools.elf.elffile import ELFFile
    from elftools.elf.relocation import RelocationSection
    notes: list[str] = []
    with obj.open("rb") as fh:
        elf = ELFFile(fh)
        text_sec = elf.get_section_by_name(".text")
        text = bytearray(text_sec.data())
        symtab = elf.get_section_by_name(".symtab")
        fsym = None
        for s in symtab.iter_symbols():
            if s.name == func:
                fsym = s
        if fsym is None:
            raise SystemExit(f"{obj}: no symbol {func}")
        fstart = fsym["st_value"]
        fsize = fsym["st_size"] or len(text) - fstart
        sec_names = {i: sec.name for i, sec in enumerate(elf.iter_sections())}
        rel = None
        for sec in elf.iter_sections():
            if isinstance(sec, RelocationSection) and \
                    sec_names.get(sec["sh_info"]) == ".text":
                rel = sec
        extra = [sec.name for sec in elf.iter_sections()
                 if sec.name in (".rodata", ".data", ".sdata", ".bss", ".sbss")
                 and sec["sh_size"]]
        if extra:
            notes.append("sections:" + ",".join(extra))
        rodata = elf.get_section_by_name(".rodata")
        rodata_base = None

        def sym_addr(s) -> int | None:
            if s["st_shndx"] == "SHN_UNDEF":
                m = _ADDR_SYM.match(s.name)
                return int(m.group(1), 16) if m else None
            secname = sec_names.get(s["st_shndx"])
            if secname == ".text":
                return vram - fstart + s["st_value"]
            if secname == ".rodata":
                return None if rodata_base is None else rodata_base + s["st_value"]
            return None

        relocs = []
        if rel is not None:
            for r in rel.iter_relocations():
                relocs.append((r["r_offset"], r["r_info_type"],
                               symtab.get_symbol(r["r_info_sym"])))
        pending_hi = []
        for off, typ, s in relocs:
            w = struct.unpack_from("<I", text, off)[0]
            if s.name == "" or sec_names.get(s["st_shndx"]) == ".rodata":
                if rodata_base is None and typ == 5 and rodata is not None:
                    notes.append("rodata-needs-pin")
            S = sym_addr(s)
            if S is None:
                notes.append(f"unresolved {s.name or sec_names.get(s['st_shndx'])} type {typ}")
                continue
            if typ == 4:  # R_MIPS_26
                a = (w & 0x3FFFFFF) << 2
                t = (S + a) >> 2
                w = (w & 0xFC000000) | (t & 0x3FFFFFF)
            elif typ == 5:  # HI16
                pending_hi.append((off, S))
                continue
            elif typ == 6:  # LO16
                lo = w & 0xFFFF
                lo = lo - 0x10000 if lo & 0x8000 else lo
                for hoff, hS in pending_hi:
                    hw = struct.unpack_from("<I", text, hoff)[0]
                    ahl = ((hw & 0xFFFF) << 16) + lo
                    val = hS + ahl
                    hw = (hw & 0xFFFF0000) | (((val + 0x8000) >> 16) & 0xFFFF)
                    struct.pack_into("<I", text, hoff, hw)
                pending_hi = []
                val = S + lo
                w = (w & 0xFFFF0000) | (val & 0xFFFF)
            elif typ == 7:  # GPREL16
                a = w & 0xFFFF
                a = a - 0x10000 if a & 0x8000 else a
                val = S + a - GP
                w = (w & 0xFFFF0000) | (val & 0xFFFF)
            elif typ == 2:  # R_MIPS_32
                w = (w + S) & 0xFFFFFFFF
            else:
                notes.append(f"reloc type {typ} unhandled")
                continue
            struct.pack_into("<I", text, off, w)
        return bytes(text[fstart:fstart + fsize]), notes


# ------------------------------------------------------------------ commands

def cmd_list(a) -> None:
    for f in true_functions(a.area):
        st = c_status(a.area, f["name"])
        extra = "" if len(f["pieces"]) == 1 else f" +{len(f['pieces'])-1} piece(s)"
        print(f"{f['name']}  link 0x{f['vram']:08X}  runtime 0x{f['runtime']:08X}"
              f"  size 0x{f['size']:X}  [{st}]{extra}")


def merged_asm(area: str, f: dict) -> str:
    p = area_paths(area)
    out = []
    for i, piece in enumerate(f["pieces"]):
        txt = (p["asm"] / f"{piece}.s").read_text(errors="replace")
        for line in txt.splitlines():
            s = line.strip()
            if i > 0 and (s.startswith("glabel") or s.startswith("nonmatching")):
                if s.startswith("glabel"):
                    out.append(f".L{piece}:")
                continue
            if s.startswith("endlabel") and i < len(f["pieces"]) - 1:
                continue
            if s.startswith("endlabel"):
                out.append(f"endlabel {f['name']}")
                continue
            if s.startswith(".globl L") or s.startswith(".globl .L"):
                continue
            out.append(line)
    txt = "\n".join(out) + "\n"
    # demote the cross-piece global labels fill_overlay created
    txt = re.sub(r"(?<![\w.])(L" + "overlay_" + area + r"_[0-9A-F]{8})", r".\1", txt)
    txt = re.sub(r"(nonmatching \S+, )0x[0-9A-F]+", lambda m: m.group(1) + hex(f["size"]).upper().replace("0X", "0x"), txt, count=1)
    return txt


def cmd_prep(a) -> None:
    p = area_paths(a.area)
    data, _, _ = load_bin(a.area)
    (p["work"] / "exp").mkdir(parents=True, exist_ok=True)
    (p["work"] / "s").mkdir(parents=True, exist_ok=True)
    funcs = true_functions(a.area)
    for f in funcs:
        body = data[HDR + f["vram"] - ARENA: HDR + f["end"] - ARENA]
        (p["work"] / "exp" / f"{f['name']}.o").write_bytes(build_elf(body, f["name"]))
        (p["work"] / "s" / f"{f['name']}.s").write_text(merged_asm(a.area, f))
    (p["work"] / "funcs.json").write_text(json.dumps(funcs, indent=1))
    print(f"[prep] {len(funcs)} functions -> {p['work'].relative_to(ROOT)}")


def leading(src: Path, key: str) -> str | None:
    for line in src.read_text(errors="replace").splitlines():
        s = line.strip()
        if not s:
            continue
        if not s.startswith("//"):
            break
        if s.startswith(key):
            return s[len(key):].strip()
    return None


def compile_batch(area: str, srcs: list[Path], tag: str,
                  ccs: list[str] | None = None) -> dict[str, Path]:
    p = area_paths(area)
    od = p["work"] / "obj"
    od.mkdir(parents=True, exist_ok=True)
    lines = ["#!/bin/sh"]
    outs = {}
    for s, cc in [(s, c) for s in srcs for c in (ccs or [None])]:
        exe = MWCC.get(cc if cc is not None else
                       (leading(s, "// COMPILER:") or ""), DEFAULT_EXE)
        flags = leading(s, "// CFLAGS:") or DEFAULT_FLAGS
        ctag = tag + (f"@{cc}" if cc is not None else "")
        o = od / f"{s.parent.name}__{s.stem}{ctag}.o"
        if o.exists():
            o.unlink()
        rs = s.resolve().relative_to(ROOT)
        lines.append(f"qemu-i386 tools/bin/wibo32 {exe} -c {flags} "
                     f"-o {o.relative_to(ROOT)} {rs} 2>&1 | grep -v 'MWCIncludes\\|Usage Warning'")
        outs[str(s) + (f"@{cc}" if cc is not None else "")] = o
    script = p["work"] / f"cc{tag}.sh"
    script.write_text("\n".join(lines) + "\n")
    r = subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work",
                        "-w", "/work", IMAGE, "sh",
                        str(script.relative_to(ROOT))],
                       capture_output=True, text=True)
    if r.stdout.strip():
        print(r.stdout.strip()[-3000:])
    return outs


def disasm(words: bytes, vram: int) -> list[str]:
    import rabbitizer
    out = []
    for i in range(0, len(words), 4):
        w = struct.unpack_from("<I", words, i)[0]
        ins = rabbitizer.Instruction(w, vram=vram + i,
                                     category=rabbitizer.InstrCategory.R5900)
        out.append(ins.disassemble())
    return out


def score(area: str, name: str, cand: Path, show: bool) -> tuple[float | None, list[str]]:
    p = area_paths(area)
    funcs = {f["name"]: f for f in json.loads((p["work"] / "funcs.json").read_text())}
    f = funcs[name]
    text, notes = resolve_object(cand, name, f["vram"])
    res = p["work"] / "res" / f"{name}.o"
    res.parent.mkdir(parents=True, exist_ok=True)
    res.write_bytes(build_elf(text, name))
    exp = p["work"] / "exp" / f"{name}.o"
    r = subprocess.run([str(ROOT / "tools/bin/objdiff-cli"), "diff", "-1", str(exp),
                        "-2", str(res), name, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    pct = None
    try:
        d = json.loads(r.stdout)
        for s in d["left"]["symbols"]:
            if s.get("name") == name:
                pct = s.get("match_percent")
    except Exception:
        notes.append("objdiff error")
    data, _, _ = load_bin(area)
    orig = data[HDR + f["vram"] - ARENA: HDR + f["end"] - ARENA]
    if text == orig:
        notes.append("BYTE-IDENTICAL")
    elif len(text) != len(orig):
        notes.append(f"size 0x{len(text):X} vs 0x{len(orig):X}")
    if show:
        import difflib
        A = disasm(orig, f["vram"])
        B = disasm(text, f["vram"])
        rows = []
        for tag_, i1, i2, j1, j2 in difflib.SequenceMatcher(
                None, A, B, autojunk=False).get_opcodes():
            n = max(i2 - i1, j2 - j1)
            for k in range(n):
                l = A[i1 + k] if i1 + k < i2 else ""
                rr = B[j1 + k] if j1 + k < j2 else ""
                off = f"{(i1+k)*4:4X}" if i1 + k < i2 else "    "
                rows.append((tag_ != "equal", f"{off} {l:<46}{'*' if tag_ != 'equal' else ' '} {rr}"))
        keep = set()
        for i, (d, _) in enumerate(rows):
            if d:
                keep.update(range(max(0, i - 2), min(len(rows), i + 3)))
        last = -1
        for i in sorted(keep):
            if last >= 0 and i != last + 1:
                print("   ...")
            print("   " + rows[i][1])
            last = i
    return pct, notes


def cmd_check(a) -> None:
    srcs = [Path(s) for s in a.files]
    ccs = a.cc.split(",") if a.cc else None
    outs = compile_batch(a.area, srcs, a.tag, ccs)
    for key, o in outs.items():
        s = Path(key.split("@")[0])
        label = f"{s.parent.name}/{s.stem}" + (f" @{key.split('@')[1]}" if "@" in key else "")
        if not o.exists():
            print(f"{label}: COMPILE FAILED")
            continue
        pct, notes = score(a.area, s.stem, o, a.show)
        print(f"{label}: {pct if pct is None else f'{pct:.2f}'}  {' '.join(notes)}")


def main() -> None:
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    for c in ("list", "prep", "check"):
        sp = sub.add_parser(c)
        sp.add_argument("area", type=str.upper)
        if c == "check":
            sp.add_argument("files", nargs="+")
            sp.add_argument("--show", action="store_true")
            sp.add_argument("--tag", default="")
            sp.add_argument("--cc", default="",
                            help="comma list to sweep: mwcc,mwcc233,mwcc24,mwcc30,mwcc301")
    a = ap.parse_args()
    {"list": cmd_list, "prep": cmd_prep, "check": cmd_check}[a.cmd](a)


if __name__ == "__main__":
    main()
