#!/usr/bin/env python3
"""sfx_request_probe.py - every sound id the ORIGINAL requests along the first-level route.

Replays the route of tools/route_census.py (the startup segment S0..S3 and
route_capture's beats 00..14, hidden PCSX2 through tools/pcsx2_session.py,
exact one-frame steps, pad input only) with two persistent EE exec
breakpoints:

  001FB9F0  the id -> track start every sound request ends in (a0 = id,
            a1 = 0x1000, a2/a3 = the request pair; ra = the caller).  Its
            callers include 001FBD50 (positional), 001FC3C0 (the 10-tick
            loop service, through 001FBD50) and 001FC6E0 (the flush of the
            001FC580 impact queue).
  001FC580  the queued-impact entry (a1 = id), so a queued id is attributed
            to the code that queued it, not only to the flush.
  001FBD50  the positional entry (a1 = id): its caller is recorded, so the
            001FB9F0 start it makes is attributed to the code that asked.

At each hit the probe reads the EE registers and the area bytes
D_00810700/701, records them with the label and frame, and resumes (the
breakpoints stay armed).  For a 001FB9F0 hit it also places a one-shot
breakpoint on the return address and records v0 there (the track, or -1
when the original itself plays nothing).

The probe only pauses the EE and reads registers and memory; it never
writes memory.  Outputs go to the ignored build/sfx_probe/.  Nothing here
embeds original code or data; it names addresses only.

Usage (decomp .venv python, repo root):
    .venv/bin/python tools/sfx_request_probe.py run [--segments all|startup|05,06]
    .venv/bin/python tools/sfx_request_probe.py report
    .venv/bin/python tools/sfx_request_probe.py scan [--extra 00161790,...]   # census completeness
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
try:        # the PCSX2 route probe; the static scan (and the port test that
    import route_capture as rc  # noqa: E402    imports it) needs none of these
    import route_census as census  # noqa: E402
    from pcsx2_session import LOOP_TOP  # noqa: E402
    _SessionBase = census.CensusSession
except (ImportError, SystemExit):
    rc = census = None
    _SessionBase = object

OUT = ROOT / "build/sfx_probe"
START, QUEUE, POSITIONAL = 0x1FB9F0, 0x1FC580, 0x1FBD50
PROBES = (START, QUEUE, POSITIONAL)
POSITIONAL_RETURN = 0x1FBD94     # 001FBD50's own call of 001FB9F0 returns here


class SfxSession(_SessionBase):
    """CensusSession whose breakpoints are persistent request probes."""

    def __init__(self, *a, **k):
        super().__init__(*a, **k)
        self.gpr_cat: int | None = None
        self.returns: dict[int, list[dict]] = {}

    def regs(self) -> dict[str, int]:
        if self.gpr_cat is None:
            data = self.debug.call({"cmd": "read_registers"})["data"]
            names = [k for k in data if k not in ("pc", "hi", "lo")]
            self.gpr_cat = next(i for i, k in enumerate(names) if k.upper() == "GPR")
        data = self.debug.call({"cmd": "read_registers", "category": self.gpr_cat})["data"]
        cat = next(v for k, v in data.items() if k not in ("pc", "hi", "lo"))
        return {r["name"]: int(r["value"], 16) & 0xFFFFFFFF for r in cat["regs"]}

    def arm(self, addrs: list[int], label: str) -> float:
        t0 = time.monotonic()
        if self.armed:
            self.disarm_all()
        self.label = label
        self.hits = []
        self.returns: dict[int, list[dict]] = {}
        self.positional: dict | None = None
        for a in PROBES:
            self.debug.call({"cmd": "set_breakpoint", "address": a})
        self.armed = set(PROBES)
        return time.monotonic() - t0

    def _resume_to_boundary(self, timeout: float = 30.0) -> None:
        frame = self.frames_stepped + 1
        limit = max(timeout, self.stall_timeout)
        while True:
            self.debug.call({"cmd": "resume"})
            pc = self._wait_pause(limit)
            if pc == LOOP_TOP:
                return
            if pc in PROBES:
                r = self.regs()
                area = self.read(0x810700, 4)
                kind = {START: "start", QUEUE: "queue", POSITIONAL: "positional"}[pc]
                ev = {"pc": pc, "label": self.label, "frame": frame, "kind": kind,
                      "id": (r["a0"] if pc == START else r["a1"]) & 0xFFFFFFFF,
                      "a1": r["a1"], "a2": r["a2"], "a3": r["a3"], "ra": r["ra"],
                      "area": [area[0], area[1]]}
                if pc == POSITIONAL:
                    self.positional = ev          # 001FBF50 may refuse: no start follows
                    self.hits.append(ev)
                    continue
                if pc == START and r["ra"] == POSITIONAL_RETURN:
                    p = self.positional
                    assert p is not None and p["id"] == ev["id"] and p["frame"] == frame, (p, ev)
                    p["started"] = True
                    self.positional = None
                    ev = p                        # v0 is recorded on the positional event
                else:
                    self.hits.append(ev)
                if pc == START:
                    site = r["ra"]
                    if site not in self.armed:
                        self.debug.call({"cmd": "set_breakpoint", "address": site})
                        self.armed.add(site)
                    self.returns.setdefault(site, []).append(ev)
            elif pc in self.returns and self.returns[pc]:
                ev = self.returns[pc].pop()
                ev["v0"] = self.regs()["v0"]
                if not self.returns[pc] and pc not in PROBES:
                    self.debug.call({"cmd": "remove_breakpoint", "address": pc})
                    self.armed.discard(pc)
                    del self.returns[pc]
            else:
                self.unexpected.append({"pc": pc, "label": self.label, "frame": frame})
                if len(self.unexpected) > 50:
                    raise RuntimeError(f"unexpected pauses: {self.unexpected[-3:]}")


def install() -> None:
    census.OUT = OUT
    census.CensusSession = SfxSession


def run(segments: list[str], pass_name: str) -> None:
    install()
    addrs = list(PROBES)
    for seg in census.SEGMENTS:
        if seg not in segments and seg[:2] not in segments:
            continue
        if seg == "startup":
            census.run_startup(addrs, pass_name)
            continue
        name, source, fn = next(b for b in rc.BEATS if b[0] == seg)
        census.run_beat(name, source, fn, addrs, pass_name)


def report(pass_name: str) -> dict:
    runs = OUT / "runs" / pass_name
    ids: dict[int, dict] = {}
    labels = {}
    for label in census.ORDER:
        path = runs / f"{label}.json"
        if not path.exists():
            continue
        doc = json.loads(path.read_text())
        labels[label] = {"frames": doc.get("frames"), "completed": doc.get("completed", True),
                         "requests": len(doc["hits"])}
        for h in doc["hits"]:
            e = ids.setdefault(h["id"], {"id": hex(h["id"]), "starts": 0, "queued": 0,
                                         "refused": 0, "labels": [], "callers": [],
                                         "areas": [], "by_area": {}})
            area_key = f"{h['area'][0]}.{h['area'][1]}"
            # Totals over every area, and the same counts per area (the
            # port test reads only its own scope's counts).
            for counts in (e, e["by_area"].setdefault(
                    area_key, {"starts": 0, "queued": 0, "refused": 0})):
                counts["queued" if h["kind"] == "queue" else "starts"] += 1
                if h["kind"] == "positional" and not h.get("started"):
                    counts["out_of_range"] = counts.get("out_of_range", 0) + 1
                if h["kind"] != "queue" and h.get("v0") == 0xFFFFFFFF:
                    counts["refused"] += 1           # 001FB9F0 returned -1
                elif h["kind"] != "queue" and h.get("v0") is not None:
                    counts["played"] = counts.get("played", 0) + 1   # a track number
            for key, value in (("labels", label), ("callers", hex(h["ra"] - 8)),
                               ("areas", area_key)):
                if value not in e[key]:
                    e[key].append(value)
    import gen_sfx_registry as registry
    known = set(registry.first_level_ids()) | set(registry.SCENES["snow"]["ids"])
    area = [int(x) for x in registry.FIRST_LEVEL_AREA.split(".")]
    for e in ids.values():
        e["in_census"] = int(e["id"], 16) in known
    out = {"pass": pass_name, "labels": labels,
           "ids": [ids[k] for k in sorted(ids)],
           # Requests made in the census scope whose id neither the census
           # nor the AREA11 ("snow") preset carries; the preset holds the
           # fence-door pair 0x401 / 0x402, so they count as in the census.
           "outside_census": [ids[k]["id"] for k in sorted(ids)
                              if not ids[k]["in_census"] and
                              f"{area[0]}.{area[1]}" in ids[k]["areas"]]}
    (OUT / f"report_{pass_name}.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# Static census scan (the census completeness evidence, reproducible; the
# port's tools/test_area11_sfx_reference.py runs it too).
#
# 1. Reachable code. Seeds: every function the route census executed
#    (build/s87/census/route_functions.json), PORT_BOUND and any --extra.
#    From every included function the scan follows, to a fixpoint:
#      - direct calls and jumps (jal / j to a function start). This takes in
#        the direct successors and siblings of the states the route ran:
#        the player dispatch 0015B130 (route) calls every handler of its
#        state table jtbl_0026D3B0, so all player states are included, and
#        each state's own workers with them;
#      - address-taken functions: an upper/lower immediate pair that forms a
#        function start (a behavior or callback the code installs, e.g. the
#        impact marker 0018ABA0 that 00186A60 stores);
#      - function-pointer tables such a pair addresses: consecutive words
#        that are function starts (zero words skipped).
#    Function starts and sizes: build/s87/census/candidates.json (boot and
#    the AREA11 overlay). Calls through a register whose target is only
#    held in data the scan does not address are not followed; the route
#    seeds cover those the route ran.
# 2. Sound requests. At each jal / j into a sound entry (001FB9F0 a0,
#    001FBD50 / 001FC580 a1, 001FC3C0 a2) the id register is traced back:
#    the call's delay slot first, then linearly backwards to the function
#    start. The trace follows register moves and add/or-immediates; it
#    resolves a constant, a constant plus rand5 (the result of 00179B90,
#    0..4), or the function's own argument register. An argument makes the
#    function a forwarding entry (e.g. 001F02C0, the cue thunks 0020CD40..)
#    and its callers are traced in turn, to a fixpoint. A call (jal or
#    jalr) clobbers the caller-saved registers, so the trace stops there
#    unless it follows a saved register (s0..s7, fp); it also stops at a
#    load or any other computation. It builds no control-flow graph: the
#    value is where the register was last set in address order, which the
#    census cross-checks against the decomp C.
# 3. DATA sites. Every site whose id is loaded or computed must have a rule
#    in DATA_RULES: 'derived' rules read the ids from original data in the
#    capture (independent of the census); 'census' rules name the census
#    group that documents the family; 'forward' rules are sites whose ids
#    another entry already covers. A computed site without a rule fails the
#    completeness check.
# Output: build/sfx_probe/scan.json with addresses and id values only
# (no code). Code and data words are read from a captured AREA11 RAM image
# (boot code and the loaded AREA11 overlay at their run addresses).

SOUND_ENTRIES = {START: (4, 0x7FFF), POSITIONAL: (5, 0xFFFF), QUEUE: (5, 0xFFFF),
                 0x1FC3C0: (6, 0xFFFF)}            # callee -> (id register, id mask)
RAND5 = 0x179B90                                   # rand5: 0..4
CALLEE_SAVED = frozenset(range(16, 24)) | {28, 29, 30}
# Functions the census cites as CONST/DATA evidence (the port's first-level
# closures). Extra seeds: the closure from the route reaches all but a few.
PORT_BOUND = (
    0x001647D0, 0x00168050, 0x0016D130, 0x0016BC40, 0x0017DFB0, 0x00163E90,
    0x0021D490, 0x001747F0, 0x00187DE0, 0x0016F530, 0x0016F600, 0x0016FCF0,
    0x001703E0, 0x00170A60, 0x001729A0, 0x00173000, 0x001735C0, 0x00173E60,
    0x0017A970, 0x00171320, 0x001E3D90, 0x002149F0, 0x0020D930, 0x002160B0,
    0x00214020, 0x00211970, 0x001FDDB0, 0x0016AE40, 0x00182AB0, 0x00169730,
    0x00182AF0, 0x0016DE40, 0x0018A180, 0x0020CD60, 0x0020CD80, 0x0020CDA0,
    0x0020E0C0, 0x0020CDC0, 0x0021C120, 0x002255C0)
SCAN_RAM = ROOT / "build/startup-reference/playable_ee.bin"
FUNCTIONS = ROOT / "build/s87/census/candidates.json"
ROUTE_FUNCTIONS = ROOT / "build/s87/census/route_functions.json"
PLACEMENT_ENTRY = 0x810702        # D_00810702, the player-placement entry


def _word(ram: bytes, address: int) -> int:
    return int.from_bytes(ram[address:address + 4], "little")


def _half(ram: bytes, address: int) -> int:
    v = int.from_bytes(ram[address:address + 2], "little")
    return v - 0x10000 if v & 0x8000 else v


def _dest(w: int) -> int | None:
    """The GPR an instruction writes (None: none, or not a GPR)."""
    op, rs, rt, rd, fn = w >> 26, w >> 21 & 31, w >> 16 & 31, w >> 11 & 31, w & 63
    if op == 0:
        if fn in (0x08, 0x0C, 0x0D, 0x0F) or fn in (0x18, 0x19, 0x1A, 0x1B, 0x11, 0x13):
            return None                    # jr, syscall, break, sync, mult/div, mthi/mtlo
        return rd
    if op == 3:
        return 31
    if op == 0x1C:                         # MMI
        return rd
    if 8 <= op <= 15 or op in (0x18, 0x19):   # immediate arithmetic, lui
        return rt
    if op in (0x1A, 0x1B, 0x1E, 0x37) or 0x20 <= op <= 0x27:   # loads
        return rt
    if op in (0x10, 0x11, 0x12) and rs in (0, 1, 2):   # mfc / dmfc / cfc
        return rt
    return None


def _target(pc: int, w: int) -> int:
    return (pc & 0xF0000000) | (w & 0x3FFFFFF) << 2


def _trace(ram: bytes, start: int, site: int, reg: int) -> tuple:
    """Value of `reg` at the call at `site`: ('const', {values}), ('arg',
    register, addend) or ('computed', None)."""
    pcs = [site + 4] + list(range(site - 4, start - 4, -4))
    add, orr = 0, 0

    def const(values):
        return "const", frozenset((v + add) & 0xFFFFFFFF | orr for v in values)
    for i, pc in enumerate(pcs):
        w = _word(ram, pc)
        op, rs, rt, rd, fn = w >> 26, w >> 21 & 31, w >> 16 & 31, w >> 11 & 31, w & 63
        if i > 0 and (op == 3 or op == 0 and fn == 0x09):   # jal / jalr
            if reg in CALLEE_SAVED:
                continue                   # the callee preserves it
            if op == 3 and reg == 2 and _target(pc, w) == RAND5:
                return const(range(5))     # rand5's result
            return "computed", None        # the call clobbers the argument registers
        if _dest(w) != reg or reg == 0:
            continue
        imm, simm = w & 0xFFFF, (w & 0xFFFF) - ((w & 0x8000) << 1)
        if op in (9, 0x19, 8):             # addiu / daddiu / addi
            if rs == 0:
                return const((simm,))
            add += simm
            reg = rs
        elif op == 13:                     # ori
            if rs == 0:
                return const((imm,))
            orr |= imm
            reg = rs
        elif op == 15:                     # lui
            return const((imm << 16,))
        elif (op == 0 and fn in (0x21, 0x2D, 0x25) or          # move forms: addu,
              op == 0x1C and fn == 0x28 and w >> 6 & 31 == 0x18) and 0 in (rs, rt):  # daddu, or, paddub
            reg = rt if rs == 0 else rs
            if reg == 0:
                return const((0,))
        else:
            return "computed", None
    if 4 <= reg <= 7 and not orr:
        return "arg", reg, add             # an argument of the function itself
    return "computed", None


def reachable(ram: bytes, functions: dict, seeds) -> dict:
    """Closure of `seeds` over calls, address-taken functions and the
    function-pointer tables they address: {function: how it was reached}."""
    why = {f: "seed" for f in seeds if f in functions}
    work = list(why)
    while work:
        f = work.pop()
        _name, size = functions[f]
        found = []
        for pc in range(f, f + size, 4):
            w = _word(ram, pc)
            op = w >> 26
            if op in (2, 3):
                found.append((_target(pc, w), "call"))
            elif op == 15:                 # an upper immediate: pair it with its lower add
                hi, r = (w & 0xFFFF) << 16, w >> 16 & 31
                for q in range(pc + 4, min(pc + 40, f + size), 4):
                    x = _word(ram, q)
                    if x >> 26 == 9 and x >> 21 & 31 == r:
                        a = (hi + (x & 0xFFFF) - ((x & 0x8000) << 1)) & 0xFFFFFFFF
                        if a in functions:
                            found.append((a, "address"))
                        elif 0x100000 <= a < 0x2000000:
                            for k in range(a, a + 0x400, 4):
                                v = _word(ram, k)
                                if v in functions:
                                    found.append((v, f"table {a:08X}"))
                                elif v:
                                    break
                        break
        for a, how in found:
            if a in functions and a not in why:
                why[a] = f"{how} from {f:08X}"
                work.append(a)
    return why


def _placement_indices(ram: bytes, functions: dict, included) -> set[int]:
    """Constant values any included function stores to D_00810702."""
    out = set()
    for f in included:
        _name, size = functions[f]
        for pc in range(f, f + size, 4):
            w = _word(ram, pc)
            if w >> 26 == 0x28 and w & 0xFFFF == PLACEMENT_ENTRY & 0xFFFF:
                hi = next((_word(ram, q) for q in range(pc - 4, max(f, pc - 64) - 4, -4)
                           if _word(ram, q) >> 26 == 15 and _word(ram, q) >> 16 & 31 == w >> 21 & 31), 0)
                if (hi & 0xFFFF) << 16 != PLACEMENT_ENTRY & 0xFFFF0000:
                    continue
                kind, *value = _trace(ram, f, pc - 4, w >> 16 & 31)
                if kind == "const":
                    out |= {v & 0xFF for v in value[0]}
    return out


def _room_ambient(ram: bytes, functions: dict, included) -> dict:
    """001FC280: the high half of word +0x20 of D_0024D650[11][0] record
    D_00810702 (sra 16; -1 = none). The indices: AREA11's own records (0..3:
    the next room table, AREA13's, starts 0xC0 after it) and every constant
    an included function stores to D_00810702 (their area gating is not
    proven, so they are taken as reachable in 11.0)."""
    table = _word(ram, _word(ram, 0x24D650 + 11 * 4))
    indices = set(range(4)) | _placement_indices(ram, functions, included)
    ids = {_half(ram, table + i * 0x30 + 0x22) for i in sorted(indices)}
    return {"ids": sorted(i for i in ids if i != -1), "indices": sorted(indices),
            "excluded": {"0x44e": "forced when area 0xB and event flag 0x30 (D_00810788) "
                                  "== 0xFF: revisit-only (0 in every capture through beat 15)"}}


def _holster(ram: bytes, functions: dict, included) -> dict:
    """0016F600: D_00248680[self+0x275], the six sub-unit entries."""
    return {"ids": sorted({_half(ram, 0x248680 + 2 * i) & 0xFFFF for i in range(6)})}


def _effects(ram: bytes, functions: dict, included) -> dict:
    """001EF940: record +0x24 (-1 = none) of the global effect table
    (*D_00259C70, 0x78 records of 0x30) and of AREA11's table
    (D_00259C74[11], up to the end of the table region 0x259C70)."""
    ids = set()
    base = _word(ram, 0x259C70)
    area = _word(ram, 0x259C74 + 11 * 4)
    for a in [base + i * 0x30 for i in range(0x78)] + list(range(area, 0x259C70, 0x30)):
        v = _word(ram, a + 0x24)
        if v != 0xFFFFFFFF:
            ids.add(v & 0x7FFF)
    return {"ids": sorted(ids),
            "note": "001EF9D0's ids 0x80000026/2C/67 write 0x18C..0x193 (or -1) into the "
                    "global table's record 0, which no scanned code spawns by a constant id"}


# Computed sites: function -> (kind, census group or rule, what the id is).
DATA_RULES = {
    0x1FC280: ("derived", _room_ambient, "room ambient 001FC280"),
    0x16F600: ("derived", _holster, "holster table D_00248680[+0x275] (0016F600)"),
    0x1EF940: ("derived", _effects, "effects with a sound (001EF940)"),
    0x182430: ("census", "surface footsteps (00182430) and landings (00182870)",
               "block(+23A) + gait offset + rand5"),
    0x18A180: ("census", "equipment hit 0018A180", "0x180 + (rand() & 1, -1 for a negative odd value)"),
    0x1B8020: ("census", "preset: snow (the AREA11 door pair 0x401 / 0x402)",
               "door script record +0x18"),
    0x1FC6E0: ("forward", None, "the 001FC580 queue flush (001FC580 is an entry)"),
}


def scan(extra: list[int], census_ids=None, write: bool = True) -> dict:
    """The census completeness scan. census_ids: the census to check against
    (default: gen_sfx_registry's first-level ids plus the AREA11 preset)."""
    ram = SCAN_RAM.read_bytes()
    assert ram[0x810700:0x810702] == bytes([11, 0]), "scan RAM must be an AREA11 capture"
    functions = {f["addr"]: (f["name"], f["size"]) for f in json.loads(FUNCTIONS.read_text())}
    route = [int(r["addr"], 16) for r in json.loads(ROUTE_FUNCTIONS.read_text())["functions"]]
    seeds = set(route) | set(PORT_BOUND) | set(extra)
    why = reachable(ram, functions, seeds)
    import gen_sfx_registry as registry
    groups = {name for name, _ids, _evidence in registry.FIRST_LEVEL_GROUPS}
    if census_ids is None:
        census_ids = set(registry.first_level_ids()) | set(registry.SCENES["snow"]["ids"])
    known = set(census_ids)
    entries = dict(SOUND_ENTRIES)          # + forwarding entries: (register, mask, addend)
    entries = {k: (r, m, 0) for k, (r, m) in entries.items()}
    while True:                            # forwarding entries, to a fixpoint
        sites, grew = [], False
        for start in sorted(why):
            name, size = functions[start]
            for pc in range(start, start + size, 4):
                w = _word(ram, pc)
                if w >> 26 not in (2, 3) or _target(pc, w) not in entries:
                    continue
                target = _target(pc, w)
                reg, mask, addend = entries[target]
                kind, *value = _trace(ram, start, pc, reg)
                row = {"function": hex(start), "name": name, "site": hex(pc),
                       "callee": f"{target:08X}", "call": "tail" if w >> 26 == 2 else "call",
                       "kind": kind}
                if kind == "const":
                    row["ids"] = [hex((v + addend) & mask) for v in sorted(value[0])]
                elif kind == "arg" and start not in entries:
                    entries[start] = (value[0], mask, value[1] + addend)
                    grew = True
                sites.append(row)
        if not grew:
            break
    derived, unexplained, rules = set(), [], {}
    for row in sites:
        if row["kind"] != "computed":
            continue
        rule = DATA_RULES.get(int(row["function"], 16))
        if rule is None:
            unexplained.append(row)
            continue
        row["rule"] = rule[0]
        if rule[0] == "derived" and row["function"] not in rules:
            got = rule[1](ram, functions, why)
            rules[row["function"]] = dict(got, group=rule[2])
            derived |= set(got["ids"])
        elif rule[0] != "derived":
            rules.setdefault(row["function"], {"kind": rule[0], "group": rule[1], "id": rule[2]})
    const = {int(i, 16) for r in sites for i in r.get("ids", [])}
    reachable_ids = const | derived
    out = {"functions": len(why), "route_functions": len(set(route)),
           "seeds": len(seeds), "sites": sites,
           "forwarding_entries": {f"{k:08X}": v[0] for k, v in entries.items()
                                  if k not in SOUND_ENTRIES},
           "constant_ids": [hex(i) for i in sorted(const)],
           "derived_ids": [hex(i) for i in sorted(derived)],
           "data_rules": rules,
           "missing": [hex(i) for i in sorted(reachable_ids - known)],
           "unexplained_sites": unexplained,
           # 'census' rules must name a census group (or the preset).
           "rule_groups_missing": sorted({r["group"] for r in rules.values()
                                          if r.get("kind") == "census"
                                          and r["group"] not in groups
                                          and not r["group"].startswith("preset: ")}),
           "reached_by": {f"{f:08X}": why[f] for f in sorted(why) if why[f] != "seed"}}
    if write:
        OUT.mkdir(parents=True, exist_ok=True)
        (OUT / "scan.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", choices=["run", "report", "scan"])
    ap.add_argument("--segments", default="all")
    ap.add_argument("--extra", default="", help="scan: extra function addresses, hex, comma-separated")
    ap.add_argument("--pass", dest="pass_name", default="A")
    a = ap.parse_args()
    if a.command == "run":
        wanted = census.DEFAULT_SEGMENTS if a.segments == "all" else a.segments.split(",")
        run(wanted, a.pass_name)
    elif a.command == "scan":
        d = scan([int(x, 16) for x in a.extra.split(",") if x])
        computed = [r for r in d["sites"] if r["kind"] == "computed"]
        print(f"{d['functions']} reachable functions ({d['route_functions']} route-executed, "
              f"{d['seeds']} seeds), {len(d['sites'])} sound call sites, "
              f"{len(d['constant_ids'])} constant ids, {len(d['derived_ids'])} derived DATA ids, "
              f"{len(computed)} DATA sites, forwarding entries "
              f"{sorted(d['forwarding_entries'])} -> {OUT / 'scan.json'}")
        for f, rule in d["data_rules"].items():
            print(f"  DATA {f}: {rule}")
        for r in d["unexplained_sites"]:
            print(f"  UNEXPLAINED: {r['function']} {r['name']} site {r['site']} -> {r['callee']}")
        print("reachable ids missing from the census:", d["missing"])
        print("census rules naming no census group:", d["rule_groups_missing"])
        sys.exit(1 if d["missing"] or d["unexplained_sites"] or d["rule_groups_missing"] else 0)
    else:
        d = report(a.pass_name)
        for e in d["ids"]:
            print(f"{e['id']:>7} starts {e['starts']:4d} queued {e['queued']:3d} refused "
                  f"{e['refused']:3d} areas {','.join(e['areas'])} labels {','.join(e['labels'])}"
                  + ("" if e["in_census"] else "  (not in the census)"))
        print("requested in the census scope, not in the census:", d["outside_census"])
