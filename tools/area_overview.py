#!/usr/bin/env python3
"""area_overview.py -- static census of one area (overlay + boot tables + captured RAM).

Reads the user's own boot ELF (config/SCUS_971.12), the area's overlay
(extract/OVERLAY/AREAnn.BIN), the extracted level-data blocks (extract/chunkNN*,
extract/manifest.txt), the local splat trees (build/asm/..., build/overlays/
AREAnn/asm/...) and, optionally, a captured EE RAM image with that area
resident. Writes a JSON report (addresses, counts, sizes, statuses, floats) to
the ignored build/s87/areaNN/ and prints a short summary. It never prints
instruction text, data blobs or on-screen text (the area title is kept only in
the ignored JSON).

One tree state. Decomp statuses come from ONE git revision of this repo
(--src-rev, default HEAD): src/*.c and src/overlays/AREAnn/*.c markers,
docs/FUNCTIONS.csv and config/symbol_addrs.txt are read from that commit with
git, never from the working tree. Boot link routes come from the provenance
audit (build/s87/census/provenance.json); the report names the source files
whose marker class changed between the audit and --src-rev. Port facts (the
first-level census rows, the script-host opcode switch, the address grep) come
from ONE revision of ../extermination-port (--port-rev, default HEAD).

What it derives (area N, subs from the area's own registries):
- the overlay's functions in ENGINE (runtime) addresses, grouped from splat's
  pieces by tools/overlay/overlay_match.py's true_functions (splat names are
  0x40 low: engine = splat label + 0x40; intra-overlay call targets split a
  function in two); slot sizes, status at --src-rev, byte-identity evidence
  (the build's compiled object, or with --compile-check a fresh compile of the
  committed file, resolved as the overlay link does and compared with the
  original bytes), direct calls, indirect-call count, data references, and the
  route census executions (--route-delta);
- who reaches each overlay function: placement/deferred behaviours, data-word
  code pointers, overlay calls;
- the registries D_0024D7C0 (placements, 0x28-byte records), D_0024D820
  (deferred groups, 0x2C-byte records), D_0024D650 (spawn entries, 0x30-byte
  records), D_0024E140 (door destinations, 4 bytes), D_00264DD0[N+1] (message
  records, 8 bytes), D_0026EC60 (area music rows), D_00264A70/D_00264AD0 (the
  area-paged sound remaps), D_00289B40/D_002671C0 (title; RAM only);
- the level data: INDEX sector N + 4 (001FFCD0), the extracted block and nested
  blocks with their files; with --ram the loaded descriptors (D_00289BC0, the
  nested descriptors at +0x100 + 0x70 * sub, D_00275C70), the load cursors
  D_0028A73C..48 and a residency sample of every file in the RAM image;
- script chains (0x40-byte records; word 0 bit 31 stop, bit 30 jump to word 1;
  op = word 0 & 0xFFF, sub = word 2) started from the data addresses that
  script-starting functions reference, with the ops the port's script host
  does not admit (opcodes from the committed em_area_script.c switch, sub rules
  from the dated snapshot ADMISSION_SNAPSHOT below);
- with --ram: the resident overlay check, the live actor pool (D_00275BC0 list
  over D_007A5640 + i*0x2F0) matched to the placement records, the area bytes,
  D_008106C8 and the music lane;
- the static census delta: boot functions reachable (direct calls, tail
  jumps and code-pointer references) from the area's roots that are not in the
  first-level census (build/s87/census/route_functions.json), with the owner
  roots that reach each one, and their lane under the LANES map below;
- which addresses the port's src/game names (plain grep, not evidence).

--doc PATH rewrites the generated blocks of a Markdown document in place (the
text between "<!-- area_overview:NAME begin -->" and "... end -->" markers);
--check-doc PATH only compares them and exits 1 on a difference. Prose
outside the markers is the author's.

Usage (macOS arm64, repo root; --compile-check needs the exterm-toolchain
container):
  .venv/bin/python tools/area_overview.py --area 1 \
      --ram build/s87/route/15_level_exit/eeMemory.bin [--compile-check] \
      [--doc ../extermination-port/docs/AREA01_OVERVIEW.md]
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import io
import json
import re
import struct
import subprocess
import sys
from collections import Counter, defaultdict
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PORT = ROOT.parent / "extermination-port"
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "overlay"))

ELF_SHA256 = "ee052236783e7d3e865754d3ff9fee71290addeb7d146c86caa7ff2724d1e17a"
ELF_VADDR, ELF_OFFSET, ELF_FILESZ = 0x100000, 0x300, 0x175B00
ARENA = 0x823500
SPLAT_SHIFT = 0x40

D_SPAWN, D_PLACE, D_DEFER, D_DOOR = 0x24D650, 0x24D7C0, 0x24D820, 0x24E140
D_MSG, D_MUSIC_ROWS = 0x264DD0, 0x26EC60
D_SND_REMAP_LO, D_SND_REMAP_HI = 0x264A70, 0x264AD0
D_TITLE_BASE, D_TITLE_STR = 0x289B40, 0x2671C0
POOL_BASE, POOL_HEAD, POOL_STRIDE = 0x7A5640, 0x275BC0, 0x2F0
D_LEVEL_DESC, D_NESTED_CUR, D_CURSORS = 0x289BC0, 0x275C70, 0x28A73C

STATUS_CODE = {"byte_matched_c": "BM", "nearmiss": "NM", "asm_inline": "AI", "asm_word": "AW",
               "c_linked_from_asm": "CL", "asm_undecompiled": "AU", "missing_source": "none",
               "c_nonperfect": "CN", "c_unknown_link": "C?"}

# ---------------------------------------------------------------------------
# Script-host admission: a DATED SNAPSHOT of the port's em_area_script.c.
# The opcode set is re-derived on every run from the execute() switch of the
# file at --port-rev. The per-sub rules below were read by hand from the file
# as committed in port b7868e1 (2026-09-25 19:52, blob ADMISSION_BLOB). When
# the blob at --port-rev differs, the report marks the sub rules stale and
# they must be re-read.
ADMISSION_SNAPSHOT = dict(
    port_commit="b7868e1",
    port_commit_date="2026-09-25 19:52:34 -0700",
    file="src/game/em_area_script.c",
    rules={
        # op: (admitted subs, text)
        0x00: ((0, 1, 2, 3, 4, 5, 6, 7, 9, 10), "op00 kinds 0-7, 9, 10 (kind 8 and larger kinds fault)"),
        0x01: ((0, 1, 2, 3, 4, 5, 6, 7, 9, 10), "op01 kinds 0-7, 9, 10 (kind 8 and larger kinds fault)"),
        0x0A: (tuple(s for s in range(4096) if s != 8), "op0A: sub 8 (001798D0) faults"),
        0x0B: ((0, 4, 6), "op0B: subs 0, 4 and 6 (other subs fault at 001B8020)"),
    },
)
ADMISSION_BLOB = "1df0dd3ad814a2c1444cfb00c5e685ed9bdd6a77"

# ---------------------------------------------------------------------------
# Per-area planning map (section 11 of AREA01_OVERVIEW.md). Owners are the
# roots of the census delta (overlay functions and boot behaviours). Titles
# are neutral (addresses and records); "claim" quotes labels taken from
# FINDINGS.md or decomp comments, which are NOT verified.
LANES = {
    1: [
        dict(code="O1", title="NPC 0x825350 (placement [36]), 0x825740 ([38]), 0x826CF0, and the 0x8261A0 / 0x8267C0 owners (sub 0)",
             claim="FINDINGS s69/s74 and the retired drawbridge export call [36] a crank, 0x8261A0 the bridge halves and 0x8267C0 a suspension fixture; the AREA01 route capture shows [36] is the control-room NPC (SECOND_LEVEL_ROUTE.md section 2) and no bridge moved on the route",
             owners=[0x825350, 0x8254B0, 0x825590, 0x825670, 0x825130, 0x825240, 0x825740, 0x826CF0,
                     0x8261A0, 0x826200, 0x826440, 0x8267C0, 0x826950, 0x1C50B0]),
        dict(code="O2", title="Owner 0x825950 and its spawns (sub 0)", claim="",
             owners=[0x825950, 0x825BE0, 0x825D30, 0x825EA0, 0x825F00, 0x825FC0, 0x826010, 0x825910,
                     0x8237D0, 0x823900, 0x8239C0]),
        dict(code="O3", title="Owner 0x823CD0 family (sub 0)", claim="",
             owners=[0x823CD0, 0x823A90, 0x8240E0, 0x824340, 0x824F70, 0x824FE0, 0x825040, 0x824770, 0x824D50]),
        dict(code="O4", title="Owner pair 0x826D40 / 0x828850 (both subs)",
             claim="SECOND_LEVEL_ROUTE.md guesses the 0x826D40 nodes are sentry guns (from a data page and their wall positions; not observed)",
             owners=[0x826D40, 0x8282F0, 0x8287C0, 0x828850]),
        dict(code="O5", title="Scripted shaft door 0x823580 (placement [12], sub 0)", claim="",
             owners=[0x823580]),
        dict(code="C1", title="Boot owners 0x15A2C0, 0x128C10, 0x12A5D0, 0x158D30, 0x159B90, 0x1C02E0, 0x1BFFD0 (sub 0)",
             claim="FINDINGS calls 0x128C10/0x12A5D0 bug brains and 0x158D30/0x159B90 creature-family fixtures",
             owners=[0x15A2C0, 0x128C10, 0x12A5D0, 0x158D30, 0x159B90, 0x1C02E0, 0x1BFFD0]),
        dict(code="C2", title="0x1E3D90 placements", claim="decomp comment: muzzle-flash driver (doubtful for 17 scattered placements)",
             owners=[0x1E3D90]),
        dict(code="C3", title="Slider doors 0x1BB860 and the class-0x0B records 0x1C2420", claim="FINDINGS s45/s63: slider door",
             owners=[0x1BB860, 0x1C2420]),
        dict(code="C4", title="0x1E7D20 (placement [35]) and the overlay init 0x823A50",
             claim="decomp comment: water surface over the D_00275C20 records", owners=[0x1E7D20, 0x823A50]),
        dict(code="C0", title="First-level census owners' unexercised paths", claim="",
             owners=[0x1551B0, 0x156620, 0x1BC350, 0x219550, 0x15AFA0, 0x1C4820, 0x1C5680]),
        dict(code="S1", title="Sub-1 owners (later)", claim="",
             owners=[0x1383C0, 0x147390, 0x1BF6B0, 0x1C06E0, 0x1C1A80, 0x826BA0]),
        dict(code="E", title="Shared engine delta (reached from more than one lane) and the code pointer 0x1F5040",
             claim="", owners=[0x1F5040]),
    ],
}
SHARED_LANE = dict(code="E", title="Shared engine delta (reached from more than one lane)", claim="", owners=[])
NO_LANE = {0x823540: "-"}   # the overlay's leading nop sled

# Labels for boot behaviours (claims, with their source; never evidence).
CLAIMS = {
    1: {0x128C10: "NPC update (decomp SEMANTICS); FINDINGS: bug brain", 0x12A5D0: "FINDINGS: bug brain (nest child)",
        0x147390: "entity dispatcher (decomp comment)", 0x1551B0: "crate (port em_crate_original)",
        0x156620: "drum (port em_drum_original)", 0x158D30: "FINDINGS s74: creature-family fixture",
        0x159B90: "FINDINGS s74: creature-family fixture", 0x15A2C0: "port em_enemy.c names it",
        0x15AFA0: "item pickup (port em_pickup_owner)", 0x1BB860: "slider door (FINDINGS s45/s63)",
        0x1BC350: "hinged door (port em_door_original)", 0x1BF6B0: "actor update (decomp SEMANTICS)",
        0x1C02E0: "actor state machine, companion of 001BFFD0 (decomp)", 0x1C06E0: "decomp name bone_root_pulse",
        0x1C1A80: "actor state machine with bone array (decomp)", 0x1C2420: "class-0x0B trigger record (8-byte leaf)",
        0x1C4820: "generic placed prop (port em_status_ui_leftovers)", 0x1C50B0: "flicker light (decomp comment)",
        0x1C5680: "indicator child (port em_indicator_child)",
        0x1E3D90: "muzzle-flash driver (decomp comment; doubtful for 17 scattered placements)",
        0x1E7D20: "water surface over D_00275C20 records (decomp comment)",
        0x1F5040: "fx_render (port em_weapon.c names it)", 0x219550: "item pickup (port em_pickup_owner)"},
}


# ---------------------------------------------------------------------------
# git access (one revision per repository)

def git(repo: Path, *args: str, stdin: bytes | None = None) -> bytes:
    r = subprocess.run(["git", "-C", str(repo), *args], input=stdin, capture_output=True)
    if r.returncode:
        raise SystemExit(f"git {' '.join(args)} in {repo}: {r.stderr.decode(errors='replace').strip()}")
    return r.stdout


class RevTree:
    """Files of one commit, read through git (never the working tree)."""

    def __init__(self, repo: Path, rev: str):
        self.repo = repo
        self.sha = git(repo, "rev-parse", "--verify", f"{rev}^{{commit}}").decode().strip()
        info = git(repo, "show", "-s", "--format=%h%n%ci%n%s", self.sha).decode().splitlines()
        self.short, self.date, self.subject = info[0], info[1], info[2] if len(info) > 2 else ""
        self._blobs: dict[str, str] = {}
        self._cache: dict[str, bytes] = {}

    def listing(self, prefix: str, recursive: bool = False) -> dict[str, str]:
        """path -> blob id for the blobs under prefix (a directory with '/')."""
        args = ["ls-tree"] + (["-r"] if recursive else []) + [self.sha, "--", prefix]
        out = {}
        for line in git(self.repo, *args).decode(errors="replace").splitlines():
            meta, path = line.split("\t", 1)
            _, typ, blob = meta.split()
            if typ == "blob":
                out[path] = blob
        self._blobs.update(out)
        return out

    def prefetch(self, paths) -> None:
        want = [p for p in paths if p in self._blobs and p not in self._cache]
        if not want:
            return
        data = git(self.repo, "cat-file", "--batch",
                   stdin="".join(self._blobs[p] + "\n" for p in want).encode())
        buf = io.BytesIO(data)
        for p in want:
            head = buf.readline().split()
            n = int(head[2])
            self._cache[p] = buf.read(n)
            buf.read(1)

    def blob(self, path: str) -> str | None:
        if path not in self._blobs:
            d = path.rsplit("/", 1)[0] + "/" if "/" in path else ""
            self.listing(d)
        return self._blobs.get(path)

    def read(self, path: str) -> bytes | None:
        if path not in self._cache:
            if self.blob(path) is None:
                return None
            self.prefetch([path])
        return self._cache.get(path)

    def text(self, path: str) -> str | None:
        b = self.read(path)
        return None if b is None else b.decode(errors="replace")

    def worktree_equal(self, *paths: str) -> bool:
        return subprocess.run(["git", "-C", str(self.repo), "diff", "--quiet", self.sha, "--", *paths]
                              ).returncode == 0

    def meta(self) -> dict:
        return dict(repo=str(self.repo), commit=self.sha, short=self.short, date=self.date,
                    subject=self.subject)


# ---------------------------------------------------------------------------
# Source markers (the classification tools/route_census.py applies to the
# working tree, applied here to the text of one commit)

def asm_kind_text(text: str) -> str:
    body = text[text.find("asm "):] if "asm " in text else text
    lines = [ln.strip() for ln in body.splitlines()[1:]
             if ln.strip() and not ln.strip().startswith(("//", "}"))]
    words = sum(1 for ln in lines if ln.startswith(".word"))
    if "All-word" in text or (lines and words * 2 >= len(lines)):
        return "asm_word"
    return "asm_inline"


def boot_marker_class(text: str | None, prov: dict) -> str:
    if text is None:
        return "missing_source"
    first = next((ln.strip() for ln in text.splitlines() if ln.strip()), "")
    route = prov.get("route", "")
    if first.startswith("// INCLUDE_ASM"):
        return "asm_undecompiled"
    if first.startswith("// NEARMISS"):
        return "nearmiss"
    if prov.get("source") == "inline_assembly" or re.search(r"^\s*asm\s+\w", text, re.M):
        return asm_kind_text(text)
    if route.startswith("compiled_object_") and prov.get("objdiff_perfect"):
        return "byte_matched_c"
    if route.startswith("compiled_object_"):
        return "c_nonperfect"
    if route.startswith("original_assembly_"):
        return "c_linked_from_asm"
    return "c_unknown_link"


def overlay_marker_class(text: str | None) -> str:
    """The classes of overlay_match.c_status, on the committed text."""
    if text is None:
        return "AU"
    if text.lstrip().startswith("// NEARMISS"):
        return "NM"
    if re.search(r"^\s*asm\s+\w[\w\s\*]*\(", text, re.M):
        return "AW" if asm_kind_text(text) == "asm_word" else "AI"
    return "C"


def boot_functions(src: RevTree, prov_path: Path):
    """addr -> function row with its status at src's commit, plus metadata on
    the provenance audit the link routes come from."""
    prov, prov_meta = {}, dict(path=str(prov_path), present=prov_path.exists())
    if prov_path.exists():
        prov = {r["name"]: r for r in json.loads(prov_path.read_text())["rows"]}
        mtime = datetime.fromtimestamp(prov_path.stat().st_mtime).astimezone()
        prov_meta["mtime"] = mtime.isoformat(timespec="seconds")
    rows = list(csv.DictReader(io.StringIO(src.text("docs/FUNCTIONS.csv"))))
    listing = src.listing("src/")
    src.prefetch([f"src/{r['name']}.c" for r in rows])
    funcs = {}
    for row in rows:
        a = int(row["vram"], 16)
        cls = boot_marker_class(src.text(f"src/{row['name']}.c"), prov.get(row["name"], {}))
        funcs[a] = dict(addr=a, name=row["name"], size=int(row["size_bytes"]), subsystem=row["subsystem"],
                        status=STATUS_CODE.get(cls, cls))
    # marker classes that changed between the audit's commit and src's commit
    changed = []
    if prov_path.exists():
        base = git(src.repo, "rev-list", "-1", f"--before={prov_meta['mtime']}", src.sha).decode().strip()
        prov_meta["audit_base_commit"] = base
        if base and base != src.sha:
            old = RevTree(src.repo, base)
            names = [ln for ln in git(src.repo, "diff", "--name-only", base, src.sha, "--", "src/").decode()
                     .splitlines() if ln.count("/") == 1 and ln.endswith(".c")]
            old.listing("src/")
            for p in names:
                n = p[4:-2]
                old_cls = boot_marker_class(old.text(p), prov.get(n, {}))
                new_cls = boot_marker_class(src.text(p), prov.get(n, {}))
                if old_cls != new_cls:
                    changed.append(dict(name=n, audit=old_cls, now=new_cls))
            prov_meta["src_files_changed_since_audit"] = len(names)
    prov_meta["marker_class_changed_since_audit"] = changed
    for c in changed:   # the audit's route cannot vouch for these; say so
        for f in funcs.values():
            if f["name"] == c["name"]:
                f["status"] += "?"
    del listing
    return funcs, prov_meta


# ---------------------------------------------------------------------------
# Image

class Image:
    """read(addr, n) over the ELF load segment, the overlay (flat at 0x823500)
    and, when given, the captured RAM (preferred for BSS and runtime state)."""

    def __init__(self, elf: bytes, overlay: bytes, ram: bytes | None):
        self.elf, self.ov, self.ram = elf, overlay, ram

    def static(self, a: int, n: int) -> bytes:
        if ELF_VADDR <= a and a + n <= ELF_VADDR + ELF_FILESZ:
            o = a - ELF_VADDR + ELF_OFFSET
            return self.elf[o:o + n]
        if ARENA <= a and a + n <= ARENA + len(self.ov):
            return self.ov[a - ARENA:a - ARENA + n]
        raise KeyError(hex(a))

    def live(self, a: int, n: int) -> bytes:
        if self.ram is None:
            raise KeyError("no RAM")
        return self.ram[a:a + n]

    def read(self, a: int, n: int) -> bytes:
        try:
            return self.static(a, n)
        except KeyError:
            return self.live(a, n)

    def u32(self, a):
        return struct.unpack("<I", self.read(a, 4))[0]

    def s16(self, a):
        return struct.unpack("<h", self.read(a, 2))[0]

    def u16(self, a):
        return struct.unpack("<H", self.read(a, 2))[0]


# ---------------------------------------------------------------------------
# Boot functions and call graph

SYM_RE = re.compile(r"(?:func|D)_(?:overlay_AREA\d\d_)?([0-9A-Fa-f]{6,8})$")
INSN_RE = re.compile(r"^\s*/\*\s*[0-9A-F]+\s+([0-9A-F]{8})\s+[0-9A-F]{8}\s*\*/\s+(\S+)\s*(.*)$")


def sym_addr(name: str, names: dict[str, int]) -> int | None:
    if name in names:
        return names[name]
    m = SYM_RE.search(name)
    return int(m.group(1), 16) if m else None


def parse_asm(path: Path, names: dict[str, int]):
    """(first address, calls, tail jumps, symbol refs, indirect-call count)."""
    first, calls, jumps, refs, indirect = None, set(), set(), set(), 0
    for line in path.read_text(errors="replace").splitlines():
        m = INSN_RE.match(line)
        if not m:
            continue
        addr, mn, ops = int(m.group(1), 16), m.group(2), m.group(3)
        if first is None:
            first = addr
        if mn == "jal":
            t = sym_addr(ops.strip(), names)
            if t is not None:
                calls.add(t)
        elif mn == "jalr":
            indirect += 1
        elif mn == "j":
            t = sym_addr(ops.strip(), names)
            if t is not None:
                jumps.add(t)
        for s in re.findall(r"%(?:hi|lo|gp_rel)\(([A-Za-z0-9_]+)\)", ops):
            t = sym_addr(s, names)
            if t is not None:
                refs.add(t)
    return first, calls, jumps, refs, indirect


def symbol_names(funcs, src: RevTree):
    names = {f["name"]: a for a, f in funcs.items()}
    for line in (src.text("config/symbol_addrs.txt") or "").splitlines():
        m = re.match(r"\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*0x([0-9A-Fa-f]+)\s*;", line)
        if m and m.group(1) not in names:
            names[m.group(1)] = int(m.group(2), 16)
    return names


def boot_graph(names):
    code = ROOT / "build/asm/matchings/main/code"
    graph = {}
    for p in code.glob("*.s"):
        first, calls, jumps, refs, ind = parse_asm(p, names)
        a = names.get(p.stem, first)
        if a is None:
            continue
        graph[a] = dict(calls=calls, jumps=jumps, refs=refs, indirect=ind)
    return graph


# ---------------------------------------------------------------------------
# Overlay functions

def overlay_functions(area_name: str, ov: bytes, text_end: int, names, src: RevTree,
                      compile_check: bool, out_dir: Path):
    import overlay_match as om
    groups = om.true_functions(area_name)
    code = ROOT / f"build/overlays/{area_name}/asm/matchings/{area_name}/code"
    pieces = {p.stem: p for p in code.glob("*.s")}
    sdir = f"src/overlays/{area_name}/"
    src.listing(sdir)
    wt_equal = src.worktree_equal(sdir)
    out = []
    for g in groups:
        calls, jumps, refs, ind = set(), set(), set(), 0
        for n in g["pieces"]:
            _, c, j, r, i = parse_asm(pieces[n], names)
            calls |= c; jumps |= j; refs |= r; ind += i
        path = f"{sdir}{g['name']}.c"
        text = src.text(path)
        st = overlay_marker_class(text)
        out.append(dict(addr=g["runtime"], splat_name=g["name"], splat_parts=g["pieces"], status=st,
                        size=g["slot_end"] - g["vram"], body_size=g["size"], link_vram=g["vram"],
                        body_end=g["end"], src_blob=src.blob(path),
                        calls=calls, jumps=jumps, refs=refs, indirect=ind))
    # a piece that is not the head of a function must have no source of its own
    heads = {g["name"] for g in groups}
    stray = sorted(p[len(sdir):-2] for p in src.listing(sdir) if p.endswith(".c")
                   and p[len(sdir):-2] not in heads)

    # byte-identity evidence for the C functions
    def resolved_equal(obj: Path, f) -> bool | None:
        try:
            text, _ = om.resolve_object(obj, f["splat_name"], f["link_vram"])
        except SystemExit:
            return None
        lo, hi = f["link_vram"] - ARENA + 0x40, f["body_end"] - ARENA + 0x40
        return text == ov[lo:hi]

    objdir = ROOT / f"build/overlays/{area_name}/obj"
    for f in out:
        if f["status"] != "C":
            continue
        o = objdir / f"{f['splat_name']}.o"
        s = ROOT / f"{sdir}{f['splat_name']}.c"
        ev = dict(build_obj=None)
        if o.exists():
            ev["build_obj"] = resolved_equal(o, f)
            ev["build_obj_newer_than_source"] = s.exists() and o.stat().st_mtime >= s.stat().st_mtime
        f["evidence"] = ev
    if compile_check:
        # The container mounts only the repo, so compile under build/ when
        # --out points elsewhere.
        work = out_dir if out_dir.resolve().is_relative_to(ROOT) else ROOT / "build" / "area_overview"
        cdir = work / "rev_src" / area_name
        odir = work / "rev_obj" / area_name
        cdir.mkdir(parents=True, exist_ok=True)
        odir.mkdir(parents=True, exist_ok=True)
        import compile_overlay_src as cos
        lines = ["#!/bin/sh"]
        todo = [f for f in out if f["status"] == "C"]
        for f in todo:
            c = cdir / f"{f['splat_name']}.c"
            c.write_bytes(src.read(f"{sdir}{f['splat_name']}.c"))
            o = odir / f"{f['splat_name']}.o"
            if o.exists():
                o.unlink()
            exe = cos.MWCC.get(cos.file_compiler(c), cos.DEFAULT_EXE)
            log = o.with_suffix(".log")
            lines.append(f"qemu-i386 tools/bin/wibo32 {exe} -c {cos.file_cflags(c)} "
                         f"-o {o.resolve().relative_to(ROOT)} {c.resolve().relative_to(ROOT)} "
                         f"> {log.resolve().relative_to(ROOT)} 2>&1 || echo 'COMPILE-FAIL {f['splat_name']}'")
        script = work / "rev_compile.sh"
        script.write_text("\n".join(lines) + "\n")
        r = subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                            "exterm-toolchain", "sh", str(script.resolve().relative_to(ROOT))],
                           capture_output=True, text=True)
        if r.returncode:
            raise SystemExit(f"compile check failed: {r.stdout[-2000:]}{r.stderr[-2000:]}")
        failed = set(re.findall(r"COMPILE-FAIL (\S+)", r.stdout))
        for f in todo:
            o = odir / f"{f['splat_name']}.o"
            if f["splat_name"] in failed or not o.exists():
                f["evidence"]["compiled_at_rev"] = None
                f["evidence"]["compile_error"] = True
            else:
                f["evidence"]["compiled_at_rev"] = resolved_equal(o, f)
    return out, dict(worktree_equals_rev=wt_equal, stray_sources=stray, compile_check=compile_check)


# ---------------------------------------------------------------------------
# Registries

def place_records(img: Image, table: int):
    recs = []
    a = table
    while img.u16(a) & 0xFF != 0xFF and len(recs) < 512:
        b = img.read(a, 0x28)
        cls, model, fl2, param, uid, kind, link = struct.unpack_from("<HBBHHHH", b, 0)
        pos = struct.unpack_from("<3f", b, 0x0C)
        rot = struct.unpack_from("<3f", b, 0x18)
        beh = struct.unpack_from("<I", b, 0x24)[0]
        recs.append(dict(index=len(recs), addr=a, cls=cls, model=model, flags2=fl2, param=param,
                         uid=uid, kind=kind, link=link, pos=[round(v, 4) for v in pos],
                         rot=[round(v, 5) for v in rot], behavior=beh,
                         deferred_at_load=(cls & 0xFF) == 0x0B))
        a += 0x28
    return recs


def defer_records(img: Image, item: int):
    recs = []
    a = item
    while img.s16(a) != -1 and len(recs) < 512:
        b = img.read(a, 0x2C)
        cond, tidx = struct.unpack_from("<hH", b, 0)
        cls, = struct.unpack_from("<H", b, 4)
        model, fl2 = b[6], b[7]
        param, = struct.unpack_from("<H", b, 8)
        uid, kind, link = struct.unpack_from("<HHH", b, 0xA)
        pos = struct.unpack_from("<3f", b, 0x10)
        rot = struct.unpack_from("<3f", b, 0x1C)
        beh, = struct.unpack_from("<I", b, 0x28)
        recs.append(dict(index=len(recs), addr=a, condition=cond, cond_arg=tidx, cls=cls,
                         model=model, flags2=fl2, param=param, uid=uid, kind=kind, link=link,
                         pos=[round(v, 4) for v in pos], rot=[round(v, 5) for v in rot],
                         behavior=beh))
        a += 0x2C
    return recs


def spawn_tables(img: Image, area: int):
    """Per sub: the 0x30-byte entries. A table ends at the next spawn table any
    area points at (tables are laid out back to back)."""
    starts = set()
    for ar in range(0x18):
        d = img.u32(D_SPAWN + 4 * ar)
        if not d:
            continue
        for s in range(8):
            try:
                t = img.u32(d + 4 * s)
            except Exception:
                break
            if ELF_VADDR <= t < ELF_VADDR + ELF_FILESZ:
                starts.add(t)
    starts = sorted(starts)
    desc = img.u32(D_SPAWN + 4 * area)
    subs = {}
    for s in range(8):
        t = img.u32(desc + 4 * s)
        if not (ELF_VADDR <= t < ELF_VADDR + ELF_FILESZ):
            break
        nxt = min([x for x in starts if x > t] + [t + 0x30 * 32])
        ents = []
        for i in range((nxt - t) // 0x30):
            b = img.read(t + 0x30 * i, 0x30)
            x, y, z, yaw = struct.unpack_from("<4f", b, 0)
            w = struct.unpack_from("<8I", b, 0x10)
            ents.append(dict(index=i, addr=t + 0x30 * i, pos=[round(x, 3), round(y, 3), round(z, 3)],
                             yaw=round(yaw, 5), w10=w[0], w14=w[1], w18=w[2], area_flags=w[3],
                             w20=w[4], bgm_cue=(w[3] >> 8) & 0x7F, bgm_bit7=(w[3] >> 15) & 1))
        subs[s] = dict(table=t, desc=desc, entries=ents)
    return subs


def door_table(img: Image, area: int):
    bases = sorted({img.u32(D_DOOR + 4 * a) for a in range(0x18) if img.u32(D_DOOR + 4 * a)})
    base = img.u32(D_DOOR + 4 * area)
    end = min([b for b in bases if b > base] + [base + 0x40])
    out = []
    for i in range((end - base) // 4):
        r = img.read(base + 4 * i, 4)
        out.append(dict(door_id=i, addr=base + 4 * i, bytes=list(r),
                        area_change=dict(next_area=r[0], entry=r[1],
                                         sub=(r[3] if r[2] else 0)),
                        room_move=dict(entry_side0=r[0], entry_side1=r[1])))
    return dict(base=base, end=end, records=out)


def message_table(img: Image, area: int):
    words = [img.u32(D_MSG + 4 * i) for i in range(24)]
    base = words[area + 1]
    extra = []
    try:
        extra.append(img.u32(0x275848 + 8))
    except Exception:
        pass
    cands = sorted({w for w in words + extra if w})
    end = min([c for c in cands if c > base] + [base + 8 * 400])
    recs = []
    for i in range((end - base) // 8):
        b = img.read(base + 8 * i, 8)
        recs.append(struct.unpack("<HBBHH", b))
    nonzero = [i for i, r in enumerate(recs) if any(r)]
    return dict(base=base, end=end, records=len(recs), nonzero_records=len(nonzero))


def music_rows(img: Image, area: int):
    rows, a = [], D_MUSIC_ROWS
    for _ in range(128):
        ar, z, trig, cue = struct.unpack("<4I", img.read(a, 16))
        if ar == 0xFFFFFFFF:
            break
        if ar == area:
            rows.append(dict(addr=a, trigger=trig, cue=cue))
        a += 16
    return rows


def sound_scope(img: Image, area: int, subs):
    out = {}
    for s in subs:
        cell = {}
        for name, remaps, first, n in (("0x3E8..0x5DB", D_SND_REMAP_LO, 0x3E8, 0x5DC - 0x3E8),
                                       ("0x7D0..0x9C3", D_SND_REMAP_HI, 0x7D0, 0x9C4 - 0x7D0)):
            t = img.u32(remaps + 4 * area)
            if not t:
                cell[name] = None
                continue
            t = img.u32(t + 4 * s)
            if not t:
                cell[name] = None
                continue
            b = img.read(t, n)
            ids = [first + i for i, v in enumerate(b) if v != 0xFF]
            cell[name] = dict(remap_table=t, present=len(ids), ranges=_ranges(ids))
        out[s] = cell
    return out


def _ranges(ids):
    rs = []
    for i in ids:
        if rs and i == rs[-1][1] + 1:
            rs[-1][1] = i
        else:
            rs.append([i, i])
    return [f"{a:#x}" if a == b else f"{a:#x}..{b:#x}" for a, b in rs]


# ---------------------------------------------------------------------------
# Level data (INDEX sector area + 4, 001FFCD0)

def level_data(area: int, ram: bytes | None):
    sector = area + 4
    manifest = {}
    for line in (ROOT / "extract/manifest.txt").read_text().splitlines():
        parts = line.split("\t")
        if len(parts) >= 3:
            manifest[parts[0]] = (int(parts[1], 16), int(parts[2], 16))
    blocks = []
    for d in sorted((ROOT / "extract").glob(f"chunk{sector:02d}*")):
        if not d.is_dir():
            continue
        files = []
        for f in sorted(d.glob("f*_id*.bin")):
            m = re.match(r"f(\d+)_id([0-9a-f]+)\.bin$", f.name)
            key = f"{d.name}/{f.name}"
            files.append(dict(name=key, index=int(m.group(1)), id=m.group(2), size=f.stat().st_size,
                              disc_offset=manifest.get(key, (None,))[0]))
        nested = re.search(r"\.n(\d+)$", d.name)
        blocks.append(dict(block=d.name, nested=int(nested.group(1)) if nested else None, files=files))
    rep = dict(index_sector=sector, blocks=blocks)
    if ram is None:
        return rep
    u32 = lambda a: struct.unpack_from("<I", ram, a)[0]
    by_disc = {b["files"][0]["disc_offset"]: b["block"] for b in blocks if b["files"]}

    def desc(a):
        return dict(at=a, word0=u32(a), disc_offset=u32(a + 4), size=u32(a + 8), word_0c=u32(a + 0xC),
                    nested_count=u32(a + 0x18), file_count=u32(a + 0x1C), block=by_disc.get(u32(a + 4)))
    top = desc(D_LEVEL_DESC)
    subs = [desc(D_LEVEL_DESC + 0x100 + 0x70 * s) for s in range(top["nested_count"])]
    cur = u32(D_NESTED_CUR)
    rep["ram"] = dict(top=top, nested=subs, d_00275c70=cur,
                      d_00275c70_is=next((f"nested descriptor {i}" for i, s in enumerate(subs) if s["at"] == cur),
                                         "top descriptor" if cur == D_LEVEL_DESC else "other"),
                      cursors=[u32(D_CURSORS + 4 * i) for i in range(4)])
    # residency: 64-byte windows at 8 evenly spaced offsets, skipping uniform windows
    for b in blocks:
        for f in b["files"]:
            data = (ROOT / "extract" / f["name"]).read_bytes()
            found = sampled = 0
            first = None
            for k in range(8):
                o = (len(data) - 64) * k // 7 if len(data) > 64 else 0
                w = data[o:o + 64]
                if len(set(w)) < 4:
                    continue
                sampled += 1
                at = ram.find(w)
                if at >= 0:
                    found += 1
                    if first is None:
                        first = at - o
            f["resident_windows"] = [found, sampled]
            f["resident_base_guess"] = first
    return rep


# ---------------------------------------------------------------------------
# Scripts

MAX_OP = 0x1A          # ftab_0024D880 handler count - 1 (ops 0x00..0x1A)


def walk_script(img: Image, entry: int, lo: int, hi: int):
    """The record chain from entry, or None when a record is not a script
    record (flags other than bits 31/30/29 set, op above MAX_OP, or the chain
    leaves the data section)."""
    pc, recs = entry, []
    for _ in range(128):
        if not (lo <= pc and pc + 0x40 <= hi):
            return None
        w0 = img.u32(pc)
        if w0 & 0x1FFFF000 or (w0 & 0xFFF) > MAX_OP:
            return None
        recs.append(dict(addr=pc, op=w0 & 0xFFF, sub=struct.unpack("<i", img.read(pc + 8, 4))[0],
                         w4=img.u32(pc + 4), w0=w0))
        if w0 & 0x80000000:
            return recs
        pc = img.u32(pc + 4) if w0 & 0x40000000 else pc + 0x40
    return None


def group_ok(img: Image, a: int, is_code) -> list | None:
    """0x2C-byte group at a: records until the halfword -1; every behaviour a
    function start (or 0). None when it does not parse that way."""
    try:
        recs = defer_records(img, a)
    except Exception:
        return None
    if not recs or len(recs) >= 512:
        return None
    if not all(r["behavior"] == 0 or is_code(r["behavior"]) for r in recs):
        return None
    if not any(r["behavior"] for r in recs):
        return None
    return recs


def script_admission(port: RevTree):
    """The port's script host at port's commit: admitted opcodes from its
    execute() switch, per-sub rules from ADMISSION_SNAPSHOT."""
    path = ADMISSION_SNAPSHOT["file"]
    text = port.text(path) or ""
    m = re.search(r"static EmScriptCommandResult execute\(.*?\n\}", text, re.S)
    ops = sorted(int(x, 16) for x in re.findall(r"case 0x([0-9A-Fa-f]+): return op[0-9A-Fa-f]+\(&o\);",
                                              m.group(0) if m else ""))
    if not ops:
        # A silent empty list would mark every op "not admitted".
        raise SystemExit(f"script_admission: no opcode cases parsed from {path}'s execute(); "
                         "update the pattern for the port's current script host")
    blob = port.blob(path)
    return dict(port=port.meta(), file=path, blob=blob, admitted_opcodes=ops,
                snapshot_commit=ADMISSION_SNAPSHOT["port_commit"],
                snapshot_date=ADMISSION_SNAPSHOT["port_commit_date"],
                snapshot_blob=ADMISSION_BLOB, sub_rules_current=blob == ADMISSION_BLOB,
                sub_rules={f"{k:02X}": v[1] for k, v in ADMISSION_SNAPSHOT["rules"].items()})


def not_admitted(ops_text, adm):
    out = []
    for o in ops_text:
        op, sub = o.split("/")
        op, sub = int(op, 16), int(sub)
        if op not in adm["admitted_opcodes"]:
            out.append(o)
            continue
        rule = ADMISSION_SNAPSHOT["rules"].get(op)
        if rule and sub not in rule[0]:
            out.append(o)
    return sorted(set(out))


# ---------------------------------------------------------------------------
# Live pool

def live_pool(img: Image):
    out = []
    a = img.u32(POOL_HEAD)
    seen = set()
    while a and a not in seen and len(out) < 0x100:
        seen.add(a)
        b = img.live(a, POOL_STRIDE)
        slot = (a - POOL_BASE) // POOL_STRIDE
        out.append(dict(slot=slot, addr=a, status=b[0], cls=b[2], model=b[3], param=b[0xD],
                        uid=struct.unpack_from("<H", b, 0xE)[0],
                        callback=struct.unpack_from("<I", b, 0x10)[0],
                        flags2=struct.unpack_from("<H", b, 0x2E)[0],
                        kind=struct.unpack_from("<H", b, 0x54)[0],
                        link=struct.unpack_from("<H", b, 0x56)[0],
                        table_index=b[0x9A],
                        pos=[round(v, 3) for v in struct.unpack_from("<3f", b, 0xB0)]))
        a = struct.unpack_from("<I", b, 0x1C)[0]
    return out


# ---------------------------------------------------------------------------
# Port facts (one port commit)

def port_index(port: RevTree):
    """value -> port src/game files whose text holds that hex number (6 to 8
    hex digits, optional 0x). A grep, not evidence of a translation."""
    idx = defaultdict(set)
    files = [p for p in port.listing("src/game/") if p.endswith((".c", ".h"))]
    port.prefetch(files)
    for p in files:
        for tok in re.findall(r"(?<![0-9A-Za-z_])(?:0[xX])?([0-9A-Fa-f]{6,8})(?![0-9A-Za-z])",
                              port.text(p)):
            idx[int(tok, 16)].add(p.rsplit("/", 1)[1])
    return idx, files


def census_rows(port: RevTree):
    """addr -> the FIRST_LEVEL_CENSUS.md row cells (whole)."""
    rows = {}
    for line in (port.text("docs/FIRST_LEVEL_CENSUS.md") or "").splitlines():
        m = re.match(r"^\| 0x([0-9A-F]{8}) \|", line)
        if not m:
            continue
        cells = [c.strip() for c in line.strip().strip("|").split(" | ")]
        if len(cells) < 5:
            continue
        rows[int(m.group(1), 16)] = dict(name=cells[1], decomp=cells[2], port=cells[3],
                                         module_test=cells[4], note=cells[5] if len(cells) > 5 else "",
                                         first=cells[6] if len(cells) > 6 else "")
    return rows


def census_label(row) -> str:
    """Port status and the module part of the Module / test cell, whole (the
    cell's own ' — ' separates the module from its tests)."""
    if not row:
        return ""
    return f"{row['port']} ({row['module_test'].split(' — ')[0]})"


# ---------------------------------------------------------------------------
# Markdown blocks

def f3(v):
    return ", ".join(("%g" % round(x, 3)) for x in v)


def short_reasons(reasons):
    c = Counter(re.sub(r"\[\d+\]", "[n]", x) for x in reasons)
    return "; ".join(f"{k}{' ×%d' % v if v > 1 else ''}" for k, v in c.items())


def counts(it):
    return ", ".join(f"{k} {v}" for k, v in Counter(it).most_common())


def markdown_blocks(rep) -> dict[str, str]:
    B = {}
    area = rep["area"]
    ov = rep["overlay"]
    ofs = rep["overlay_functions"]
    cd = rep["census_delta"]
    subs = sorted(rep["spawn"], key=int)
    srcm, portm = rep["trees"]["decomp"], rep["trees"]["port"]
    pool = rep.get("live_pool", [])
    src_slot = defaultdict(list)
    for n in pool:
        for s in n["source"]:
            src_slot[s.split(" (")[0]].append(n["slot"])
    ld = rep["level_data"]

    # ---- summary
    L = ["| Item | Value |", "|---|---|"]
    L.append(f"| Overlay | `AREA{area:02d}.BIN`, MWo3 id {ov['id']}, text {ov['text'][0]:#x}..{ov['text'][1]:#x}, "
             f"data ..{ov['data'][1]:#x}, BSS {ov['bss'][0]:#x}..{ov['bss'][1]:#x} |")
    L.append(f"| Sub-states | {len(subs)} (spawn descriptor entries); placement tables "
             + ", ".join(f"sub {s}: {rep['placements'][s]['table']:#x}" for s in subs) + " |")
    if "titles" in rep:
        t = rep["titles"]
        L.append(f"| Title (RAM) | D_00289B40[{area}] = {t['d_00289b40']:#010x}; "
                 + ", ".join(f"sub {s}: D_002671C0[{v['index']}] -> {v['string_at']:#x}" for s, v in sorted(t['subs'].items()))
                 + " |")
    L.append(f"| Level data | INDEX sector {ld['index_sector']}: "
             + "; ".join(f"`{b['block']}` ({len(b['files'])} files)" for b in ld["blocks"]) + " |")
    sc = Counter(f["status"] for f in ofs)
    L.append(f"| Overlay functions | {len(ofs)} (from {sum(len(f['splat_parts']) for f in ofs)} splat pieces), "
             f"{sum(f['size'] for f in ofs):,} bytes; at decomp {srcm['short']}: "
             + ", ".join(f"{k} {v}" for k, v in sc.most_common()) + " |")
    for s in subs:
        g = rep["deferred"][s]
        L.append(f"| Sub {s} roster | {len(rep['placements'][s]['records'])} placement records; deferred groups "
                 + (", ".join(f"{x['item']:#x} ({len(x['records'])})" for x in g) or "none") + " |")
    if rep["nest_groups"]:
        L.append("| Nest groups | " + ", ".join(f"link {n['link']}: {n['item']:#x} ({len(n['records'])})"
                                            for n in rep["nest_groups"]) + " |")
    if pool:
        L.append(f"| Live pool (RAM) | {len(pool)} nodes, {len({n['callback'] for n in pool})} behaviours |")
    L.append(f"| Scripts | {len(rep['scripts'])} chains, started by "
             f"{len({a for s in rep['scripts'] for a in s['started_by']})} overlay functions |")
    L.append(f"| Doors | destination table {rep['doors']['base']:#x}, {len(rep['doors']['records'])} records |")
    if "ram" in rep:
        L.append(f"| BGM at the capture | lane 0 cue {rep['ram']['music_lane0_cue']} (state {rep['ram']['music_lane0_state']}), "
                 f"D_008106C8 = {rep['ram']['d_008106c8']:#x} |")
    m = rep["messages"]
    L.append(f"| Messages | D_00264DD0[{area + 1}] = {m['base']:#x}..{m['end']:#x}, {m['records']} records |")
    L.append(f"| Static census delta | {cd['delta']} boot functions ({cd['delta_bytes']:,} bytes) not in the "
             f"first-level census; {cd['delta_sub0']} ({cd['delta_sub0_bytes']:,} bytes) from sub 0 / nest owners |")
    B["summary"] = "\n".join(L)

    # ---- level data
    L = ["| Block | Files (index: id, bytes; resident windows found/sampled) |", "|---|---|"]
    for b in ld["blocks"]:
        L.append(f"| `{b['block']}` | " + "; ".join(
            f"f{f['index']:02d}: {f['id']}, {f['size']:,}" + (f" ({f['resident_windows'][0]}/{f['resident_windows'][1]})"
                                                           if "resident_windows" in f else "")
            for f in b["files"]) + " |")
    if "ram" in ld:
        r = ld["ram"]
        L.append("")
        L.append("| Descriptor | At | Disc offset | Size | Nested count | File count | Block |")
        L.append("|---|---|---|---|---:|---:|---|")
        for name, d in [("top (D_00289BC0)", r["top"])] + [(f"nested {i} (+0x100 + 0x70 * {i})", d)
                                                           for i, d in enumerate(r["nested"])]:
            L.append(f"| {name} | {d['at']:#x} | {d['disc_offset']:#x} | {d['size']:#x} | {d['nested_count']} | "
                     f"{d['file_count']} | {('`' + d['block'] + '`') if d['block'] else '?'} |")
        L.append("")
        L.append(f"D_00275C70 = {r['d_00275c70']:#x} ({r['d_00275c70_is']}); load cursors D_0028A73C..48 = "
                 + ", ".join(f"{c:#x}" for c in r["cursors"]) + ".")
    B["level_data"] = "\n".join(L)

    # ---- registries
    reg = rep["registries"]
    L = ["| Registry | Area entry | " + " | ".join(f"Sub {s}" for s in subs) + " |",
         "|---|---|" + "---|" * len(subs)]
    L.append(f"| D_0024D7C0 placements (0x28-byte records, 0xFF end) | {reg['place_desc']:#x} | "
             + " | ".join(f"{rep['placements'][s]['table']:#x} ({len(rep['placements'][s]['records'])})" for s in subs) + " |")
    L.append(f"| D_0024D820 deferred groups (0x2C-byte records, -1 end) | {reg['defer_desc']:#x} | "
             + " | ".join((f"list {rep['deferred'][s][0]['list_at']:#x}: " if rep['deferred'][s] else "")
                          + (", ".join(f"{g['item']:#x} ({len(g['records'])})" for g in rep["deferred"][s]) or "none")
                          for s in subs) + " |")
    if rep["nest_groups"]:
        L.append(f"| D_0024D820 nest slots (D_0024A850[{area}] = {reg['nest_base']}) | {reg['defer_desc']:#x} | "
                 + ", ".join(f"[{reg['nest_base'] + n['link']}] link {n['link']}: {n['item']:#x} ({len(n['records'])})"
                             for n in rep["nest_groups"]) + " |" + " |" * (len(subs) - 1))
    L.append(f"| D_0024D650 spawn entries (0x30-byte records) | {reg['spawn_desc']:#x} | "
             + " | ".join(f"{rep['spawn'][s]['table']:#x} ({len(rep['spawn'][s]['entries'])})" for s in subs) + " |")
    d = rep["doors"]
    L.append(f"| D_0024E140 door destinations (4 bytes) | {d['base']:#x} | "
             + " | ".join([f"{len(d['records'])} records, shared (ends at {d['end']:#x})"] + ["same"] * (len(subs) - 1)) + " |")
    L.append(f"| D_00264DD0[{area + 1}] message line records (8 bytes) | {m['base']:#x} | "
             + " | ".join([f"{m['records']} records ({m['nonzero_records']} nonzero), shared, end {m['end']:#x}"]
                          + ["same"] * (len(subs) - 1)) + " |")
    if "titles" in rep:
        t = rep["titles"]
        L.append(f"| D_00289B40 title base/count (RAM) | {t['d_00289b40']:#010x} | "
                 + " | ".join(f"D_002671C0[{t['subs'][s]['index']}]" for s in subs) + " |")
    L.append(f"| D_0026EC60 area music rows | {len(rep['music_rows'])} rows | "
             + " | ".join(["; ".join(f"trigger {r['trigger']} → cue {r['cue']}" for r in rep["music_rows"]) or "none"]
                          + [""] * (len(subs) - 1)) + " |")
    B["registries"] = "\n".join(L)

    # ---- spawn
    L = ["| Entry | Position | Yaw | +0x10 | +0x14 | " + " | ".join(f"+0x1C sub {s}" for s in subs) + " | +0x20 |",
         "|---:|---|---:|---|---|" + "---|" * len(subs) + "---|"]
    ents = [rep["spawn"][s]["entries"] for s in subs]
    for i, a in enumerate(ents[0]):
        others = [e[i] for e in ents if i < len(e)]
        same = all(o["pos"] == a["pos"] and o["yaw"] == a["yaw"] and o["w10"] == a["w10"] and o["w14"] == a["w14"]
                   and o["w20"] == a["w20"] for o in others)
        L.append(f"| {a['index']} | {f3(a['pos'])} | {a['yaw']:g} | {a['w10']:#x} | {a['w14']} | "
                 + " | ".join(f"{o['area_flags']:#06x} (cue {o['bgm_cue']})" for o in others)
                 + f" | {a['w20']:#010x}{'' if same else ' (subs differ elsewhere)'} |")
    B["spawn"] = "\n".join(L)

    # ---- doors
    door_use = defaultdict(list)
    for s in subs:
        for x in rep["placements"][s]["records"]:
            if x["behavior"] in (0x1BC350, 0x1BB860) or (x["cls"] & 0xFF) == 0x85:
                door_use[x["flags2"] & 0x7F].append(
                    (x["flags2"] & 0x80, f"s{s}[{x['index']}] {x['behavior']:#x} fl {x['flags2']:#04x} "
                                         f"m {x['model']:#04x} ({f3(x['pos'])})"))
    L = ["| Door id | Record | Placements using it | Meaning |", "|---:|---|---|---|"]
    for dr in d["records"]:
        b = dr["bytes"]
        uses = door_use.get(dr["door_id"], [])
        if not uses:
            mean = "no placement uses it"
        elif any(u[0] for u in uses):
            mean = f"area change → area {b[0]} entry {b[1]} sub {b[3] if b[2] else 0}"
        else:
            mean = f"room move → entry {b[0]} (side 0) / {b[1]} (side 1)"
        L.append(f"| {dr['door_id']} | {' '.join('%02X' % x for x in b)} | {'<br>'.join(u[1] for u in uses) or '—'} | {mean} |")
    B["doors"] = "\n".join(L)

    # ---- sound
    ss = rep["sound_scope"]
    L = ["| Sub | 0x3E8..0x5DB present | 0x7D0..0x9C3 present |", "|---|---|---|"]
    for s in subs:
        cells = []
        for k in ("0x3E8..0x5DB", "0x7D0..0x9C3"):
            c = ss[s][k]
            cells.append("—" if c is None else f"{c['present']} (remap {c['remap_table']:#x}): {', '.join(c['ranges'])}")
        L.append(f"| {s} | {cells[0]} | {cells[1]} |")
    B["sound_scope"] = "\n".join(L)

    # ---- placements
    def place_table(s, full):
        hdr = ("| # | Class | Model | Flags2 | Param | UID | Kind | Link | Position | Yaw | Behaviour |"
               + (" Live |" if full else ""))
        L = [hdr, "|---:|---|---|---|---|---|---|---|---|---:|---|" + ("---|" if full else "")]
        for x in rep["placements"][s]["records"]:
            sl = src_slot.get(f"place[{x['index']}]")
            L.append(f"| {x['index']} | {x['cls']:#04x} | {x['model']:#04x} | {x['flags2']:#04x} | {x['param']:#x} | "
                     f"{x['uid']:#06x} | {x['kind']:#x} | {x['link']:#x} | {f3(x['pos'])} | {x['rot'][1]:g} | "
                     f"{x['behavior']:#x} |" + ((" " + (", ".join(map(str, sl)) if sl else "—") + " |") if full else ""))
        return "\n".join(L)
    cur = str(rep["ram"]["area_bytes"][1]) if "ram" in rep else None
    for s in subs:
        B[f"placements_sub{s}"] = place_table(s, s == cur)

    # ---- groups
    L = ["| Group | Records | How it is reached | Behaviours |", "|---|---:|---|---|"]
    for g in rep["groups"]:
        L.append(f"| {g['item']:#x} | {g['count']} | {g['label']}; {g['how']} | "
                 + ", ".join(f"{b:#x}" for b in g["behaviours"]) + " |")
    for g in rep["unreferenced_groups"]:
        L.append(f"| {g['item']:#x} | {g['count']} | unreferenced (no pointer found) | "
                 + ", ".join(f"{b:#x}" for b in g["behaviours"]) + " |")
    B["groups"] = "\n".join(L)
    L = ["| Group record | Cond (+0, +2) | Class | Model | Param | UID | Kind | Link | Position | Behaviour | Live |",
         "|---|---|---|---|---|---|---|---|---|---|---|"]
    lab = {g["item"]: g["label"] for g in rep["groups"]}
    for g in rep["groups"]:
        if cur is not None and g["label"].startswith("sub") and not g["label"].startswith(f"sub{cur} "):
            continue
        for x in rep["group_records"][f"{g['item']:#x}"]:
            sl = src_slot.get(f"{lab[g['item']]}[{x['index']}]")
            L.append(f"| {g['item']:#x}[{x['index']}] | {x['condition']}, {x['cond_arg']} | {x['cls']:#04x} | "
                     f"{x['model']:#04x} | {x['param']:#x} | {x['uid']:#06x} | {x['kind']:#x} | {x['link']:#x} | "
                     f"{f3(x['pos'])} | {x['behavior']:#x} | {', '.join(map(str, sl)) if sl else '—'} |")
    B["group_records"] = "\n".join(L)
    other = []
    for g in rep["groups"]:
        if cur is not None and g["label"].startswith("sub") and not g["label"].startswith(f"sub{cur} "):
            c = Counter(r["behavior"] for r in rep["group_records"][f"{g['item']:#x}"])
            other.append(f"{g['label']} ({g['count']}): " + ", ".join(f"{b:#x} ×{n}" for b, n in c.items()))
    B["group_records_other_sub"] = "\n".join(f"- {x}" for x in other) or "- none"

    # ---- live pool
    if pool:
        cr = rep["port_census_rows"]
        L = ["| Behaviour | Nodes | From | First-level census row (port status and module) |", "|---|---:|---|---|"]
        by = defaultdict(list)
        for n in pool:
            by[n["callback"]].append(n)
        for cb, ns in sorted(by.items(), key=lambda kv: (-len(kv[1]), kv[0])):
            srcs = Counter(re.sub(r"\[\d+\].*", "", s) for n in ns for s in (n["source"][:1] or ["runtime spawn"]))
            row = cr.get(f"{cb:#x}")
            L.append(f"| {cb:#x} | {len(ns)} | {'; '.join(f'{k} ×{v}' for k, v in srcs.items())} | "
                     f"{census_label(row) if row else ('in census, no row' if ns[0]['in_census'] else 'not in census')} |")
        B["live_pool"] = "\n".join(L)

    # ---- overlay functions
    lane_of = rep["lanes"]["overlay_lane"]
    ran = rep["route_census"]["overlay_ran"] if rep.get("route_census") else {}
    L = ["| Engine address | Splat piece(s) | Slot bytes | Decomp | Evidence | Route | Boot calls | Indirect calls | Reached by | Lane |",
         "|---|---|---:|---|---|---|---:|---:|---|---|"]
    for f in ofs:
        labs = ", ".join("`" + p.replace(f"func_overlay_AREA{area:02d}_", "").replace(f"overlay_AREA{area:02d}_func_", "")
                         + "`" for p in f["splat_parts"])
        rb = short_reasons(f["reached_by"]) or ("nop sled, no code" if f["addr"] == ov["text"][0] else "—")
        rb = re.sub(r"boot call from (0x[0-9a-f]+) \(([^)]*)\)", r"boot \1 (\2)", rb)
        ev = f.get("evidence", {})
        if ev.get("compile_error"):
            evs = "compiled at rev: COMPILE FAILED"
        elif "compiled_at_rev" in ev:
            evs = "compiled at rev: " + ("identical" if ev["compiled_at_rev"] else "DIFFERS")
        elif ev.get("build_obj") is not None:
            evs = "build obj: " + ("identical" if ev["build_obj"] else "DIFFERS")
        else:
            evs = "—"
        rr = ran.get(f"{f['addr']:#x}")
        L.append(f"| {f['addr']:#010x} | {labs} | {f['size']} | {f['status']} | {evs} | "
                 f"{('ran, ' + str(len(rr)) + (' beat' if len(rr) == 1 else ' beats')) if rr else '—'} | {len(f['calls_boot'])} | "
                 f"{f['indirect_calls']} | {rb} | {lane_of.get(f'{f['addr']:#x}', '?')} |")
    tot = defaultdict(int)
    for f in ofs:
        tot[f["status"]] += f["size"]
    L.append("")
    L.append(f"Totals ({len(ofs)} functions, {sum(f['size'] for f in ofs):,} slot bytes, decomp {srcm['short']}): "
             + ", ".join(f"{k} {Counter(x['status'] for x in ofs)[k]} / {v:,} B" for k, v in
                         sorted(tot.items(), key=lambda kv: -kv[1])) + ".")
    B["overlay_functions"] = "\n".join(L)
    g = defaultdict(list)
    for f in ofs:
        for a in f["global_refs"]:
            g[a].append(f["addr"])
    sp = sorted(a for a in g if 0x70000000 <= a < 0x70004000)
    L = ["| Address (%hi/%lo symbol) | Referenced by |", "|---|---|"]
    for a, fs in sorted(g.items()):
        if a not in sp:
            L.append(f"| {a:#x} | {', '.join(f'{x:#x}' for x in sorted(set(fs)))} |")
    if sp:
        L.append(f"| scratchpad {sp[0]:#x}..{sp[-1]:#x} ({len(sp)} addresses) | "
                 f"{len({x for a in sp for x in g[a]})} functions |")
    B["global_refs"] = "\n".join(L)

    # ---- scripts
    adm = rep["script_admission"]
    L = ["| Entry | Records | Started by | Ops | Not admitted by the port host |", "|---|---:|---|---|---|"]
    for s in rep["scripts"]:
        by_ = ", ".join(f"{a:#x}" for a in s["started_by"]) or ", ".join(f"word {a:#x}" for a in s["referenced_by_words"])
        L.append(f"| {s['entry']:#x} | {len(s['records'])} | {by_} | {' '.join(s['ops'])} | "
                 f"{', '.join(s['not_admitted']) or '—'} |")
    L.append("")
    L.append(f"Port column: port commit {adm['port']['short']} ({adm['port']['date'][:16]}), "
             f"`{adm['file']}`. Admitted opcodes (its execute() switch): "
             + " ".join(f"{o:02X}" for o in adm["admitted_opcodes"]) + ". Sub rules (dated snapshot of the file at "
             f"{adm['snapshot_commit']}, {adm['snapshot_date'][:10]}; "
             + ("current at this commit" if adm["sub_rules_current"] else "**STALE: the file changed since the snapshot**")
             + "): " + "; ".join(adm["sub_rules"].values()) + ".")
    B["scripts"] = "\n".join(L)

    # ---- boot roots
    claims = CLAIMS.get(area, {})
    cr = rep["port_census_rows"]
    live_by_cb = Counter(n["callback"] for n in pool)
    L = ["| Behaviour | Bytes | Decomp | Subsystem | Tables | Live (RAM) | First-level census row | Reach | Label (claim, unverified) |",
         "|---|---:|---|---|---|---:|---|---:|---|"]
    for b in rep["boot_roots"]:
        row = cr.get(f"{b['addr']:#x}")
        cc = Counter(re.sub(r"\[\d+\]", "", x) for x in b["reasons"])
        L.append(f"| {b['addr']:#x} | {b['size']} | {b['status']} | {b['subsystem']} | "
                 f"{'; '.join(f'{k} ×{v}' for k, v in cc.items())} | {live_by_cb.get(b['addr'], 0)} | "
                 f"{census_label(row) if row else ('in census, no row' if b['in_census'] else 'no')} | "
                 f"{b['delta_reached']} | {claims.get(b['addr'], '—')} |")
    B["boot_roots"] = "\n".join(L)

    # ---- census delta and lanes
    L = [f"Reach {cd['reachable']} boot functions ({cd['reachable_direct']} by direct calls only), "
         f"{cd['in_census']} already in the first-level census, **delta {cd['delta']} functions / "
         f"{cd['delta_bytes']:,} bytes** ({cd['delta_direct']} by direct calls only). Sub 0 and the nest reach "
         f"{cd['delta_sub0']} of them ({cd['delta_sub0_bytes']:,} bytes). Decomp status of the delta at "
         f"{srcm['short']}: {counts(f['status'] for f in cd['functions'])}. Beat-15 exit census: "
         f"{cd['exit_beat_new_reached']} of its {cd['exit_beat_new_total']} new functions are in the static reach."]
    rc = rep.get("route_census")
    if rc:
        L[0] += (f" AREA{area:02d} route census ({rc['file']}): {rc['delta_ran']} of the delta ran on the recorded "
                 f"route; {len(rc['ran_not_in_static_reach'])} boot functions it records as new are outside the "
                 f"static reach.")
    L += ["", "| Lane | Scope (neutral) | Overlay functions (n / slot bytes; statuses) | Boot delta (n / bytes) | "
              "Delta that ran at the arrival (beat 15) | Delta that ran on the route | Delta statuses |",
          "|---|---|---|---|---:|---:|---|"]
    for ln in rep["lanes"]["lanes"]:
        o = ln["overlay"]
        L.append(f"| {ln['code']} | {ln['title']} | "
                 + (f"{len(o)} / {ln['overlay_bytes']:,} B; {counts(ln['overlay_status'])}" if o else "—")
                 + f" | {len(ln['delta'])} / {ln['delta_bytes']:,} | {ln['delta_ran_arrival']} | "
                 f"{ln['delta_ran_route'] if rc else '—'} | {counts(ln['delta_status']) or '—'} |")
    B["lanes"] = "\n".join(L)
    F = {f["addr"]: f for f in cd["functions"]}
    L = []
    for ln in rep["lanes"]["lanes"]:
        if not ln["delta"]:
            continue
        L.append(f"- **{ln['code']}** ({len(ln['delta'])}): " + ", ".join(
            f"{a:06X} ({F[a]['size']}, {F[a]['status']}{', arrival' if F[a]['ran_in_exit_beat'] else ''}"
            f"{', route' if F[a].get('ran_on_route') else ''}{', port' if F[a]['port_files'] else ''})"
            for a in ln["delta"]) + ".")
    B["lane_deltas"] = "\n".join(L)
    L = []
    for ln in rep["lanes"]["lanes"]:
        if ln["claim"]:
            L.append(f"- {ln['code']}: {ln['claim']}.")
    B["lane_claims"] = "\n".join(L) or "- none"
    return B


def apply_doc(path: Path, blocks: dict[str, str], write: bool) -> list[str]:
    text = path.read_text()
    problems = []
    pat = re.compile(r"(<!-- area_overview:([a-z0-9_]+) begin -->)\n?(.*?)\n?(<!-- area_overview:\2 end -->)", re.S)
    seen = set()

    def sub(m):
        name = m.group(2)
        seen.add(name)
        if name not in blocks:
            problems.append(f"unknown block {name}")
            return m.group(0)
        if m.group(3) != blocks[name]:
            problems.append(f"block {name} differs")
        return m.group(1) + "\n" + blocks[name] + "\n" + m.group(4)
    new = pat.sub(sub, text)
    if write and new != text:
        path.write_text(new)
    for n in blocks:
        if n not in seen:
            problems.append(f"block {n} not in the document")
    return problems


# ---------------------------------------------------------------------------

def display(p: Path) -> str:
    try:
        return str(p.relative_to(ROOT))
    except ValueError:
        return str(p)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--area", type=int, default=1)
    ap.add_argument("--elf", type=Path, default=ROOT / "config/SCUS_971.12")
    ap.add_argument("--overlay", type=Path)
    ap.add_argument("--ram", type=Path, help="captured eeMemory.bin with the area resident")
    ap.add_argument("--src-rev", default="HEAD", help="decomp commit the statuses are read from")
    ap.add_argument("--port-rev", default="HEAD", help="port commit the census rows, script host and grep use")
    ap.add_argument("--census", type=Path, default=ROOT / "build/s87/census/route_functions.json")
    ap.add_argument("--exit-delta", type=Path, default=ROOT / "build/s87/census/exit_delta.json")
    ap.add_argument("--provenance", type=Path, default=ROOT / "build/s87/census/provenance.json")
    ap.add_argument("--route-delta", type=Path, help="route census delta of this area "
                    "(default build/s87/census/aNN_delta.json when present)")
    ap.add_argument("--compile-check", action="store_true",
                    help="compile each committed overlay C file in the exterm-toolchain container and "
                         "compare the resolved object with the original bytes")
    ap.add_argument("--out", type=Path)
    ap.add_argument("--doc", type=Path, help="rewrite this document's generated blocks")
    ap.add_argument("--check-doc", type=Path, help="compare this document's generated blocks; exit 1 on a difference")
    args = ap.parse_args()
    area = args.area
    area_name = f"AREA{area:02d}"
    ov_path = args.overlay or ROOT / f"extract/OVERLAY/{area_name}.BIN"
    elf = args.elf.read_bytes()
    if hashlib.sha256(elf).hexdigest() != ELF_SHA256:
        sys.exit("not the pinned SCUS-97112 boot ELF")
    ov = ov_path.read_bytes()
    if ov[:3] != b"MWo" or struct.unpack_from("<I", ov, 8)[0] != ARENA:
        sys.exit("not an MWo3 overlay loaded at 0x823500")
    ov_id, text_size, data_size, bss_size = struct.unpack_from("<I4xIII", ov, 4)
    text_lo, text_end = ARENA + 0x40, ARENA + 0x40 + text_size
    data_lo, data_end = text_end, text_end + data_size
    ram = args.ram.read_bytes() if args.ram else None
    img = Image(elf, ov, ram)
    out = (args.out or ROOT / f"build/s87/area{area:02d}/overview.json").resolve()
    out.parent.mkdir(parents=True, exist_ok=True)
    src = RevTree(ROOT, args.src_rev)
    port = RevTree(PORT, args.port_rev)
    rep: dict = dict(area=area, overlay=dict(file=display(ov_path), id=ov_id,
                     text=[text_lo, text_end], data=[data_lo, data_end], bss_size=bss_size,
                     bss=[data_end, data_end + bss_size]))
    rep["trees"] = dict(decomp=src.meta(), port=port.meta())

    if ram is not None:
        rep["ram"] = dict(path=display(args.ram), sha256=hashlib.sha256(ram).hexdigest(),
                          area_bytes=list(ram[0x810700:0x810705]),
                          overlay_id_resident=struct.unpack_from("<I", ram, ARENA + 4)[0],
                          text_identical=ram[ARENA:text_end] == ov[:text_end - ARENA],
                          data_words_changed=[a for a in range(data_lo, data_end, 4)
                                              if ram[a:a + 4] != ov[a - ARENA:a - ARENA + 4]],
                          d_008106c8=struct.unpack_from("<I", ram, 0x8106C8)[0],
                          d_00810d38=struct.unpack_from("<I", ram, 0x810D38)[0],
                          music_lane0_cue=struct.unpack_from("<I", ram, 0x282178)[0],
                          music_lane0_state=ram[0x282154])

    bfuncs, prov_meta = boot_functions(src, args.provenance)
    rep["trees"]["decomp"]["worktree_src_equals_commit"] = src.worktree_equal("src/", "docs/FUNCTIONS.csv",
                                                                              "config/symbol_addrs.txt")
    rep["trees"]["provenance_audit"] = prov_meta
    names = symbol_names(bfuncs, src)
    bgraph = boot_graph(names)
    ofuncs, ov_meta = overlay_functions(area_name, ov, text_end, names, src, args.compile_check, out.parent)
    rep["overlay_source"] = ov_meta
    ostarts = {f["addr"]: f for f in ofuncs}

    def is_code(v):
        return v in ostarts or v in bfuncs

    # ---- registries -------------------------------------------------------
    place_desc = img.u32(D_PLACE + 4 * area)
    defer_desc = img.u32(D_DEFER + 4 * area)
    spawn = spawn_tables(img, area)
    subs = sorted(spawn)
    nest_base = struct.unpack("<h", img.read(0x24A850 + 2 * area, 2))[0] or 1
    placements, deferred = {}, {}
    for s in subs:
        t = img.u32(place_desc + 4 * s)
        placements[s] = dict(table=t, records=place_records(img, t))
        items = []
        if defer_desc and s < nest_base:
            q = img.u32(defer_desc + 4 * s)
            while q:
                it = img.u32(q)
                if not it:
                    break
                items.append(dict(item=it, list_at=q, records=defer_records(img, it)))
                q += 4
        deferred[s] = items
    nests = []
    if defer_desc:
        k = nest_base
        while k < 64:
            g = img.u32(defer_desc + 4 * k)
            if not (data_lo <= g < data_end) or group_ok(img, g, is_code) is None:
                break
            nests.append(dict(link=k - nest_base, slot=defer_desc + 4 * k, item=g,
                              records=defer_records(img, g)))
            k += 1
    rep["registries"] = dict(place_desc=place_desc, defer_desc=defer_desc, nest_base=nest_base,
                             spawn_desc=img.u32(D_SPAWN + 4 * area))
    rep["placements"] = placements
    rep["deferred"] = deferred
    rep["nest_groups"] = nests
    rep["spawn"] = spawn
    rep["doors"] = door_table(img, area)
    rep["messages"] = message_table(img, area)
    rep["music_rows"] = music_rows(img, area)
    rep["sound_scope"] = sound_scope(img, area, subs)
    rep["level_data"] = level_data(area, ram)
    if ram is not None:
        tb = struct.unpack_from("<I", ram, D_TITLE_BASE + 4 * area)[0]
        titles = {}
        for s in subs:
            idx = (tb & 0xFFFF) + s
            p = struct.unpack_from("<I", ram, D_TITLE_STR + 4 * idx)[0]
            e = ram.index(b"\0", p)
            titles[s] = dict(index=idx, string_at=p, length=e - p, text=ram[p:e].decode("latin1"))
        rep["titles"] = dict(d_00289b40=tb, subs=titles)

    # ---- structures of the data section ------------------------------------
    slot_owner = {}         # data word address -> structure label (words inside structures)
    ptr_slots = {}          # data word address -> label of a legitimate pointer slot

    def claim(lo, hi, label):
        for a in range(lo, hi, 4):
            slot_owner.setdefault(a, label)

    for s in subs:
        t = placements[s]["table"]
        n = len(placements[s]["records"])
        if data_lo <= t < data_end:
            claim(t, t + 0x28 * n + 4, f"sub{s} placement table {t:#x}")
            for r in placements[s]["records"]:
                ptr_slots[r["addr"] + 0x24] = f"sub{s} place[{r['index']}]"
    groups = {}             # item -> dict(label, records)
    for s in subs:
        for g in deferred[s]:
            groups[g["item"]] = dict(label=f"sub{s} deferred group {g['item']:#x}", records=g["records"],
                                     how=f"D_0024D820[{area}][{s}] list")
    for n in nests:
        groups.setdefault(n["item"], dict(label=f"nest group link {n['link']} {n['item']:#x}",
                                          records=n["records"],
                                          how=f"D_0024D820[{area}][{nest_base}+{n['link']}]"))

    code_refs = defaultdict(set)    # data address -> overlay functions referencing it
    for f in ofuncs:
        for r in f["refs"]:
            if data_lo <= r < data_end:
                code_refs[r].add(f["addr"])
    word_refs = defaultdict(set)    # data address -> data words holding it
    for a in range(data_lo, data_end, 4):
        v = img.u32(a)
        if data_lo <= v < data_end:
            word_refs[v].add(a)

    scripts = {}
    pending = sorted(set(code_refs) | set(word_refs))
    for entry in pending:
        ch = walk_script(img, entry, data_lo, data_end)
        if ch is None:
            continue
        scripts[entry] = dict(entry=entry, records=[r["addr"] for r in ch],
                              ops=[f"{r['op']:02X}/{r['sub']}" for r in ch],
                              started_by=sorted(code_refs.get(entry, ())),
                              referenced_by_words=sorted(word_refs.get(entry, ())))
    in_chain = defaultdict(set)
    for e, sc in scripts.items():
        for a in sc["records"]:
            in_chain[a].add(e)
    for e, sc in scripts.items():
        sc["inside_other_chain"] = sorted(x for x in in_chain[e] if x != e)
        for a in sc["records"]:
            claim(a, a + 0x40, f"script {e:#x}")
    for e, sc in scripts.items():
        for a, o in zip(sc["records"], sc["ops"]):
            if o.startswith("09/"):
                ptr_slots[a + 4] = f"script {e:#x} op09 record {a:#x}"
    for a in sorted(set(code_refs) | set(word_refs)):
        if a in groups:
            continue
        recs = group_ok(img, a, is_code)
        if recs is None:
            continue
        via = []
        if a in code_refs:
            via.append("code " + ", ".join(f"{x:#x}" for x in sorted(code_refs[a])))
        for w in sorted(word_refs.get(a, ())):
            via.append(f"word {w:#x} ({slot_owner.get(w, 'unclassified')})")
        groups[a] = dict(label=f"group {a:#x}", records=recs, how="; ".join(via))
    for g, d in groups.items():
        n = len(d["records"])
        claim(g, g + 0x2C * n + 2, d["label"])
        for r in d["records"]:
            ptr_slots[r["addr"] + 0x28] = f"{d['label']}[{r['index']}]"
    orphan_groups = []
    a = data_lo
    while a < data_end - 0x2C:
        if a in slot_owner or img.u32(a + 0x28) not in ostarts and img.u32(a + 0x28) not in bfuncs:
            a += 4
            continue
        recs = group_ok(img, a, is_code)
        if recs and not any(x in slot_owner for x in range(a, a + 0x2C * len(recs), 4)):
            orphan_groups.append(dict(item=a, records=recs))
            claim(a, a + 0x2C * len(recs) + 2, f"unreferenced group {a:#x}")
            a += 0x2C * len(recs)
            continue
        a += 4
    rep["groups"] = [dict(item=g, label=d["label"], how=d["how"], count=len(d["records"]),
                          behaviours=sorted({r["behavior"] for r in d["records"]}))
                     for g, d in sorted(groups.items())]
    rep["unreferenced_groups"] = [dict(item=g["item"], count=len(g["records"]),
                                       behaviours=sorted({r["behavior"] for r in g["records"]}),
                                       records=g["records"])
                                  for g in orphan_groups]
    rep["group_records"] = {f"{g:#x}": d["records"] for g, d in groups.items()}
    adm = script_admission(port)
    rep["script_admission"] = adm
    for sc in scripts.values():
        sc["not_admitted"] = not_admitted(sc["ops"], adm)
    rep["scripts"] = sorted(scripts.values(), key=lambda s: s["entry"])

    pointer_sites = []
    for a in range(data_lo, data_end, 4):
        v = img.u32(a)
        if not is_code(v):
            continue
        if a in ptr_slots:
            kind = ptr_slots[a]
        elif a in slot_owner:
            kind = None       # a coincidence inside a structure (not a pointer field)
        else:
            kind = "unclassified data word"
        if kind:
            pointer_sites.append(dict(at=a, target=v, slot=kind))
    rep["pointer_sites"] = pointer_sites

    boot_entries = []
    for a, g in bgraph.items():
        for t in g["calls"] | g["jumps"]:
            if text_lo <= t < text_end:
                boot_entries.append(dict(caller=a, target=t,
                                         symbol=next((n for n, v in names.items() if v == t
                                                      and not n.startswith("func_")), None)))
    rep["boot_calls_into_arena"] = boot_entries

    # ---- roots --------------------------------------------------------------
    roots = defaultdict(list)
    for ps in pointer_sites:
        roots[ps["target"]].append(ps["slot"])
    for e in boot_entries:
        if e["target"] in ostarts and e["symbol"] and f"_state{area:02X}" in e["symbol"]:
            roots[e["target"]].append(f"boot call from {e['caller']:#x} ({e['symbol']})")
    for f in ofuncs:
        for c in f["calls"] | f["jumps"]:
            if c in ostarts:
                roots[c].append(f"call from {f['addr']:#x}")
        for r in f["refs"]:
            if r in ostarts or r in bfuncs:
                roots[r].append(f"code pointer in {f['addr']:#x}")

    idx, port_files = port_index(port)
    rep["port_files_naming_area"] = sorted(p.rsplit("/", 1)[1] for p in port_files
                                           if area_name in port.text(p))

    # route census (optional)
    rd = args.route_delta or ROOT / f"build/s87/census/a{area:02d}_delta.json"
    route = None
    if rd.exists():
        rj = json.loads(rd.read_text())
        route = dict(file=display(rd), sha256=hashlib.sha256(rd.read_bytes()).hexdigest(),
                     mtime=datetime.fromtimestamp(rd.stat().st_mtime).astimezone().isoformat(timespec="seconds"),
                     summary={k: v for k, v in rj.get("summary", {}).items() if not isinstance(v, (dict, list))},
                     overlay_ran={}, boot_ran=set())
        for f in rj.get("functions", []):
            a = int(f["addr"], 16)
            if f.get("region") == "boot":
                route["boot_ran"].add(a)
            elif a in ostarts:
                route["overlay_ran"][f"{a:#x}"] = f.get("beats", [])

    ofrep = []
    for f in ofuncs:
        ofrep.append(dict(addr=f["addr"], size=f["size"], body_size=f["body_size"], splat_parts=f["splat_parts"],
                          status=f["status"], src_blob=f["src_blob"], evidence=f.get("evidence", {}),
                          calls_boot=sorted(c for c in f["calls"] | f["jumps"] if c in bfuncs),
                          calls_overlay=sorted(c for c in f["calls"] | f["jumps"] if c in ostarts),
                          calls_unresolved=sorted(c for c in f["calls"] | f["jumps"]
                                                  if c not in bfuncs and c not in ostarts),
                          indirect_calls=f["indirect"],
                          data_refs=sorted(r for r in f["refs"] if data_lo <= r < data_end),
                          bss_refs=sorted(r for r in f["refs"] if data_end <= r < data_end + bss_size),
                          global_refs=sorted(r for r in f["refs"] if not (ARENA <= r < ARENA + 0x800000)),
                          code_pointer_refs=sorted(r for r in f["refs"] if is_code(r)),
                          reached_by=roots.get(f["addr"], [])))
    rep["overlay_functions"] = ofrep

    # ---- boot closure and the census delta -----------------------------------
    census = {int(x["addr"], 16) for x in json.loads(args.census.read_text())["functions"]}
    exit_new = set()
    if args.exit_delta.exists():
        for x in json.loads(args.exit_delta.read_text()).get("new_functions", []):
            exit_new.add(int(x["addr"], 16) if isinstance(x, dict) else int(x, 16))

    def closure(starts, with_refs=True):
        seen, stack = set(), list(starts)
        while stack:
            a = stack.pop()
            if a in seen or a not in bfuncs:
                continue
            seen.add(a)
            g = bgraph.get(a)
            if not g:
                continue
            nxt = g["calls"] | g["jumps"]
            if with_refs:
                nxt |= {r for r in g["refs"] if r in bfuncs}
            stack.extend(nxt)
        return seen

    owner_roots = {}
    for f in ofuncs:
        owner_roots[f["addr"]] = {c for c in f["calls"] | f["jumps"] | f["refs"] if c in bfuncs}
    for r in roots:
        if r in bfuncs:
            owner_roots.setdefault(r, set()).add(r)
    reach = {o: closure(st) for o, st in owner_roots.items()}
    reach_direct = {o: closure(st, with_refs=False) for o, st in owner_roots.items()}
    allreach = set().union(*reach.values()) if reach else set()
    alldirect = set().union(*reach_direct.values()) if reach_direct else set()
    delta = sorted(allreach - census)
    drep = []
    for a in delta:
        f = bfuncs[a]
        drep.append(dict(addr=a, name=f["name"], size=f["size"], status=f["status"],
                         subsystem=f["subsystem"], direct_call_path=a in alldirect,
                         ran_in_exit_beat=a in exit_new,
                         ran_on_route=(a in route["boot_ran"]) if route else None,
                         reached_from=sorted(o for o, s in reach.items() if a in s),
                         reached_from_direct=sorted(o for o, s in reach_direct.items() if a in s),
                         port_files=sorted(idx.get(a, ()))))
    rep["census_delta"] = dict(roots=len(owner_roots), reachable=len(allreach),
                               reachable_direct=len(alldirect),
                               in_census=len(allreach & census), delta=len(delta),
                               delta_bytes=sum(bfuncs[a]["size"] for a in delta),
                               delta_direct=len(alldirect - census),
                               exit_beat_new_total=len(exit_new),
                               exit_beat_new_reached=len(exit_new & allreach),
                               exit_beat_new_not_reached=sorted(exit_new - allreach),
                               functions=drep)
    if route:
        new_route = set()
        rj = json.loads(rd.read_text())
        for f in rj.get("new_functions", []):
            if f.get("region") == "boot":
                new_route.add(int(f["addr"], 16))
        route["delta_ran"] = sum(1 for d in drep if d["ran_on_route"])
        route["ran_not_in_static_reach"] = sorted(new_route - allreach)
        route["boot_ran"] = len(route["boot_ran"])
        rep["route_census"] = route
    bb = []
    for r, why in sorted(roots.items()):
        if r in bfuncs:
            f = bfuncs[r]
            bb.append(dict(addr=r, name=f["name"], size=f["size"], status=f["status"],
                           subsystem=f["subsystem"], in_census=r in census,
                           reasons=why, port_files=sorted(idx.get(r, ())),
                           delta_reached=len(reach.get(r, set()) - census)))
    rep["boot_roots"] = bb

    # ---- sub attribution: which sub's tables lead to each owner ------------
    owner_subs = defaultdict(set)
    for ps in pointer_sites:
        m = re.match(r"sub(\d+) ", ps["slot"])
        if m:
            owner_subs[ps["target"]].add(int(m.group(1)))
        elif ps["slot"].startswith("nest group"):
            owner_subs[ps["target"]].add("nest")
    for e in boot_entries:
        if e["target"] in ostarts and e["symbol"] and f"_state{area:02X}" in e["symbol"]:
            owner_subs[e["target"]].update(subs)
    group_of_fn = defaultdict(set)
    for g, d in groups.items():
        for part in d["how"].split("; "):
            for x in re.findall(r"0x[0-9a-f]+", part.split("(")[0]):
                group_of_fn[int(x, 16)].add(g)
    script_cb = defaultdict(set)
    for ps in pointer_sites:
        m = re.match(r"script (0x[0-9a-f]+) op09", ps["slot"])
        if m:
            script_cb[int(m.group(1), 16)].add(ps["target"])
    changed = True
    while changed:
        changed = False
        for f in ofuncs:
            s0 = owner_subs.get(f["addr"], set())
            if not s0:
                continue
            nxt = {c for c in f["calls"] | f["jumps"] | f["refs"] if is_code(c)}
            for g in group_of_fn.get(f["addr"], ()):
                nxt |= {r["behavior"] for r in groups[g]["records"] if r["behavior"]}
            for sc in f["refs"]:
                nxt |= script_cb.get(sc, set())
            for sc in scripts:
                if f["addr"] in scripts[sc]["started_by"]:
                    for a in scripts[sc]["records"]:
                        g = img.u32(a + 0x14)
                        if g in groups:
                            nxt |= {r["behavior"] for r in groups[g]["records"] if r["behavior"]}
            for t in nxt:
                if not s0 <= owner_subs[t]:
                    owner_subs[t] |= s0
                    changed = True
    rep["owner_subs"] = {f"{k:#x}": sorted(map(str, v)) for k, v in owner_subs.items()}
    for d in drep:
        ss = set()
        for o in d["reached_from"]:
            ss |= owner_subs.get(o, {"?"})
        d["subs"] = sorted(map(str, ss))
    rep["census_delta"]["delta_sub0"] = sum(1 for d in drep if "0" in d["subs"] or "nest" in d["subs"])
    rep["census_delta"]["delta_sub0_bytes"] = sum(d["size"] for d in drep
                                                  if "0" in d["subs"] or "nest" in d["subs"])
    excl = defaultdict(list)
    for d in drep:
        if len(d["reached_from"]) == 1:
            excl[d["reached_from"][0]].append(d["addr"])
    rep["owner_exclusive_delta"] = {f"{o:#x}": dict(count=len(v), bytes=sum(bfuncs[a]["size"] for a in v),
                                                   functions=v) for o, v in excl.items()}

    # ---- lanes (the LANES map; a delta function belongs to the lane of its
    # owners, the shared lane E when they span lanes, S1 when all are S1) -----
    lanes_def = LANES.get(area, [])
    lane_of_owner = {o: ln["code"] for ln in lanes_def for o in ln["owners"]}
    if lanes_def and not any(ln["code"] == "E" for ln in lanes_def):
        lanes_def = lanes_def + [SHARED_LANE]
    lanes = [dict(code=ln["code"], title=ln["title"], claim=ln["claim"], owners=ln["owners"], delta=[])
             for ln in lanes_def]
    by_code = {ln["code"]: ln for ln in lanes}
    unmapped_owners = set()
    if lanes_def:
        for d in drep:
            owners = d["reached_from_direct"] or d["reached_from"]
            ls = set()
            for o in owners:
                if o in lane_of_owner:
                    ls.add(lane_of_owner[o])
                else:
                    unmapped_owners.add(o)
                    ls.add("?")
            code = ls.pop() if len(ls) == 1 else ("S1" if ls <= {"S1"} else "E")
            if code == "?":
                code = "E"
            by_code[code]["delta"].append(d["addr"])
            d["lane"] = code
    ov_lane = {}
    for f in ofuncs:
        ov_lane[f"{f['addr']:#x}"] = NO_LANE.get(f["addr"], lane_of_owner.get(f["addr"], "?"))
    Fd = {d["addr"]: d for d in drep}
    for ln in lanes:
        o = [f for f in ofuncs if lane_of_owner.get(f["addr"]) == ln["code"]]
        ln["overlay"] = [f["addr"] for f in o]
        ln["overlay_bytes"] = sum(f["size"] for f in o)
        ln["overlay_status"] = [f["status"] for f in o]
        ln["delta_bytes"] = sum(Fd[a]["size"] for a in ln["delta"])
        ln["delta_status"] = [Fd[a]["status"] for a in ln["delta"]]
        ln["delta_ran_arrival"] = sum(Fd[a]["ran_in_exit_beat"] for a in ln["delta"])
        ln["delta_ran_route"] = sum(bool(Fd[a]["ran_on_route"]) for a in ln["delta"])
    rep["lanes"] = dict(lanes=lanes, overlay_lane=ov_lane,
                        unmapped_owners=sorted(unmapped_owners),
                        unmapped_overlay=sorted(k for k, v in ov_lane.items() if v == "?") if lanes_def else [])

    if ram is not None:
        pool = live_pool(img)
        s_cur = ram[0x810701]
        src_idx = {}
        for r in placements.get(s_cur, {}).get("records", []):
            src_idx.setdefault((r["behavior"], tuple(round(v, 2) for v in r["pos"])), []).append(
                f"place[{r['index']}]")
        for g, d in groups.items():
            for r in d["records"]:
                src_idx.setdefault((r["behavior"], tuple(round(v, 2) for v in r["pos"])), []).append(
                    f"{d['label']}[{r['index']}]")
        by_index = {}
        for r in placements.get(s_cur, {}).get("records", []):
            by_index.setdefault((r["behavior"], r["index"] & 0xFF), []).append(f"place[{r['index']}]")
        for g, d in groups.items():
            for r in d["records"]:
                by_index.setdefault((r["behavior"], r["cond_arg"] & 0xFF), []).append(
                    f"{d['label']}[{r['index']}]")
        for n in pool:
            n["source"] = src_idx.get((n["callback"], tuple(round(v, 2) for v in n["pos"])), [])
            if not n["source"]:
                n["source"] = [x + " (by +0x9A)" for x in by_index.get((n["callback"], n["table_index"]), [])]
            n["in_census"] = n["callback"] in census
        rep["live_pool"] = pool

    crows = census_rows(port)
    wanted = {n["callback"] for n in rep.get("live_pool", [])} | {b["addr"] for b in bb}
    rep["port_census_rows"] = {f"{a:#x}": crows[a] for a in sorted(wanted) if a in crows}

    # JSON: sets -> sorted lists; dict keys stay as written
    def enc(o):
        if isinstance(o, set):
            return sorted(o)
        raise TypeError(type(o))
    rep = json.loads(json.dumps(rep, default=enc))   # normalize (int keys become strings)
    blocks = markdown_blocks(rep)
    rep["markdown_blocks"] = sorted(blocks)
    out.write_text(json.dumps(rep, indent=1) + "\n")
    (out.parent / "tables.md").write_text("".join(
        f"<!-- area_overview:{k} begin -->\n{v}\n<!-- area_overview:{k} end -->\n\n" for k, v in blocks.items()))

    cd = rep["census_delta"]
    sc = Counter(f["status"] for f in rep["overlay_functions"])
    print(f"{area_name}: overlay id {ov_id}, text {text_lo:#x}..{text_end:#x}, data ..{data_end:#x}")
    print(f"  decomp {src.short} ({src.date[:16]}), worktree src equal: "
          f"{rep['trees']['decomp']['worktree_src_equals_commit']}; port {port.short} ({port.date[:16]})")
    print(f"  overlay functions {len(ofuncs)} (splat pieces {sum(len(f['splat_parts']) for f in ofuncs)}): "
          + ", ".join(f"{k} {v}" for k, v in sc.most_common()))
    evs = Counter(("compiled " + str(f["evidence"].get("compiled_at_rev"))) if "compiled_at_rev" in f["evidence"]
                  else ("build-obj " + str(f["evidence"].get("build_obj"))) for f in rep["overlay_functions"]
                  if f["status"] == "C")
    if evs:
        print("  C evidence: " + ", ".join(f"{k}: {v}" for k, v in evs.items()))
    for s in subs:
        print(f"  sub {s}: placements {len(placements[s]['records'])} @ {placements[s]['table']:#x}, "
              f"deferred groups {len(deferred[s])} "
              f"({sum(len(g['records']) for g in deferred[s])} records), "
              f"spawn entries {len(spawn[s]['entries'])} @ {spawn[s]['table']:#x}")
    print(f"  nest groups {len(nests)}, all groups {len(groups)}, unreferenced groups {len(orphan_groups)}")
    print(f"  doors {len(rep['doors']['records'])}, message records {rep['messages']['records']}, "
          f"scripts {len(scripts)}, music rows {len(rep['music_rows'])}, pointer sites {len(pointer_sites)}")
    print(f"  boot reach {cd['reachable']} (direct {cd['reachable_direct']}), in census {cd['in_census']}, "
          f"delta {cd['delta']} = {cd['delta_bytes']} bytes (direct {cd['delta_direct']}); "
          f"exit-beat new reached {cd['exit_beat_new_reached']}/{cd['exit_beat_new_total']}")
    if prov_meta.get("marker_class_changed_since_audit"):
        print(f"  provenance audit older than a marker change: {prov_meta['marker_class_changed_since_audit']}")
    if rep["lanes"]["unmapped_owners"] or rep["lanes"]["unmapped_overlay"]:
        print(f"  lanes: unmapped owners {rep['lanes']['unmapped_owners']}, overlay {rep['lanes']['unmapped_overlay']}")
    if not adm["sub_rules_current"]:
        print(f"  script admission sub rules are a stale snapshot ({adm['file']} blob {adm['blob']})")
    if ram is not None:
        print(f"  live pool {len(rep['live_pool'])} nodes; RAM text identical: {rep['ram']['text_identical']}")
    print(f"  wrote {display(out)} and {display(out.parent / 'tables.md')}")
    status = 0
    for path, write in ((args.doc, True), (args.check_doc, False)):
        if path:
            probs = apply_doc(path.resolve(), blocks, write)
            if write:
                print(f"  {display(path.resolve())}: rewrote generated blocks"
                      + (f" ({'; '.join(p for p in probs if 'differs' not in p)})"
                         if any('differs' not in p for p in probs) else ""))
            else:
                print(f"  {display(path.resolve())}: " + ("; ".join(probs) if probs else "generated blocks current"))
                status = 1 if probs else status
    sys.exit(status)


if __name__ == "__main__":
    main()
