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
# Secondary opcode is in bits [10:6]; for the bc variants the bc is in [1:0].
# Key encodings (consolidated from public VU tables):
#   primary=0x3C secondary table:
UPPER_SPECIAL_3C = {
    0x00: 'addAbc',   # +bc in [1:0]
    0x01: 'subAbc',
    0x02: 'maddAbc',
    0x03: 'msubAbc',
    0x04: 'itof0',
    0x05: 'itof4',
    0x06: 'itof12',
    0x07: 'itof15',
    0x08: 'ftoi0',
    0x09: 'ftoi4',
    0x0a: 'ftoi12',
    0x0b: 'ftoi15',
    0x0c: 'mulAbc',
    0x0d: 'mulAq',
    0x0e: 'absA',     # rare
    0x0f: 'clip',     # CLIPw.xyz
}
UPPER_SPECIAL_3D = {
    0x00: 'addAq',
    0x01: 'maddAq',
    0x02: 'addAi',
    0x03: 'maddAi',
    0x04: 'subAq',
    0x05: 'msubAq',
    0x06: 'subAi',
    0x07: 'msubAi',
    0x08: 'addA',
    0x09: 'maddA',
    0x0a: 'mulA',
    0x0b: 'opmula',
    0x0c: 'subA',
    0x0d: 'msubA',
    0x0e: 'nop',
}
UPPER_SPECIAL_3E = {
    0x00: 'mulAbc',  # variant
    0x01: 'mulAbc',
    0x02: 'mulAbc',
    0x03: 'mulAbc',
    0x04: 'itof0',
    0x05: 'itof4',
    0x06: 'itof12',
    0x07: 'itof15',
    0x08: 'ftoi0',
    0x09: 'ftoi4',
    0x0a: 'ftoi12',
    0x0b: 'ftoi15',
    0x0c: 'mulAi',
    0x0d: 'abs',
    0x0e: 'mulAi',
    0x0f: 'clip',
}
UPPER_SPECIAL_3F = {
    0x00: 'maxi',
    0x01: 'minii',
    # remainder rare / unused on most kernels
}


def decode_upper(upper):
    if upper == 0x000002FF or upper == 0x800002FF:
        return ('nop', dest_mask(upper))
    op = upper & 0x3f
    dm = dest_mask(upper)
    # bc-style primaries
    if op in UPPER_BC_OPS:
        bc = upper & 0x3
        return (UPPER_BC_OPS[op] + BC[bc], dm)
    if op in UPPER_PRIMARY:
        return (UPPER_PRIMARY[op], dm)
    # Special groups
    if 0x3c <= op <= 0x3f:
        secondary = (upper >> 6) & 0x1f
        tables = {0x3c: UPPER_SPECIAL_3C, 0x3d: UPPER_SPECIAL_3D,
                  0x3e: UPPER_SPECIAL_3E, 0x3f: UPPER_SPECIAL_3F}
        name = tables[op].get(secondary, f'upp{op:02x}_{secondary:02x}')
        # bc suffix for the bc-form rows
        if name.endswith('bc'):
            bc = upper & 0x3
            name = name + BC[bc]
        return (name, dm)
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

# LOWER special: when top 7 bits == 0x40, the bottom 11 bits select one of
# many ops. The bottom 6 bits are the "group selector"; ops sharing a group
# disambiguate by bits [10:6].
# We tabulate by the full 11-bit fingerprint (subop = lower & 0x7FF).
# This is the master table — covers all common encodings we expect to hit.

LOWER_SPECIAL = {
    # group 0x30 — integer ALU
    0x030: 'iadd',
    0x031: 'isub',
    0x032: 'iaddi',
    0x034: 'iand',
    0x035: 'ior',
    # group 0x3C — extended LOWER1 (5-bit secondary in [10:6])
    # The 11-bit encoding is (secondary << 6) | 0x3C
    0x03c: 'move',
    0x07c: 'lqi',     # vlqi vfT, (vit++)
    0x0bc: 'div',
    0x0fc: 'mtir',
    0x13c: 'rnext',
    0x17c: 'sqi',     # vsqi vfS, (vit++)
    0x1bc: 'sqrt',
    0x1fc: 'mfir',    # also FCAND/etc in alt tables; use context
    0x23c: 'rget',
    0x27c: 'lqd',
    0x2bc: 'rsqrt',
    0x2fc: 'ilwr',
    0x33c: 'rinit',
    0x37c: 'sqd',
    0x3bc: 'waitq',
    0x3fc: 'iswr',
    0x43c: 'rxor',
    0x47c: 'mfp',
    0x4bc: 'xtop',
    0x4fc: 'xitop',
    # group 0x3D — EFU stuff (EE-side single-issue)
    0x73d: 'esadd',
    0x77d: 'ersadd',
    0x7bd: 'eleng',
    0x7fd: 'erleng',
    # 0x3E group:
    0x73e: 'eatanxy',
    0x77e: 'eatanxz',
    0x7be: 'esum',
    0x7fe: 'ercpr',
    # 0x3F group:
    0x73f: 'esqrt',
    0x77f: 'ersqrt',
    0x7bf: 'esin',
    0x7ff: 'eatan',
    0xbbf: 'eexp',
    # Misc
    0x3bf: 'waitp',
    # XGKICK — primary 0x40, fingerprint matches sub 0x6FC (encoded as
    # secondary 0x1B in the LOWER1 0x3C group: (0x1B << 6) | 0x3C = 0x6FC)
    0x6fc: 'xgkick',
    # MFP / extended placeholders — common but rare:
    0x7fc: 'mfp',
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
            return (name, f'  vf{ft:02d}.{dest}, {imm11}(vi{is_:02d})')
        if name == 'sq':
            return (name, f'  vf{ft:02d}.{dest}, {imm11}(vi{is_:02d})')
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
        if name.startswith('fc') or name.startswith('fs') or name.startswith('fm'):
            return (name, f'  vi{ft:02d}, 0x{lower & 0xffffff:06x}')
        return (name, '')
    if opc == 0x40:
        sub = lower & 0x7ff
        # XGKICK detection: the canonical PCSX2 encoding uses bits [10:6]==0x1B
        secondary = (lower >> 6) & 0x1f
        ft = _fld(lower, 20, 16)
        is_ = _fld(lower, 15, 11)
        dest = _DMASK_NAMES[_fld(lower, 24, 21)]
        # primary fingerprint table:
        nm = LOWER_SPECIAL.get(sub)
        if nm:
            # most LOWER_SPECIAL entries that take operands fall through
            # to the 0x3c-group block below; fast-path the unambiguous ones
            if nm == 'jr':
                return (nm, f'    vi{is_:02d}')
            if nm == 'jalr':
                return (nm, f'  vi{ft:02d}, vi{is_:02d}')
            if nm in ('waitp', 'waitq'):
                return (nm, '')
            return (nm, '')
        # Try secondary lookup for 0x3C-group ops we may have missed:
        if (sub & 0x3f) == 0x3c:
            # 0x3C ext group, secondary in [10:6]
            ext_map = {
                0x00: 'move', 0x01: 'lqi', 0x02: 'div', 0x03: 'mtir',
                0x04: 'rnext', 0x05: 'sqi', 0x06: 'sqrt', 0x07: 'mfir',
                0x08: 'rget', 0x09: 'lqd', 0x0a: 'rsqrt', 0x0b: 'ilwr',
                0x0c: 'rinit', 0x0d: 'sqd', 0x0e: 'waitq', 0x0f: 'iswr',
                0x10: 'rxor', 0x11: 'mfp', 0x12: 'xtop', 0x13: 'xitop',
                0x1a: 'mfp', 0x1b: 'xgkick', 0x1c: 'waitp',
            }
            nm2 = ext_map.get(secondary, f'low1_{secondary:02x}')
            if nm2 == 'lqi':
                return (nm2, f'   vf{ft:02d}.{dest}, (vi{is_:02d}++)')
            if nm2 == 'sqi':
                return (nm2, f'   vf{ft:02d}.{dest}, (vi{is_:02d}++)')
            if nm2 == 'lqd':
                return (nm2, f'   vf{ft:02d}.{dest}, (--vi{is_:02d})')
            if nm2 == 'sqd':
                return (nm2, f'   vf{ft:02d}.{dest}, (--vi{is_:02d})')
            if nm2 == 'mfir':
                return (nm2, f'  vf{ft:02d}.{dest}, vi{is_:02d}')
            if nm2 == 'mtir':
                return (nm2, f'  vi{ft:02d}, vf{is_:02d}.{dest}')
            if nm2 == 'ilwr':
                return (nm2, f'  vi{ft:02d}, (vi{is_:02d})')
            if nm2 == 'iswr':
                return (nm2, f'  vi{ft:02d}, (vi{is_:02d})')
            if nm2 == 'xtop':
                return (nm2, f'  vi{ft:02d}')
            if nm2 == 'xgkick':
                return (nm2, f' vi{is_:02d}')
            if nm2 == 'move':
                return (nm2, f'  vf{ft:02d}.{dest}, vf{is_:02d}')
            if nm2 == 'mfp':
                return (nm2, f'   vf{ft:02d}.{dest}')
            return (nm2, '')
        if (sub & 0x3f) == 0x3d:
            ext_map = {
                0x1c: 'esadd', 0x1d: 'ersadd', 0x1e: 'eleng', 0x1f: 'erleng',
            }
            return (ext_map.get(secondary, f'low2_{secondary:02x}'), '')
        if (sub & 0x3f) == 0x3e:
            ext_map = {
                0x1c: 'eatanxy', 0x1d: 'eatanxz', 0x1e: 'esum', 0x1f: 'ercpr',
            }
            return (ext_map.get(secondary, f'low3_{secondary:02x}'), '')
        if (sub & 0x3f) == 0x3f:
            ext_map = {
                0x1c: 'esqrt', 0x1d: 'ersqrt', 0x1e: 'esin', 0x1f: 'eatan',
            }
            return (ext_map.get(secondary, f'low4_{secondary:02x}'), '')
        # vcallms / vcallmsr
        if (sub & 0x3f) == 0x38:
            return ('vcallms', 'imm15')
        if (sub & 0x3f) == 0x39:
            return ('vcallmsr', '')
        # iadd/isub/etc
        if (sub & 0x3f) in (0x30, 0x31, 0x34, 0x35):
            return ({0x30: 'iadd', 0x31: 'isub', 0x34: 'iand',
                     0x35: 'ior'}[sub & 0x3f], '')
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
    i_pending = False
    for k in range(n):
        lower, upper = struct.unpack_from('<II', blob, k * 8)
        vram = base_vram + k * 8
        if i_pending:
            # Previous insn was I-bit set: this 64-bit word is a 32-bit float
            # immediate (loaded into I register). The immediate occupies the
            # upper word (it replaces the FP op).
            f = struct.unpack('<f', struct.pack('<I', upper))[0]
            out.append(f'  {vram:08x} {upper:08x}|{lower:08x}  '
                       f'<imm32 I = {f:g}>')
            i_pending = False
            continue
        un, ud = decode_upper(upper)
        ln, lops = decode_lower(lower, pc=vram)
        flags = flags_str(upper)
        out.append(f'  {vram:08x} {upper:08x}|{lower:08x}  [{flags}] '
                   f'U:{un + ud:18s}  L:{ln}{lops}')
        if upper & 0x80000000:
            i_pending = True
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
    i_pending = False
    for pv, bv, sz, dst in pkts:
        blob = read_elf_bytes(bv, sz)
        n = sz // 8
        total_qw += n
        for k in range(n):
            lower, upper = struct.unpack_from('<II', blob, k * 8)
            if i_pending:
                i_pending = False
                continue
            un, _ = decode_upper(upper)
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
            if upper & 0x80000000:
                i_pending = True
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
