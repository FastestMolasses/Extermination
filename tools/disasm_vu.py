#!/usr/bin/env python3
"""
disasm_vu.py — partial PS2 VU1 microcode disassembler + microcode scanner.

Each VU instruction is 64 bits / 8 bytes:
  bytes +0..3 = UPPER 32 bits = LOWER pipe op (load/store, integer, branch, vcallms, ...)
  bytes +4..7 = LOWER 32 bits = UPPER pipe op (FP math: vadd, vsub, vmul, ...)
  (i.e. file layout has lower-pipe word FIRST, then upper-pipe word — confirmed
  empirically: 0x000002FF appears in second-word slot = UPPER NOP; 0x8000033C
  appears in first-word slot = LOWER NOP.)
plus 4 bit flags packed in the top of the upper word:
  I (bit 31): immediate-load form (LOAD-imm32 follows in next slot)
  E (bit 30): end-of-program (set on the LAST instruction; one delay slot follows)
  M (bit 29): use macroflag
  D (bit 28): debug
  T (bit 27): T-bit

This decoder is INTENTIONALLY PARTIAL — it identifies enough opcodes to
characterize a program's shape (loads, stores, FP math, vcallms, branches,
xgkick, end-of-program) without reproducing every encoding edge case.

References used (no source paste): public ps2tek docs; PCSX2 emulator
disassembler tables; openpsy/VuAssembler opcode tables.

Two modes:
    disasm <vram> <size>     -- disassemble bytes at vram for size bytes
    scan                     -- scan the ELF data region for microcode-shaped
                                blocks (low E-bit density, ending with E-bit
                                then a non-instruction transition)

Bytes are read from the user's local boot ELF at config/SCUS_971.12 — never
committed. Output goes to stdout; capture to /tmp/ for analysis.
"""

import argparse
import os
import struct
import sys

ELF_PATH = os.path.join(os.path.dirname(__file__), '..', 'config', 'SCUS_971.12')

# ---------- LOWER pipe ----------
# Bottom 11 bits select the op (within a coarse opcode group).
# Reduced to high-value cases for characterizing programs.

LOWER_OPC = {
    # opcode field [31:25] = top bits of LOWER word
    0x00: 'LQ',     # lq vfT, imm(vis)
    0x01: 'SQ',     # sq vfS, imm(vit)
    0x04: 'ILW',    # integer load word
    0x05: 'ISW',
    0x08: 'IADDIU',
    0x09: 'ISUBIU',
    0x10: 'FCEQ',
    0x11: 'FCSET',
    0x12: 'FCAND',
    0x13: 'FCOR',
    0x14: 'FSEQ',
    0x15: 'FSSET',
    0x16: 'FSAND',
    0x17: 'FSOR',
    0x18: 'FMEQ',
    0x1a: 'FMAND',
    0x1b: 'FMOR',
    0x1c: 'FCGET',
    0x20: 'B',
    0x21: 'BAL',
    0x24: 'JR',
    0x25: 'JALR',
    0x28: 'IBEQ',
    0x29: 'IBNE',
    0x2c: 'IBLTZ',
    0x2d: 'IBGTZ',
    0x2e: 'IBLEZ',
    0x2f: 'IBGEZ',
}

# Special LOWER ops (opc field == 0x40): selected by bits [10:6] (sub-opcode 11 bits actually)
LOWER_SPECIAL = {
    0x30: 'IADD',
    0x31: 'ISUB',
    0x32: 'IADDI',
    0x34: 'IAND',
    0x35: 'IOR',
    # Sub-table when bits [5:0] == 0x3c (extended lower ops)
}

# Extended lower (subop field at low bits) — these are the "interesting" ones:
# When LOWER opc == 0x40 and the lowest 6 bits form a secondary opcode
LOWER_EXT = {
    # Heavily abbreviated mapping of common LOWER ops by 11-bit fingerprint
    0x030: 'MOVE',
    0x031: 'LQI',     # vlqi vfT, (vit++)  -- load + post-inc
    0x032: 'DIV',
    0x033: 'MTIR',
    0x034: 'RNEXT',
    0x035: 'SQI',     # vsqi vfS, (vit++)  -- store + post-inc
    0x036: 'SQRT',
    0x037: 'MFIR',
    0x038: 'RGET',
    0x039: 'LQD',     # pre-dec load
    0x03a: 'RSQRT',
    0x03b: 'ILWR',
    0x03c: 'RINIT',
    0x03d: 'SQD',     # pre-dec store
    0x03e: 'WAITQ',
    0x03f: 'ISWR',
    0x040: 'RXOR',
    0x041: 'MFP',
    0x042: 'XTOP',
    0x043: 'XITOP',
    0x6c4: 'XGKICK',  # *** key for GIF emit ***
    0x06c: 'ESADD',
    0x070: 'ERSADD',
    0x074: 'ELENG',
    0x078: 'ERLENG',
    0x07c: 'EATANxy',
    0x0fc: 'WAITP',
    0x1fc: 'ESIN',
    0x3fc: 'ERCPR',
    0x1ff: 'NOP_LOWER',
}

# Pseudo opcodes for the upper-word "flags + opcode"
UPPER_OPC_SIMPLE = {
    # bits 5..0
    0x00: 'ADDbc',  # ADDbc with bc=0..3 selecting x/y/z/w
    0x04: 'SUBbc',
    0x08: 'MADDbc',
    0x0c: 'MSUBbc',
    0x10: 'MAXbc',
    0x14: 'MINIbc',
    0x18: 'MULbc',
    0x1c: 'MULq',
    0x1d: 'MAXi',
    0x1e: 'MULi',
    0x1f: 'MINIi',
    0x20: 'ADDq',
    0x21: 'MADDq',
    0x22: 'ADDi',
    0x23: 'MADDi',
    0x24: 'SUBq',
    0x25: 'MSUBq',
    0x26: 'SUBi',
    0x27: 'MSUBi',
    0x28: 'ADD',
    0x29: 'MADD',
    0x2a: 'MUL',
    0x2b: 'MAX',
    0x2c: 'SUB',
    0x2d: 'MSUB',
    0x2e: 'OPMSUB',
    0x2f: 'MINI',
    0x3c: 'EXT',   # extended UPPER op
}


def decode_upper(upper: int) -> str:
    # NOP fingerprint
    if upper == 0x000002FF:
        return 'nop'
    e = (upper >> 30) & 1
    i_flag = (upper >> 31) & 1
    m = (upper >> 29) & 1
    d = (upper >> 28) & 1
    t = (upper >> 27) & 1
    op = upper & 0x3f
    flags = ''.join(c if v else '.' for c, v in [('I', i_flag), ('E', e), ('M', m), ('D', d), ('T', t)])
    name = UPPER_OPC_SIMPLE.get(op, f'UPP_{op:02x}')
    if op == 0x3c:
        # extended upper subop in bits [10:6]
        ext = (upper >> 6) & 0x1f
        # mapping subset
        ext_map = {
            0x00: 'ITOF0', 0x04: 'FTOI0',
            0x01: 'ITOF4', 0x05: 'FTOI4',
            0x02: 'ITOF12', 0x06: 'FTOI12',
            0x03: 'ITOF15', 0x07: 'FTOI15',
            0x08: 'MULAbc', 0x0a: 'ABS',
            0x0c: 'MULAq', 0x0d: 'ADDAi', 0x0e: 'SUBAq', 0x0f: 'MSUBAi',
            0x10: 'ADDAbc', 0x14: 'SUBAbc', 0x18: 'MADDAbc', 0x1c: 'MSUBAbc',
            0x1d: 'NOP',  # NOP_UPPER fingerprint when full word == 0x800002FF
        }
        name = ext_map.get(ext, f'EXT_{ext:02x}')
    return f'[{flags}] {name}'


def decode_lower(lower: int) -> str:
    # NOP fingerprint: 0x8000033C is the typical "lower NOP"
    if lower == 0x8000033C:
        return 'nop'
    opc = (lower >> 25) & 0x7f
    name = LOWER_OPC.get(opc)
    if name:
        return name
    if opc == 0x40:
        sub = lower & 0x7ff
        ext_name = LOWER_EXT.get(sub)
        if ext_name:
            return ext_name
    return f'LOW_{lower:08x}'


def disasm_block(blob: bytes, base_vram: int) -> str:
    out = []
    for i in range(0, len(blob) - 7, 8):
        # Memory layout: bytes +0..3 = LOWER pipe word, bytes +4..7 = UPPER pipe word
        lower, upper = struct.unpack_from('<II', blob, i)
        vram = base_vram + i
        out.append(f'  {vram:08x} {upper:08x}|{lower:08x}  '
                   f'U:{decode_upper(upper):24s}  L:{decode_lower(lower)}')
        if upper & 0x40000000:
            out.append('    -- [E] end-of-program (one delay slot follows)')
    return '\n'.join(out)


def read_elf_bytes(vram: int, size: int) -> bytes:
    with open(ELF_PATH, 'rb') as f:
        data = f.read()
    foff = vram - 0x00100000 + 0x300
    return data[foff:foff + size]


def cmd_disasm(args):
    vram = int(args.vram, 0)
    size = int(args.size, 0)
    blob = read_elf_bytes(vram, size)
    print(f'# VU1 disasm  vram=0x{vram:08x} size=0x{size:x}')
    print(disasm_block(blob, vram))


def cmd_scan(args):
    """Scan the ELF data region for plausible VU1 microcode blocks.

    Heuristic: a microcode program is a run of N >= 32 quadwords ending with
    exactly one E-bit-set instruction near the end (last 1-3 qwords), with no
    other E-bits in the body, and with the LOWER word of the last 1-2
    instructions plausibly == nop (0x8000033C) or a vcallms-style end.
    """
    with open(ELF_PATH, 'rb') as f:
        data = f.read()

    # Restrict scan to the data half of the PROGBITS section (after main code).
    # Conservative: start at vram 0x00230000 (post-game-code, into data tables).
    start_vram = 0x00230000
    end_vram = 0x00275B00
    foff_start = start_vram - 0x00100000 + 0x300
    foff_end = end_vram - 0x00100000 + 0x300
    blob = data[foff_start:foff_end]

    # Find every E-bit-set qword
    es = []
    for i in range(0, len(blob) - 8, 8):
        # UPPER word at +4
        upper = struct.unpack_from('<I', blob, i + 4)[0]
        if upper & 0x40000000:
            es.append(i)

    # Find candidate programs: an E-bit at index k where the previous E-bit
    # was at index k - 8*N for N >= 32, with no body E-bits in between.
    # i.e. consecutive E-bit gaps that are >= 32 qwords.
    candidates = []
    last = -8
    for e in es:
        gap_qw = (e - last) // 8
        if 32 <= gap_qw <= 2000:
            prog_start = last + 8
            prog_end = e + 8 + 8  # include the E-bit insn + one delay slot
            candidates.append((prog_start, prog_end, gap_qw))
        last = e

    print(f'# scanned vram 0x{start_vram:08x}..0x{end_vram:08x} '
          f'({len(blob)} bytes, {len(es)} E-bit qwords)')
    print(f'# {len(candidates)} candidate microcode programs:')
    for s, e, n in candidates:
        vram = start_vram + s
        size = e - s
        # Quick sanity: do the first 4 instructions look like VU ops?
        # We accept any whose first lower-word == 0x8000033C (nop_lower) and
        # whose first upper has a recognized UPPER opcode.
        first_lower, first_upper = struct.unpack_from('<II', blob, s)
        print(f'  vram=0x{vram:08x} size=0x{size:x} ({n} qw)  '
              f'first: U=0x{first_upper:08x} L=0x{first_lower:08x}')


def cmd_catalog(args):
    """List every VIF1 MPG packet in the ELF data region — these are the
    static microcode upload sites. Each MPG tag is preceded by 4 VIF1 setup
    words (STMOD/FLUSHE/STCYCL/STMASK template) and followed immediately by
    the microcode body (num*8 bytes).
    """
    with open(ELF_PATH, 'rb') as f:
        data = f.read()
    start_vram = 0x00230000
    end_vram = 0x00275B00
    foff_start = start_vram - 0x00100000 + 0x300
    foff_end = end_vram - 0x00100000 + 0x300
    blob = data[foff_start:foff_end]
    print(f'# vram_packet  size  imem_dst  microcode_vram_start  microcode_vram_end')
    n = 0
    for i in range(0, len(blob) - 16, 4):
        w = struct.unpack_from('<I', blob, i)[0]
        cmd = (w >> 24) & 0x7F
        addr = w & 0xFFFF
        if cmd != 0x4A or addr >= 0x4000:
            continue
        num = (w >> 16) & 0xFF
        qw = 256 if num == 0 else num
        sz = qw * 8
        mc_start = start_vram + i + 4
        mc_end = mc_start + sz
        print(f'  0x{start_vram+i:08x}  0x{sz:04x}  0x{addr:04x}    0x{mc_start:08x}  '
              f'0x{mc_end:08x}  ({qw} qw)')
        n += 1
    print(f'# {n} microcode programs total')


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest='cmd', required=True)
    d = sub.add_parser('disasm', help='disassemble bytes at vram')
    d.add_argument('vram')
    d.add_argument('size')
    d.set_defaults(func=cmd_disasm)
    s = sub.add_parser('scan', help='scan ELF data region for microcode blocks (heuristic)')
    s.set_defaults(func=cmd_scan)
    c = sub.add_parser('catalog', help='list all VIF1 MPG packets / microcode upload sites')
    c.set_defaults(func=cmd_catalog)
    args = p.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
