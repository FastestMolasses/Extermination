#!/usr/bin/env python3
"""cut_content_census.py - which dialogue, voice and cutscene audio the code never reaches.

Static census over the user's own files (macOS arm64, no emulator):

  * the boot ELF (config/SCUS_971.12, pinned by SHA-256);
  * the extracted DATA.DAT chunks (extract/, rebuilt into one contiguous
    image from extract/manifest.txt, because some text banks straddle the
    extracted file boundaries) and extract/OVERLAY/AREAxx.BIN;
  * optionally the disc image (--iso) to decode orphan voice / stream cues.

What it pairs and counts (addresses are the original's; docs/CURIOSITIES.md
entry 29 and docs/FINDINGS.md "Cut-content census"):

  message records   D_00264DD0[0] (global) and [area + 1], 8-byte records
                    {u16 duration, s16 VOICE.DAT cue, u8 speaker/face slot,
                    u8 end flag}; each table ends where the next table the
                    original points at begins (port docs/MESSAGE_SERVICE.md).
  text banks        the global examine bank (asset slot 0x16) and one
                    per-area bank (slot 0x41), found by scanning the DATA.DAT
                    image for the group layout of FINDINGS "MESSAGE BANK
                    EXPORTED" and paired with the area table whose record
                    count equals the bank's line count (or line count + 1).
                    With --iso each pairing is checked against INDEX.IDX:
                    the bank must be that load set's slot-0x41 entry (0x16
                    for the global bank) by the loader's address rule.
  voice cues        the VOICE.DAT cue table D_0025E170 (178 cues). The only
                    voice-channel starts are 001FA5F0's, fed by 001FA5A0,
                    whose only callers (001FD580 / 001FD6A0) push a record's
                    +2 cue, so a cue no record names is never requested
                    (a jal scan of the ELF and every overlay is printed).
  stream cues       the MUSIC.DAT cue table D_0025DD30 and the (area,
                    trigger) -> cue rows D_0026EC60 that 001FD4C0 scans.
  scripts           64-byte event-script records (word 0 = flags | opcode,
                    the 001BA1F0 sequencer) in the ELF and every overlay.
                    op0C (001B7D60) subs 0/1 name a line at +0x14 (bit 31 =
                    global), op15 (001B6FA0) at +0x08; op07 (001B82D0) subs
                    9..12 and op1A (001B6AE0) pass a stream trigger at +0x18.
                    A started line plays records until one with a non-zero
                    end flag (001FDB80), so every line in that run is reached.
  map names         the spawn table D_0024D650[area][sub] record word +0x1C,
                    byte 2: low nibble = map, bits 4..5 = floor (001B0460
                    copies it to D_008106CD); names from the map group of the
                    help bank (chunk00/f02_id02.bin group 6).

Outputs (git-ignored; disc-derived, never commit): OUT/messages/<area>.tsv
(every record with its line text), OUT/census.json (counts, unreferenced
voice and stream cues, lines no script reaches, non-ASCII lines, script
references, area map/floor). With --iso: OUT/voice/cue_NNN.wav and
OUT/music/cue_NNN.wav for the cues no record or stream row names.

Usage (macOS arm64, decomp repo root or a worktree; paths default to the
main checkout's ignored inputs):
  python3 tools/cut_content_census.py --out build/cut_content/census
  python3 tools/cut_content_census.py --iso Extermination-rebuilt.iso \
      --out build/cut_content/census

The census is static: a call through a function pointer, or a line started
by overlay code that writes the message block directly, is not excluded.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import struct
import sys
from pathlib import Path

ELF_SHA256 = "ee052236783e7d3e865754d3ff9fee71290addeb7d146c86caa7ff2724d1e17a"
MAIN = Path(__file__).resolve().parents[1]
if MAIN.parent.name == "worktrees":          # .claude/worktrees/<name>
    MAIN = MAIN.parents[2]

MSG_TABLES, MSG_WORDS, GLOBAL_END = 0x264DD0, 24, 0x272FA0
VOICE_CUES, VOICE_COUNT = 0x25E170, 179
MUSIC_CUES, MUSIC_COUNT = 0x25DD30, 68
STREAM_ROWS = 0x26EC60
SPAWN_TABLE = 0x24D650
OVERLAY_BASE = 0x823500                     # file offset 0 (MWo3 header) runs here
VOICE_START = {0x1FA5A0: "001FA5A0", 0x1FA790: "001FA790", 0x1FD4C0: "001FD4C0"}
NO_TEXT = "<no text>"


class Elf:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        if hashlib.sha256(self.data).hexdigest() != ELF_SHA256:
            raise SystemExit(f"{path}: not the pinned SCUS-97112 boot ELF")

    def read(self, addr: int, size: int) -> bytes:
        off = addr - 0x100000 + 0x300
        return self.data[off:off + size]

    def u32(self, addr: int) -> int:
        return struct.unpack("<I", self.read(addr, 4))[0]

    def text(self) -> bytes:
        return self.data[0x300:0x300 + 0x175B00]


def u32(buf, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def data_image(extract: Path) -> tuple[bytearray, list[tuple[str, int, int]]]:
    """DATA.DAT rebuilt from the extracted files at their manifest offsets."""
    entries = []
    for line in (extract / "manifest.txt").read_text().splitlines():
        name, off, size, *_ = line.split("\t")
        entries.append((name, int(off, 16), int(size, 16)))
    image = bytearray(max(off + size for _, off, size in entries))
    for name, off, _ in entries:
        blob = (extract / name).read_bytes()
        image[off:off + len(blob)] = blob
    return image, entries


def bank_at(image, base: int):
    """Line strings of a message group at `base`, or None when the bytes are
    not one (the OUTER / TEXT headers of FINDINGS "MESSAGE BANK EXPORTED")."""
    if base + 16 > len(image):
        return None
    text_off, count, records_size, sixteen = (u32(image, base + 4 * i) for i in range(4))
    if sixteen != 0x10 or not 0 < count < 2000 or text_off != 0x10 + 16 * count:
        return None
    text = base + text_off + records_size
    if text + 16 > len(image):
        return None
    strbase, count2, strbytes, one = (u32(image, text + 4 * i) for i in range(4))
    if count2 != count or one != 1 or strbase != 0x10 + 16 * count:
        return None
    if text + strbase + strbytes > len(image):
        return None
    lines = []
    for i in range(count):
        off, length = u32(image, text + 0x10 + 16 * i), u32(image, text + 0x18 + 16 * i)
        lines.append(bytes(image[text + strbase + off:text + strbase + off + length]))
    return lines


def where(entries, addr: int) -> str:
    for name, off, size in entries:
        if off <= addr < off + size:
            return f"{name}+{addr - off:#x}"
    return f"{addr:#x}"


def decode_line(raw: bytes) -> tuple[str, bool]:
    """(printable text, non_ascii). Non-ASCII lines are tried as Shift-JIS."""
    if all(b < 0x80 for b in raw):
        return raw.decode("ascii"), False
    try:
        return raw.decode("shift_jis"), True
    except UnicodeDecodeError:
        return raw.decode("latin-1"), True


def message_tables(elf: Elf):
    words = [elf.u32(MSG_TABLES + 4 * i) for i in range(MSG_WORDS)]
    ends = sorted({w for w in words if w} | {GLOBAL_END})
    tables = {}
    for index, start in enumerate(words):
        if not start:
            continue
        end = min(e for e in ends if e > start)
        key = "global" if index == 0 else f"area{index - 1:02d}"
        tables[key] = [struct.unpack("<HhBBH", elf.read(start + 8 * i, 8))
                       for i in range((end - start) // 8)]
    return tables


def pair_banks(tables, banks, first_area_offset: int):
    """Area table -> bank: record count == line count (+1 for a trailing
    zero record). Area banks live in the area chunks (chunk04 on); the
    global chunks 00/03 hold the help, record and examine groups. Ties (two
    tables of one size) go in disc order."""
    pairs, used = {}, set()
    for key in sorted(tables, key=lambda k: (k != "global", k)):
        n = len(tables[key])
        for addr, lines in banks:
            if addr not in used and len(lines) in (n, n - 1):
                if key == "global" and len(lines) != n:
                    continue
                if key != "global" and addr < first_area_offset:
                    continue
                pairs[key] = (addr, lines)
                used.add(addr)
                break
    return pairs


def script_runs(blob: bytes, base: int):
    """Runs of valid 64-byte records ending in a STOP or JUMP record."""
    def valid(off):
        word = u32(blob, off)
        if word & 0x1FFFF000 or (word & 0xFFF) > 0x1A:
            return False
        if word & 0x40000000:
            target = u32(blob, off + 4)
            return base <= target < base + len(blob)
        return True
    runs, seen = [], set()
    for start in range(0, len(blob) - 64, 4):
        if start in seen or not valid(start):
            continue
        run, off = [], start
        while off + 64 <= len(blob) and valid(off):
            run.append(off)
            if u32(blob, off) & 0xC0000000:
                break
            off += 64
        else:
            continue
        last = u32(blob, run[-1])
        if not last & 0xC0000000 or (len(run) < 2 and not last & 0x80000000):
            continue
        seen.update(run)
        runs.append(run)
    return runs


def script_refs(blob: bytes, base: int):
    refs = {"lines": [], "streams": []}
    for run in script_runs(blob, base):
        for off in run:
            op, sub = u32(blob, off) & 0xFFF, u32(blob, off + 8)
            addr = base + off
            if op == 0x0C and sub in (0, 1):
                refs["lines"].append((addr, "op0C", u32(blob, off + 0x14)))
            elif op == 0x15:
                refs["lines"].append((addr, "op15", u32(blob, off + 8)))
            elif (op == 0x07 and sub in (9, 10, 11, 12)) or op == 0x1A:
                refs["streams"].append((addr, f"op{op:02X}", u32(blob, off + 0x18)))
    return refs


def spawn_entries(blob: bytes, base: int):
    """op14 (001BAC00) spawn entries, 0x2C bytes: u8 class (9 actor, 8
    attachment), u8 0, u8 mode, u8 0, u16 model resource, u16 clip bank,
    u16 clip, u16 command, f32 step, 0x1C zero bytes."""
    hits = []
    for off in range(0, len(blob) - 0x2C, 4):
        cls, zero, mode, zero2, model, bank, clip, cmd = struct.unpack_from("<BBBBHHHH", blob, off)
        step = struct.unpack_from("<f", blob, off + 0xC)[0]
        if (cls in (8, 9) and not zero and not zero2 and mode in (1, 2) and 0x40 <= model < 0x100
                and bank < 0x100 and clip < 0x200 and cmd < 0x20 and step in (0.0, 0.25, 0.5, 1.0, 2.0)
                and not any(blob[off + 0x10:off + 0x2C]) and not (cls == 9 and bank == 0)):
            hits.append({"addr": hex(base + off), "class": cls, "mode": mode, "model": hex(model),
                         "bank": hex(bank), "clip": clip, "command": cmd})
    return hits


def jal_targets(blob: bytes, base: int, targets):
    hits = []
    for off in range(0, len(blob) - 3, 4):
        word = u32(blob, off)
        if word >> 26 == 3 and (word & 0x3FFFFFF) << 2 in targets:
            hits.append((base + off, targets[(word & 0x3FFFFFF) << 2]))
    return hits


def run_chain(records, start: int):
    out, i = [], start
    while i < len(records):
        out.append(i)
        if records[i][3]:
            break
        i += 1
    return out


def census(args) -> dict:
    elf = Elf(args.elf)
    image, entries = data_image(args.extract)
    banks = []
    for addr in range(0, len(image) - 16, 16):
        lines = bank_at(image, addr)
        if lines:
            banks.append((addr, lines))
    tables = message_tables(elf)
    pairs = pair_banks(tables, banks, min(off for name, off, _ in entries if name.startswith("chunk04")))

    overlays = {p.stem: p.read_bytes() for p in sorted((args.extract / "OVERLAY").glob("AREA*.BIN"))}
    refs = {name: script_refs(blob, OVERLAY_BASE) for name, blob in overlays.items()}
    refs["ELF"] = script_refs(elf.text(), 0x100000)

    voice = [struct.unpack("<IIII", elf.read(VOICE_CUES + 16 * i, 16)) for i in range(VOICE_COUNT)]
    music = [struct.unpack("<IIII", elf.read(MUSIC_CUES + 16 * i, 16)) for i in range(MUSIC_COUNT)]
    rows, addr = [], STREAM_ROWS
    while elf.u32(addr) != 0xFFFFFFFF:
        rows.append(struct.unpack("<IIII", elf.read(addr, 16)))
        addr += 16

    out = {"banks": {}, "areas": {}, "voice": {}, "streams": {}, "maps": {}}
    msg_dir = args.out / "messages"
    msg_dir.mkdir(parents=True, exist_ok=True)
    voice_users = {}
    non_ascii = []
    for key, records in tables.items():
        bank_addr, raw_lines = pairs.get(key, (None, []))
        out["banks"][key] = {"records": len(records), "lines": len(raw_lines),
                             "bank": where(entries, bank_addr) if bank_addr is not None else None}
        with open(msg_dir / f"{key}.tsv", "w", newline="") as fh:
            w = csv.writer(fh, delimiter="\t")
            w.writerow(["line", "duration", "voice", "slot", "end", "text"])
            for i, rec in enumerate(records):
                text, odd = decode_line(raw_lines[i]) if i < len(raw_lines) else (NO_TEXT, False)
                w.writerow([f"{i:#x}", rec[0], rec[1], rec[2], rec[3], text])
                if rec[1] > 0:
                    voice_users.setdefault(rec[1], []).append(f"{key}:{i:#x}")
                if odd:
                    non_ascii.append({"table": key, "line": f"{i:#x}", "voice": rec[1]})
        if key == "global":
            continue
        area = int(key[4:])
        starts = set()
        for _, _, word in refs.get(f"AREA{area:02d}", {"lines": []})["lines"]:
            if not word & 0x80000000:
                starts.add(word)
        for _, _, trig in refs.get(f"AREA{area:02d}", {"streams": []})["streams"]:
            starts.add(trig)
        reached = set()
        for s in starts:
            if s < len(records):
                reached.update(run_chain(records, s))
        texted = [i for i in range(min(len(records), len(raw_lines))) if raw_lines[i]]
        slots = {}
        for i in texted:
            slots[records[i][2]] = slots.get(records[i][2], 0) + 1
        out["areas"][key] = {
            "script_starts": [hex(s) for s in sorted(starts)],
            "text_lines": len(texted),
            "unreached_lines": [hex(i) for i in texted if i not in reached],
            "speaker_slots": {str(k): v for k, v in sorted(slots.items())},
        }
    out["non_ascii_lines"] = non_ascii

    out["voice"]["cues"] = VOICE_COUNT - 1
    out["voice"]["named_by_records"] = len(voice_users)
    out["voice"]["unreferenced"] = [
        {"cue": c, "seconds": round(voice[c][2] / 16 * 28 / 48000, 2),
         "previous_named": max(k for k in voice_users if k < c),
         "next_named": min((k for k in voice_users if k > c), default=None)}
        for c in range(1, VOICE_COUNT) if c not in voice_users]
    calls = jal_targets(elf.text(), 0x100000, VOICE_START)
    for name, blob in overlays.items():
        calls += [(a, t) for a, t in jal_targets(blob, OVERLAY_BASE, VOICE_START)]
    out["voice"]["direct_calls"] = [(hex(a), t) for a, t in calls]

    triggers = {}
    for name, r in refs.items():
        for addr, op, trig in r["streams"]:
            triggers.setdefault(name, set()).add(trig)
    stream_rows = []
    for area, _, trig, cue in rows:
        started = trig in triggers.get(f"AREA{area:02d}", set())
        stream_rows.append({"area": area, "trigger": trig, "cue": cue, "script_started": started})
    named = {r["cue"] for r in stream_rows}
    out["streams"]["rows"] = stream_rows
    out["streams"]["cutscene_range_without_row"] = [
        {"cue": c, "seconds": round((music[c][2] >> 11) * 0.0746514 / 2, 1)}
        for c in range(29, MUSIC_COUNT) if c not in named]

    for area in range(0x17):
        rooms = elf.u32(SPAWN_TABLE + 4 * area)
        if not rooms:
            continue
        subs = []
        for sub in range(4):
            room = elf.u32(rooms + 4 * sub)
            if not room:
                break
            byte = (elf.u32(room + 0x1C) >> 16) & 0xFF
            subs.append({"sub": sub, "map": byte & 0xF, "floor": (byte >> 4) & 3})
        out["maps"][f"area{area:02d}"] = subs
    out["spawn_entries"] = {name: hits for name, blob in overlays.items()
                            if (hits := spawn_entries(blob, OVERLAY_BASE))}
    out["script_refs"] = {name: {"lines": [(hex(a), op, hex(w)) for a, op, w in r["lines"]],
                                 "streams": [(hex(a), op, t) for a, op, t in r["streams"]]}
                          for name, r in refs.items()}
    return out, voice, music, entries


def slot41_addresses(index: bytes) -> dict[int, str]:
    """DATA.DAT address of every slot-0x41 entry (and the global slot 0x16)
    by the loader's rule: descriptor base + resident start (+0x14) + entry
    offset (docs/REPACK.md "Descriptor"; the legacy extractor ignores the
    resident start, so extracted file names can differ from slots)."""
    def entries(block):
        groups = (struct.unpack_from("<H", block, 0xC)[0] + struct.unpack_from("<H", block, 0xE)[0]
                  + u32(block, 0x10))
        table = 0x20 + 8 * groups
        base, resident = u32(block, 4), u32(block, 0x14)
        for i in range(u32(block, 0x1C)):
            word = u32(block, table + 4 * i)
            yield word >> 24, base + resident + (word & 0xFFFFFF)
    found = {}
    for sector in range(len(index) // 0x800):
        desc = index[sector * 0x800:(sector + 1) * 0x800]
        if u32(desc, 0) != sector:
            continue
        blocks = [("", desc)] + [(f".n{n}", desc[0x100 + 0x70 * n:0x170 + 0x70 * n])
                                 for n in range(min(u32(desc, 0x18), 8))]
        for suffix, block in blocks:
            if not u32(block, 4):
                continue
            for slot, addr in entries(block):
                if slot in (0x41, 0x16):
                    found[addr] = f"chunk{sector:02d}{suffix} slot {slot:#x}"
    return found


def check_slots(args, report, entries):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from repack.iso import inventory
    index = inventory(args.iso)["files"]
    index = next(f for f in index if f["path"] == "DATA/INDEX.IDX")
    with open(args.iso, "rb") as fh:
        fh.seek(index["offset"])
        slots = slot41_addresses(fh.read(index["size"]))
    offsets = {name: off for name, off, _ in entries}
    for key, bank in report["banks"].items():
        name, off = bank["bank"].split("+")
        bank["loader_slot"] = slots.get(offsets[name] + int(off, 16))


def export_audio(args, report, voice, music):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import audio_export as ae
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from repack.iso import inventory
    files = {f["path"]: f for f in inventory(args.iso)["files"]}
    v_off, m_off = files["STREAM/VOICE.DAT"]["offset"], files["STREAM/MUSIC.DAT"]["offset"]
    (args.out / "voice").mkdir(parents=True, exist_ok=True)
    (args.out / "music").mkdir(parents=True, exist_ok=True)
    with open(args.iso, "rb") as fh:
        for item in report["voice"]["unreferenced"]:
            _, start, length, _ = voice[item["cue"]]
            fh.seek(v_off + start)
            ae.write_wav(args.out / "voice" / f"cue_{item['cue']:03d}.wav",
                         ae.decode_adpcm(fh.read(length)), 48000, 1)
        for item in report["streams"]["cutscene_range_without_row"]:
            _, start, length, _ = music[item["cue"]]
            fh.seek(m_off + start)
            left, right = ae.deinterleave(fh.read(length), 64)
            ae.write_wav(args.out / "music" / f"cue_{item['cue']:03d}.wav",
                         ae.interleave_pcm(ae.decode_adpcm(left), ae.decode_adpcm(right)), 48000, 2)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--elf", type=Path, default=MAIN / "config" / "SCUS_971.12")
    ap.add_argument("--extract", type=Path, default=MAIN / "extract")
    ap.add_argument("--iso", type=Path, help="disc image: also decode the orphan cues")
    ap.add_argument("--out", type=Path, default=MAIN / "build" / "cut_content" / "census")
    args = ap.parse_args(argv)
    args.out.mkdir(parents=True, exist_ok=True)
    report, voice, music, entries = census(args)
    if args.iso:
        check_slots(args, report, entries)
        export_audio(args, report, voice, music)
        print("banks at their loader slot: "
              f"{sum(1 for b in report['banks'].values() if b.get('loader_slot'))}/{len(report['banks'])}")
    (args.out / "census.json").write_text(json.dumps(report, indent=1))
    unreached = {k: v["unreached_lines"] for k, v in report["areas"].items() if v["unreached_lines"]}
    print(f"text banks paired: {sum(1 for b in report['banks'].values() if b['bank'])}/{len(report['banks'])}")
    print(f"voice cues: {report['voice']['cues']}, named by records {report['voice']['named_by_records']}, "
          f"unreferenced {[u['cue'] for u in report['voice']['unreferenced']]}")
    print(f"direct voice/stream starts: {len(report['voice']['direct_calls'])}")
    print(f"stream rows {len(report['streams']['rows'])}, started by a script "
          f"{sum(r['script_started'] for r in report['streams']['rows'])}; cutscene-range cues without a row "
          f"{[c['cue'] for c in report['streams']['cutscene_range_without_row']]}")
    print(f"lines no script reaches: {unreached}")
    print(f"non-ASCII lines: {[(n['table'], n['line']) for n in report['non_ascii_lines']]}")
    print(f"op14 spawn entries: {sum(map(len, report['spawn_entries'].values()))} "
          f"in {len(report['spawn_entries'])} overlays")
    print(f"wrote {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
