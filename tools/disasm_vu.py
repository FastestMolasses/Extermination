#!/usr/bin/env python3
"""
disasm_vu.py — PS2 VU1 microcode disassembler + microcode scanner.

Each VU instruction is 64 bits / 8 bytes laid out little-endian:
  bytes +0..3 = LOWER pipe word (load/store, integer, branch, vcallms, ...)
  bytes +4..7 = UPPER pipe word (FP math: vadd, vsub, vmul, ...)
  Confirmed empirically: the E-bit (bit 30) end-of-program flag and the
  I/M/D/T flags live in the +4..7 word only. At vram 0x0023c780 the qw58
  word1 = 0x400002FF — that is UPPER NOP 0x000002FF with the E-bit set.
  At that same qw the LOWER word is 0x80006EFC = XGKICK (primary op 0x40,
  subop 0x6FC), confirming bytes 0..3 carry the LOWER pipe op.

UPPER pipe word top-bit flags:
  I (bit 31): immediate-load form (LOAD-imm32 follows in next slot)
  E (bit 30): end-of-program (set on the LAST instruction; one delay slot follows)
  M (bit 29): use macroflag
  D (bit 28): debug
  T (bit 27): T-bit

This decoder is sized to *classify* programs (not to round-trip every encoding
edge case); it identifies XGKICK, vector LQ/SQ with auto-inc/dec, accumulator
math (MULA/MADDA/etc.), and the common helpers (DIV/RSQRT/ABS/CLIP/ITOF/FTOI)
so a per-program op-frequency profile distinguishes a skinning kernel from a
particle/effect kernel.

References (no source paste): public ps2tek docs, PCSX2's VU disasm tables,
VuAssembler.

Three modes:
    disasm <vram> <size>     -- disassemble bytes at vram for size bytes
    catalog                  -- list every VIF1 MPG packet (microcode upload)
    profile                  -- per-program op-frequency table for all 48 progs
"""

import argparse
import os
import struct
import sys
from collections import defaultdict

ELF_PATH = os.path.join(os.path.dirname(__file__), '..', 'config', 'SCUS_971.12')

BC = ['x', 'y', 'z', 'w']
DEST_BITS = ['x', 'y', 'z', 'w']  # bit positions [24]=x [23]=y [22]=z [21]=w


def dest_mask(upper):
    """Return e.g. '.xyzw' or '.xy' from the dest field (bits 24..21)."""
    s = ''
    for i, c in enumerate(DEST_BITS):
        if upper & (1 << (24 - i)):
            s += c
    return '.' + s if s else ''


# =====================================================================
# UPPER pipe decoder
# =====================================================================
# Primary opcode is bits [5:0]. For 0x3C..0x3F, the secondary opcode lives
# in bits [10:6] combined with bits [1:0] (the 'special' subgroup).

# bc-style ops have primary op = 00,04,08,0C,10,14,18 with bc in bits [1:0]
UPPER_BC_OPS = {
    0x00: 'addbc',
    0x04: 'subbc',
    0x08: 'maddbc',
    0x0c: 'msubbc',
    0x10: 'maxbc',
    0x14: 'minibc',
    0x18: 'mulbc',
}

UPPER_PRIMARY = {
    0x1c: 'mulq',
    0x1d: 'maxi',
    0x1e: 'muli',
    0x1f: 'minii',
    0x20: 'addq',
    0x21: 'maddq',
    0x22: 'addi',
    0x23: 'maddi',
    0x24: 'subq',
    0x25: 'msubq',
    0x26: 'subi',
    0x27: 'msubi',
    0x28: 'add',
    0x29: 'madd',
    0x2a: 'mul',
    0x2b: 'max',
    0x2c: 'sub',
    0x2d: 'msub',
    0x2e: 'opmsub',
    0x2f: 'mini',
}

# UPPER "special" — primary opcode 0x3C/0x3D/0x3E/0x3F.
# Secondary opcode is bits [10:6]; primary&3 selects one of 4 FD tables.
# *** CORRECTED 2026-06-09 against PCSX2 pcsx2/VUops.cpp _vuTablesMess
# (VU{0,1}_UPPER_FD_{00,01,10,11}_TABLE[(code>>6)&0x1f]). The previous
# tables here were guesses and systematically WRONG (e.g. 3C/sec6 was
# called itof12 but is canonically MULAx) — every conclusion drawn from
# the old decode of ITOF/FTOI/MULA ops must be re-verified. ***
# Verified anchors in this binary: upper NOP 0x000002FF = 3F/secB = NOP;
# 0x01c529ff = 3F/sec7 = CLIP, sitting 4 qw before an fcand 0x3ffff.
UPPER_FD = {
    0x3c: {0x0: 'addAx', 0x1: 'subAx', 0x2: 'maddAx', 0x3: 'msubAx',
           0x4: 'itof0', 0x5: 'ftoi0', 0x6: 'mulAx', 0x7: 'mulAq',
           0x8: 'addAq', 0x9: 'subAq', 0xa: 'addA', 0xb: 'subA'},
    0x3d: {0x0: 'addAy', 0x1: 'subAy', 0x2: 'maddAy', 0x3: 'msubAy',
           0x4: 'itof4', 0x5: 'ftoi4', 0x6: 'mulAy', 0x7: 'abs',
           0x8: 'maddAq', 0x9: 'msubAq', 0xa: 'maddA', 0xb: 'msubA'},
    0x3e: {0x0: 'addAz', 0x1: 'subAz', 0x2: 'maddAz', 0x3: 'msubAz',
           0x4: 'itof12', 0x5: 'ftoi12', 0x6: 'mulAz', 0x7: 'mulAi',
           0x8: 'addAi', 0x9: 'subAi', 0xa: 'mulA', 0xb: 'opmula'},
    0x3f: {0x0: 'addAw', 0x1: 'subAw', 0x2: 'maddAw', 0x3: 'msubAw',
           0x4: 'itof15', 0x5: 'ftoi15', 0x6: 'mulAw', 0x7: 'clip',
           0x8: 'maddAi', 0x9: 'msubAi', 0xb: 'nop'},
}


def decode_upper(upper):
    if upper == 0x000002FF or upper == 0x800002FF:
        return ('nop', dest_mask(upper))
    op = upper & 0x3f
    dm = dest_mask(upper)
    # bc-style primaries: ops 0x00..0x1b are 7 bc-families x 4 bc lanes.
    # The table is keyed by the bc=x member; mask off the bc bits [1:0]
    # so addbc.y/z/w (op 0x01..0x03) etc. decode too.
    if op < 0x1c and (op & ~0x3) in UPPER_BC_OPS:
        bc = op & 0x3
        base = op & ~0x3
        ft = _fld(upper, 20, 16)
        fs = _fld(upper, 15, 11)
        fd = _fld(upper, 10, 6)
        return (f'{UPPER_BC_OPS[base]}{BC[bc]}',
                f'{dm} vf{fd:02d}, vf{fs:02d}, vf{ft:02d}{BC[bc]}')
    if op in UPPER_PRIMARY:
        ft = _fld(upper, 20, 16)
        fs = _fld(upper, 15, 11)
        fd = _fld(upper, 10, 6)
        nm = UPPER_PRIMARY[op]
        if nm.endswith(('q', 'i')) and nm not in ('maddi', 'msubi'):
            # q/i-register second operand forms: fd, fs only
            return (nm, f'{dm} vf{fd:02d}, vf{fs:02d}')
        return (nm, f'{dm} vf{fd:02d}, vf{fs:02d}, vf{ft:02d}')
    # Special groups (FD tables)
    if 0x3c <= op <= 0x3f:
        secondary = (upper >> 6) & 0x1f
        name = UPPER_FD[op].get(secondary, f'upp{op:02x}_{secondary:02x}')
        ft = _fld(upper, 20, 16)
        fs = _fld(upper, 15, 11)
        if name == 'nop':
            return ('nop', '')
        if name.startswith(('itof', 'ftoi', 'abs')):
            return (name, f'{dm} vf{ft:02d}, vf{fs:02d}')
        if name == 'clip':
            return (name, f' vf{fs:02d}.xyz, vf{ft:02d}w')
        # ACC ops; bc-form names end in x/y/z/w (addAx..mulAw)
        if name[-1] in 'xyzw' and name not in ('clip',):
            return (name, f'{dm} ACC, vf{fs:02d}, vf{ft:02d}{name[-1]}')
        return (name, f'{dm} ACC, vf{fs:02d}, vf{ft:02d}')
    return (f'upp_{op:02x}', dm)


# =====================================================================
# LOWER pipe decoder
# =====================================================================
# Two big groups:
#   - "regular" LOWER: top 7 bits [31:25] = primary opcode (lq, sq, ilw, branches, ...)
#   - "special" LOWER: top 7 bits == 0x40 -> sub-opcode in bits [10:0]
#       within that, last 6 bits select group; bits [10:6] select sub-sub.

LOWER_PRIMARY = {
    0x00: 'lq',
    0x01: 'sq',
    0x04: 'ilw',
    0x05: 'isw',
    0x08: 'iaddiu',
    0x09: 'isubiu',
    0x10: 'fceq',
    0x11: 'fcset',
    0x12: 'fcand',
    0x13: 'fcor',
    0x14: 'fseq',
    0x15: 'fsset',
    0x16: 'fsand',
    0x17: 'fsor',
    0x18: 'fmeq',
    0x1a: 'fmand',
    0x1b: 'fmor',
    0x1c: 'fcget',
    0x20: 'b',
    0x21: 'bal',
    0x24: 'jr',
    0x25: 'jalr',
    0x28: 'ibeq',
    0x29: 'ibne',
    0x2c: 'ibltz',
    0x2d: 'ibgtz',
    0x2e: 'iblez',
    0x2f: 'ibgez',
}

# LOWER special: when top 7 bits == 0x40, bits [5:0] select the group:
# 0x30..0x35 integer ALU, 0x3C..0x3F dispatch to a T3 table indexed by
# bits [10:6].
# *** CORRECTED 2026-06-09 against PCSX2 pcsx2/VUops.cpp _vuTablesMess
# (VU{0,1}LowerOP_T3_{00,01,10,11}_OPCODE[(code>>6)&0x1f]). The previous
# 11-bit fingerprint dict here was shifted (e.g. 0x3BC was called WAITQ
# but is canonically DIV; 0x6FC=XGKICK was the only correct anchor).
# Verified anchors in this binary: lower NOP 0x8000033C = T3_00[0xC] =
# MOVE vf00,vf00 (canonical pseudo-NOP); XGKICK sub 0x6FC = T3_00[0x1B].
LOWER_T3 = {
    0x3c: {0x0c: 'move', 0x0d: 'lqi', 0x0e: 'div', 0x0f: 'mtir',
           0x10: 'rnext', 0x19: 'mfp', 0x1a: 'xtop', 0x1b: 'xgkick',
           0x1c: 'esadd', 0x1d: 'eatanxy', 0x1e: 'esqrt', 0x1f: 'esin'},
    0x3d: {0x0c: 'mr32', 0x0d: 'sqi', 0x0e: 'sqrt', 0x0f: 'mfir',
           0x10: 'rget', 0x1a: 'xitop',
           0x1c: 'ersadd', 0x1d: 'eatanxz', 0x1e: 'ersqrt', 0x1f: 'eatan'},
    0x3e: {0x0d: 'lqd', 0x0e: 'rsqrt', 0x0f: 'ilwr', 0x10: 'rinit',
           0x1c: 'eleng', 0x1d: 'esum', 0x1e: 'ercpr', 0x1f: 'eexp'},
    0x3f: {0x0d: 'sqd', 0x0e: 'waitq', 0x0f: 'iswr', 0x10: 'rxor',
           0x1c: 'erleng', 0x1e: 'waitp'},
}

# VCALLMS / VCALLMSR / BAL / B operate on primary-op group too, but their
# imm15 target needs decoding. Handled in decode_lower below.

LOWER1_GROUP_BASE = 0x3C  # 6-bit base of all "vector misc" ops


_DMASK_NAMES = ['', 'w', 'z', 'zw', 'y', 'yw', 'yz', 'yzw',
                'x', 'xw', 'xz', 'xzw', 'xy', 'xyw', 'xyz', 'xyzw']


def _s11(x):
    """Sign-extend the 11-bit LOWER immediate (range -1024..+1023)."""
    x &= 0x7ff
    return x - 0x800 if x & 0x400 else x


def _fld(w, hi, lo):
    return (w >> lo) & ((1 << (hi - lo + 1)) - 1)


def decode_lower(lower, pc=None):
    """Decode a 32-bit LOWER-pipe word into (mnemonic, operand_string).

    *** Field-position convention (CORRECTED 2026-05-27) ***
    Empirical decode of the per-bone skinner main at vram 0x00234610
    shows that on this binary the integer-register field lies at
    bits [15:11] and the vector-register field at bits [20:16] for
    LOWER load/store/branch ops — the OPPOSITE of the PCSX2-source
    convention. Under PCSX2's mapping (FT=[15:11], IS=[20:16]) every
    bone-matrix and constant-table load decoded as `vf00` (zero
    register); swapping the fields produced sensible vf28..vf31
    bone-matrix loads, matching the documented kernel pattern.
    """
    if lower == 0x8000033C:
        return ('nop', '')
    opc = (lower >> 25) & 0x7f
    name = LOWER_PRIMARY.get(opc)
    if name:
        ft = _fld(lower, 20, 16)   # vector reg (FT)
        is_ = _fld(lower, 15, 11)  # integer reg (IS) — swapped vs. PCSX2
        dest = _DMASK_NAMES[_fld(lower, 24, 21)]
        imm11 = _s11(lower)
        if name == 'lq':
            # LQ vfFt, imm11(viIs): dest = ft [20:16], base = is [15:11]
            return (name, f'  vf{ft:02d}.{dest}, {imm11}(vi{is_:02d})')
        if name == 'sq':
            # SQ vfFs, imm11(viIt): SOURCE = fs [15:11], base = it [20:16]
            # (asymmetric vs LQ — per PCSX2 _vuSQ; corrected 2026-06-09)
            return (name, f'  vf{is_:02d}.{dest}, {imm11}(vi{ft:02d})')
        if name in ('ilw', 'isw'):
            return (name, f'  vi{ft:02d}, {imm11}(vi{is_:02d})')
        if name in ('iaddiu', 'isubiu'):
            imm15 = _fld(lower, 10, 0) | (_fld(lower, 24, 21) << 11)
            return (name, f' vi{ft:02d}, vi{is_:02d}, 0x{imm15:04x}')
        if name in ('ibeq', 'ibne'):
            return (name, f'  vi{ft:02d}, vi{is_:02d}, {imm11*8:+d}')
        if name in ('ibltz', 'ibgtz', 'iblez', 'ibgez'):
            return (name, f' vi{is_:02d}, {imm11*8:+d}')
        if name in ('b', 'bal'):
            tgt = f' vi{ft:02d},' if name == 'bal' else ''
            return (name, f'{tgt} {imm11*8:+d}')
        if name == 'jr':
            return (name, f'    vi{is_:02d}')
        if name == 'jalr':
            return (name, f'  vi{ft:02d}, vi{is_:02d}')
        if name.startswith('fc') or name.startswith('fs') or name.startswith('fm'):
            return (name, f'  vi{ft:02d}, 0x{lower & 0xffffff:06x}')
        return (name, '')
    if opc == 0x40:
        sub = lower & 0x7ff
        secondary = (lower >> 6) & 0x1f
        ft = _fld(lower, 20, 16)
        is_ = _fld(lower, 15, 11)
        dest = _DMASK_NAMES[_fld(lower, 24, 21)]
        fsf = _fld(lower, 22, 21)
        ftf = _fld(lower, 24, 23)
        grp = sub & 0x3f
        if grp in LOWER_T3:
            nm2 = LOWER_T3[grp].get(
                secondary, f'lowT3_{grp:02x}_{secondary:02x}')
            if nm2 == 'lqi':
                return (nm2, f'   vf{ft:02d}.{dest}, (vi{is_:02d}++)')
            if nm2 == 'sqi':
                # SQI vfFs, (viIt++) — source fs, base it (like SQ)
                return (nm2, f'   vf{is_:02d}.{dest}, (vi{ft:02d}++)')
            if nm2 == 'lqd':
                return (nm2, f'   vf{ft:02d}.{dest}, (--vi{is_:02d})')
            if nm2 == 'sqd':
                return (nm2, f'   vf{is_:02d}.{dest}, (--vi{ft:02d})')
            if nm2 == 'div':
                return (nm2, f'   Q, vf{is_:02d}.{BC[fsf]}, '
                             f'vf{ft:02d}.{BC[ftf]}')
            if nm2 == 'sqrt':
                return (nm2, f'  Q, vf{ft:02d}.{BC[ftf]}')
            if nm2 == 'rsqrt':
                return (nm2, f' Q, vf{is_:02d}.{BC[fsf]}, '
                             f'vf{ft:02d}.{BC[ftf]}')
            if nm2 in ('eleng', 'erleng', 'esadd', 'ersadd', 'esum'):
                return (nm2, f' P, vf{is_:02d}')
            if nm2 in ('esqrt', 'ersqrt', 'esin', 'eatan', 'ercpr',
                       'eexp', 'eatanxy', 'eatanxz'):
                return (nm2, f' P, vf{is_:02d}.{BC[fsf]}')
            if nm2 == 'mr32':
                return (nm2, f'  vf{ft:02d}.{dest}, vf{is_:02d}')
            if nm2 == 'mfir':
                return (nm2, f'  vf{ft:02d}.{dest}, vi{is_:02d}')
            if nm2 == 'mtir':
                return (nm2, f'  vi{ft:02d}, vf{is_:02d}.{BC[fsf]}')
            if nm2 == 'ilwr':
                return (nm2, f'  vi{ft:02d}.{dest}, (vi{is_:02d})')
            if nm2 == 'iswr':
                return (nm2, f'  vi{ft:02d}.{dest}, (vi{is_:02d})')
            if nm2 in ('xtop', 'xitop'):
                return (nm2, f'  vi{ft:02d}')
            if nm2 == 'xgkick':
                return (nm2, f' vi{is_:02d}')
            if nm2 == 'move':
                if ft == 0 and is_ == 0:
                    return ('nop', '')
                return (nm2, f'  vf{ft:02d}.{dest}, vf{is_:02d}')
            if nm2 == 'mfp':
                return (nm2, f'   vf{ft:02d}.{dest}, P')
            return (nm2, '')
        # vcallms / vcallmsr
        if (sub & 0x3f) == 0x38:
            return ('vcallms', 'imm15')
        if (sub & 0x3f) == 0x39:
            return ('vcallmsr', '')
        # iaddi it, is, imm5 (imm5 signed, bits [10:6])
        if (sub & 0x3f) == 0x32:
            imm5 = _fld(lower, 10, 6)
            if imm5 & 0x10:
                imm5 -= 32
            return ('iaddi', f' vi{ft:02d}, vi{is_:02d}, {imm5:+d}')
        # iadd/isub/iand/ior id, is, it  (id in [10:6])
        if (sub & 0x3f) in (0x30, 0x31, 0x34, 0x35):
            id_ = _fld(lower, 10, 6)
            nm3 = {0x30: 'iadd', 0x31: 'isub', 0x34: 'iand',
                   0x35: 'ior'}[sub & 0x3f]
            return (nm3, f'  vi{id_:02d}, vi{is_:02d}, vi{ft:02d}')
        return (f'lspec_{sub:03x}', '')
    return (f'low_{lower:08x}', '')


# =====================================================================
# Display helpers
# =====================================================================

def flags_str(upper):
    i = (upper >> 31) & 1
    e = (upper >> 30) & 1
    m = (upper >> 29) & 1
    d = (upper >> 28) & 1
    t = (upper >> 27) & 1
    return ''.join(c if v else '.' for c, v in
                   [('I', i), ('E', e), ('M', m), ('D', d), ('T', t)])


def disasm_block(blob, base_vram):
    out = []
    n = len(blob) // 8
    for k in range(n):
        lower, upper = struct.unpack_from('<II', blob, k * 8)
        vram = base_vram + k * 8
        un, ud = decode_upper(upper)
        flags = flags_str(upper)
        if upper & 0x80000000:
            # I-bit: the LOWER word of THIS pair is a 32-bit float
            # immediate loaded into the I register (same-slot, per VU
            # spec — corrected 2026-06-09; previously misread as
            # occupying the following pair).
            f = struct.unpack('<f', struct.pack('<I', lower))[0]
            out.append(f'  {vram:08x} {upper:08x}|{lower:08x}  [{flags}] '
                       f'U:{un + ud:18s}  L:<I = {f:g} (0x{lower:08x})>')
        else:
            ln, lops = decode_lower(lower, pc=vram)
            out.append(f'  {vram:08x} {upper:08x}|{lower:08x}  [{flags}] '
                       f'U:{un + ud:18s}  L:{ln}{lops}')
        if upper & 0x40000000:
            out.append('    -- [E] end-of-program (one delay slot follows)')
    return '\n'.join(out)


# =====================================================================
# ELF I/O
# =====================================================================

def read_elf():
    with open(ELF_PATH, 'rb') as f:
        return f.read()


def vram_to_foff(vram):
    return vram - 0x00100000 + 0x300


def read_elf_bytes(vram, size):
    data = read_elf()
    foff = vram_to_foff(vram)
    return data[foff:foff + size]


# =====================================================================
# Commands
# =====================================================================

def cmd_disasm(args):
    vram = int(args.vram, 0)
    size = int(args.size, 0)
    blob = read_elf_bytes(vram, size)
    print(f'# VU1 disasm  vram=0x{vram:08x} size=0x{size:x}')
    print(disasm_block(blob, vram))


def list_packets():
    """Return list of (packet_vram, body_vram, body_size, imem_dst)."""
    data = read_elf()
    start_vram = 0x00230000
    end_vram = 0x00275B00
    foff_start = vram_to_foff(start_vram)
    foff_end = vram_to_foff(end_vram)
    blob = data[foff_start:foff_end]
    pkts = []
    for i in range(0, len(blob) - 16, 4):
        w = struct.unpack_from('<I', blob, i)[0]
        cmd = (w >> 24) & 0x7F
        addr = w & 0xFFFF
        if cmd != 0x4A or addr >= 0x4000:
            continue
        num = (w >> 16) & 0xFF
        qw = 256 if num == 0 else num
        sz = qw * 8
        body_vram = start_vram + i + 4
        pkts.append((start_vram + i, body_vram, sz, addr))
    return pkts


def cmd_catalog(args):
    print(f'# vram_packet  size  imem_dst  microcode_vram_start  microcode_vram_end')
    pkts = list_packets()
    for pv, bv, sz, dst in pkts:
        print(f'  0x{pv:08x}  0x{sz:04x}  0x{dst:04x}    '
              f'0x{bv:08x}  0x{bv+sz:08x}  ({sz//8} qw)')
    print(f'# {len(pkts)} microcode programs total')


# Group multi-MPG uploads into logical kernels. Heuristic: an upload chain
# is a run of consecutive packets where each subsequent imem_dst > prev_dst
# (i.e. continuation segments at 0x100, 0x200, ...). A new kernel starts at
# every imem_dst == 0x0000 OR when imem_dst <= prev_dst.
def group_kernels(pkts):
    kernels = []
    cur = []
    for p in pkts:
        if cur and p[3] <= cur[-1][3]:
            kernels.append(cur)
            cur = [p]
        else:
            cur.append(p)
    if cur:
        kernels.append(cur)
    return kernels


def profile_kernel(pkts):
    """Disassemble a kernel (one or more contiguous packets) and return op
    frequency counts."""
    counts = defaultdict(int)
    total_qw = 0
    for pv, bv, sz, dst in pkts:
        blob = read_elf_bytes(bv, sz)
        n = sz // 8
        total_qw += n
        for k in range(n):
            lower, upper = struct.unpack_from('<II', blob, k * 8)
            un, _ = decode_upper(upper)
            if upper & 0x80000000:
                ln = 'nop'  # I-bit: lower word is the imm32, not an op
            else:
                ln, _ = decode_lower(lower)
            # Family bucketing
            if un != 'nop':
                if 'madd' in un.lower():
                    counts['UPPER:madd*'] += 1
                elif 'msub' in un.lower():
                    counts['UPPER:msub*'] += 1
                elif un.startswith('mul'):
                    counts['UPPER:mul*'] += 1
                elif un.startswith('add'):
                    counts['UPPER:add*'] += 1
                elif un.startswith('sub'):
                    counts['UPPER:sub*'] += 1
                elif un.startswith(('ftoi', 'itof')):
                    counts['UPPER:ftoi/itof'] += 1
                elif un == 'clip':
                    counts['UPPER:clip'] += 1
                elif un.startswith(('max', 'mini')):
                    counts['UPPER:max/mini'] += 1
                else:
                    counts['UPPER:other'] += 1
            else:
                counts['UPPER:nop'] += 1
            if ln != 'nop':
                if ln == 'xgkick':
                    counts['LOWER:XGKICK'] += 1
                elif ln in ('lq', 'lqi', 'lqd'):
                    counts['LOWER:LQ*'] += 1
                elif ln in ('sq', 'sqi', 'sqd'):
                    counts['LOWER:SQ*'] += 1
                elif ln in ('div', 'sqrt', 'rsqrt'):
                    counts['LOWER:div/sqrt'] += 1
                elif ln in ('iaddiu', 'iaddi', 'iadd', 'isub', 'isubiu',
                            'iand', 'ior'):
                    counts['LOWER:int-alu'] += 1
                elif ln.startswith(('ib', 'b', 'bal', 'jr', 'jalr')):
                    counts['LOWER:branch'] += 1
                elif ln == 'vcallms' or ln == 'vcallmsr':
                    counts['LOWER:vcallms'] += 1
                elif ln in ('mtir', 'mfir', 'ilw', 'ilwr', 'isw', 'iswr'):
                    counts['LOWER:int-mem/move'] += 1
                elif ln == 'waitp' or ln == 'waitq':
                    counts['LOWER:wait'] += 1
                else:
                    counts['LOWER:other'] += 1
            else:
                counts['LOWER:nop'] += 1
    return total_qw, counts


def cmd_profile(args):
    pkts = list_packets()
    kernels = group_kernels(pkts)
    # Column order
    cols = ['UPPER:mul*', 'UPPER:madd*', 'UPPER:msub*', 'UPPER:add*',
            'UPPER:sub*', 'UPPER:ftoi/itof', 'UPPER:clip', 'UPPER:max/mini',
            'UPPER:other', 'LOWER:XGKICK', 'LOWER:LQ*', 'LOWER:SQ*',
            'LOWER:div/sqrt', 'LOWER:int-alu', 'LOWER:int-mem/move',
            'LOWER:branch', 'LOWER:vcallms', 'LOWER:wait', 'LOWER:other']
    hdr = ['#kern', 'pkts', 'segs', 'imem', 'qw'] + [c.split(':')[1] for c in cols]
    print(' '.join(f'{h:>10}' for h in hdr))
    for ki, kpkts in enumerate(kernels):
        total_qw, counts = profile_kernel(kpkts)
        imem0 = kpkts[0][3]
        n_segs = len(kpkts)
        row = [str(ki), f'{kpkts[0][0]:08x}', str(n_segs),
               f'{imem0:04x}', str(total_qw)]
        row += [str(counts.get(c, 0)) for c in cols]
        print(' '.join(f'{v:>10}' for v in row))


def cmd_scan(args):
    # Legacy scanner — kept for backward-compat
    cmd_catalog(args)


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest='cmd', required=True)
    d = sub.add_parser('disasm')
    d.add_argument('vram')
    d.add_argument('size')
    d.set_defaults(func=cmd_disasm)
    sub.add_parser('catalog').set_defaults(func=cmd_catalog)
    sub.add_parser('scan').set_defaults(func=cmd_scan)
    sub.add_parser('profile').set_defaults(func=cmd_profile)
    args = p.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
