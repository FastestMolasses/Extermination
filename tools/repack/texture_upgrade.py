"""Closed title-screen texture layout and fresh-ELF TEX0 upgrades.

The title module owns only GS bytes 0x2a0000..0x300000. Repacking within
that footprint avoids inventing a free global VRAM arena. The title's ten
views are consumed by 001AC7F0; 00207E40 derives UVs from TEX0 TW/TH, while
the screen rectangle stays fixed. Other startup modules load their own
overlapping atlas and use different constants, so they are not patched.
"""
from __future__ import annotations

from collections import Counter
import hashlib
import json
import os
from pathlib import Path
import struct
import tempfile

from . import png, textures
from .archive import safe_output

SCHEMA = 'extermination-texture-upgrades-v1'
PROFILE = 'title-menu-v1'
DESCRIPTOR = 'texture-upgrades.json'
LEAF = 'archive/chunk01/transient00.bin'
NAMES = ('background-top-left', 'background-top-right', 'background-bottom-left',
         'background-bottom-right', 'new-game-selected', 'load-game', 'options',
         'new-game', 'load-game-selected', 'options-selected')
TOKENS = tuple(textures.TITLE) + tuple(dict.fromkeys(t for row in textures.MENU for t in row))
FUNCTION = 0x1AC7F0
FUNCTION_SIZE = 0x22C
FUNCTION_SHA256 = 'b66fc015e5e4a0d36c5b9ccceffc656e8b6110335470a4e5282b394c613a9ff3'
BASE, END = 0x2A0000, 0x300000


def _hash(data):
    return hashlib.sha256(data).hexdigest()


def _json(path):
    def unique(pairs):
        result = {}
        for key, value in pairs:
            if key in result:
                raise ValueError(f'duplicate texture JSON key: {key}')
            result[key] = value
        return result
    return json.loads(Path(path).read_text(), object_pairs_hook=unique)


def _atomic(path: Path, data: bytes):
    if Path(path).is_symlink():
        raise ValueError('texture upgrade output may not be a symlink')
    path = safe_output(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, tmp = tempfile.mkstemp(prefix='.texture-upgrade-', dir=path.parent)
    try:
        with os.fdopen(fd, 'wb') as stream:
            stream.write(data)
        os.replace(tmp, path)  # detach hardlinks rather than mutating other trees
    finally:
        if os.path.exists(tmp):
            os.unlink(tmp)


def _leaf(tree):
    tree = safe_output(tree)
    path = tree / LEAF
    if path.is_symlink() or not path.resolve().is_relative_to(tree) or not path.is_file():
        raise ValueError('title upload must be an ordinary file within the disc loose tree')
    return tree, path


def _upload(raw):
    uploads = textures._uploads(raw)
    expected = dict(number=0, bitblt_offset=48, payload_offset=128,
                    payload_size=END-BASE, width=256, height=384, dbp=BASE//256, dbw=4)
    if uploads != [expected]:
        raise ValueError('title-menu-v1 requires its complete original 384KiB PSMCT32 upload footprint')
    qwc = (END - BASE) // 16
    if (struct.unpack_from('<8I', raw) != (6 << 28 | (qwc+7), 0, 0, 0, 0, 0, 0x10000000, 0x50000000 | (qwc+6))
            or struct.unpack_from('<QQ', raw, 32) != (0x1000000000000004, 14)
            or any(raw[128+END-BASE:])):
        raise ValueError('title upload DMA/VIF/GIF envelope or trailing padding is outside the audited profile')
    return textures._words(uploads)


def _parameters(tokens=TOKENS):
    return {name: dict(width=(f := textures._fields(token))['width'], height=f['height'],
                       palette_size=256 if f['psm'] == 0x13 else 16)
            for name, token in zip(NAMES, tokens)}


def _layout(params):
    if not isinstance(params, dict) or set(params) != set(NAMES):
        raise ValueError('title profile requires exactly its ten named views')
    cursor, result = BASE, {}
    for name, original in zip(NAMES, TOKENS):
        row = params[name]
        if not isinstance(row, dict) or set(row) != {'width', 'height', 'palette_size'}:
            raise ValueError('texture parameters contain unknown fields; arbitrary addresses/opcodes are forbidden')
        width, height, count = row['width'], row['height'], row['palette_size']
        if (any(type(x) is not int for x in (width, height, count)) or count not in (16, 256)
                or width < 1 or width > 512 or height < 1 or height > 512
                or width & (width-1) or height & (height-1)):
            raise ValueError('title textures require power-of-two dimensions 1..512 and 16 or 256 palette entries; '
                             '00207E40 emits size*16 endpoints, so 1024 overflows the GS 14-bit UV fields')
        psm, bw = (0x13 if count == 256 else 0x14), max(2, width // 64)
        page_height = 64 if count == 256 else 128
        size = ((width+127)//128) * ((height+page_height-1)//page_height) * 8192
        # Reserve whole pages even for subpage views; TBW stays an even
        # number of 64-texel units. Small logical images remain supported.
        cursor = (cursor + 8191) & ~8191
        result[name] = dict(tbp=cursor // 256, tbw=bw, psm=psm, width=width, height=height)
        cursor += size
    for name in NAMES:
        cursor = (cursor + 255) & ~255
        result[name]['cbp'] = cursor // 256
        cursor += params[name]['palette_size'] * 4 if params[name]['palette_size'] == 256 else 256
    if cursor > END:
        raise ValueError(f'title layout needs {cursor-BASE} bytes but its owned GS arena is {END-BASE} bytes; explicitly reduce other view dimensions or palettes. Unproved external VRAM is not allocated')
    for name, original in zip(NAMES, TOKENS):
        f = result[name]
        mask = (1 << 51) - 1
        result[name]['tex0'] = ((original & ~mask) | f['tbp'] | (f['tbw'] << 14)
            | (f['psm'] << 20) | ((f['width'].bit_length()-1) << 26)
            | ((f['height'].bit_length()-1) << 30) | (original & (3 << 34 | 1 << 36)) | (f['cbp'] << 37))
    return result


def _decode(raw, tokens):
    words = _upload(raw)
    result = {}
    for name, token in zip(NAMES, tokens):
        fields, mapping = textures._view_map(words, token, True)
        palette = textures._palette(raw, textures._palette_map(words, fields['cbp'], fields['psm']))
        result[name] = (fields['width'], fields['height'], textures._rgba(raw, mapping, palette))
    return result


def _descriptor(tree, raw):
    path = tree / DESCRIPTOR
    if path.is_symlink():
        raise ValueError('texture descriptor may not be a symlink')
    if not path.exists():
        return None, TOKENS
    meta = _json(path)
    if (not isinstance(meta, dict) or set(meta) != {'schema', 'profile', 'views', 'upload_sha256', 'decoded_sha256'}
            or meta['schema'] != SCHEMA or meta['profile'] != PROFILE or meta['upload_sha256'] != _hash(raw)):
        raise ValueError('texture descriptor schema/profile/native hash mismatch')
    layout = _layout(meta['views'])
    tokens = tuple(layout[n]['tex0'] for n in NAMES)
    decoded = _decode(raw, tokens)
    if meta['decoded_sha256'] != {n: _hash(decoded[n][2]) for n in NAMES}:
        raise ValueError('texture descriptor decoded view hashes disagree with native pixels')
    return meta, tokens


def validate_tree(tree: Path) -> dict:
    """Validate authored semantic parameters; never accept patch bytes/addresses."""
    tree = Path(tree).resolve()
    if (tree / DESCRIPTOR).is_symlink():
        raise ValueError('texture descriptor may not be a symlink')
    if not (tree / DESCRIPTOR).exists():
        return dict(active=False, profile=PROFILE)
    tree, leaf = _leaf(tree)
    raw = leaf.read_bytes()
    meta, tokens = _descriptor(tree, raw)
    return dict(active=True, profile=PROFILE, descriptor=str(tree / DESCRIPTOR),
                upload_sha256=meta['upload_sha256'], views=meta['views'],
                tex0s={n: t for n, t in zip(NAMES, tokens)}, gs_range=[BASE, END])


def probe_runtime(game, plan: dict) -> dict:
    """Read the cold game's audited function/table without modifying its RAM."""
    if not plan.get('active') or plan.get('profile') != PROFILE:
        raise ValueError('runtime texture proof requires an active audited title upgrade')
    tokens = tuple(plan['tex0s'][name] for name in NAMES)
    expected = _compositor(tokens)
    actual = game.read(FUNCTION, FUNCTION_SIZE)
    if actual != expected:
        raise RuntimeError('loaded title compositor differs from the texture upgrade')
    table = struct.pack('<10Q', *tokens)
    offset = expected.find(table)
    if offset < 0 or offset % 8 or expected.find(table, offset + 1) >= 0:
        raise RuntimeError('audited title sampler table is not unique and aligned')
    live = struct.unpack_from('<10Q', actual, offset)
    return dict(function_vram=FUNCTION, function_size=FUNCTION_SIZE,
                function_sha256=_hash(actual), table_vram=FUNCTION+offset,
                table_sha256=_hash(actual[offset:offset+len(table)]),
                views={name: textures._fields(token) for name, token in zip(NAMES, live)},
                read_only=True, matches_authored_compositor=True)


def export_views(tree: Path, out: Path) -> dict:
    tree, leaf = _leaf(tree)
    raw = leaf.read_bytes()
    meta, tokens = _descriptor(tree, raw)
    out = safe_output(out)
    if out.exists() and any(out.iterdir()):
        raise ValueError('texture view export directory must be empty')
    out.mkdir(parents=True, exist_ok=True)
    views = {}
    for name, (w, h, pixels) in _decode(raw, tokens).items():
        png.write(out / (name + '.png'), w, h, pixels)
        views[name] = dict(png=name+'.png', palette_size=_parameters(tokens)[name]['palette_size'])
    spec = dict(profile=PROFILE, views=views)
    (out / 'spec.json').write_text(json.dumps(spec, indent=2)+'\n')
    return dict(output=str(out), spec=str(out / 'spec.json'), views=list(NAMES))


def _indexed(rgba, count, quantize):
    # RGBA255 maps exactly to native alpha128; other odd alpha values cannot
    # round-trip and must not be silently claimed exact.
    pixels = [bytes(rgba[i:i+4]) for i in range(0, len(rgba), 4)]
    colors = Counter(pixels)
    if any(c[3] != 255 and c[3] & 1 for c in colors):
        if quantize == 'exact':
            raise ValueError('GS palette alpha is 0..128: exact PNG alpha must be even or255')
        pixels = [c[:3] + bytes((min(255, ((c[3]+1)//2)*2),)) for c in pixels]
        colors = Counter(pixels)
    if len(colors) <= count:
        palette = sorted(colors)
    elif quantize == 'exact':
        raise ValueError(f'PNG needs {len(colors)} colors, exceeding palette{count}; opt in to quantize=median-cut')
    else:
        # Deterministic weighted median cut, in native decoded RGBA space.
        boxes = [list(colors)]
        while len(boxes) < count:
            splittable = [(max(max(c[k] for c in box)-min(c[k] for c in box) for k in range(4)),
                           sum(colors[c] for c in box), -i, i)
                          for i, box in enumerate(boxes) if len(box) > 1]
            if not splittable:
                break
            _, _, _, index = max(splittable)
            box = boxes.pop(index)
            channel = max(range(4), key=lambda k: max(c[k] for c in box)-min(c[k] for c in box))
            box.sort(key=lambda c: (c[channel], c))
            midpoint = sum(colors[c] for c in box)/2
            cumulative, split = 0, 1
            for i, c in enumerate(box[:-1]):
                cumulative += colors[c]
                split = i+1
                if cumulative >= midpoint:
                    break
            boxes.extend((box[:split], box[split:]))
        palette = []
        for box in boxes:
            weight = sum(colors[c] for c in box)
            rgba_mean = [round(sum(c[k]*colors[c] for c in box)/weight) for k in range(4)]
            rgba_mean[3] = min(255, ((rgba_mean[3]+1)//2)*2)
            palette.append(bytes(rgba_mean))
    lookup = {c: min(range(len(palette)), key=lambda i: sum((c[k]-palette[i][k])**2 for k in range(4))) for c in colors}
    indices = bytes(lookup[c] for c in pixels)
    decoded = b''.join(palette[i] for i in indices)
    palette += [bytes(4)] * (count-len(palette))
    return indices, palette, decoded


def upgrade_tree(tree: Path, spec: Path, *, quantize='exact') -> dict:
    """Apply explicit PNG/CLUT edits to one isolated disc loose tree."""
    if quantize not in ('exact', 'median-cut'):
        raise ValueError('quantize must be exact or median-cut')
    tree, leaf = _leaf(tree)
    spec = Path(spec).resolve()
    edit = _json(spec)
    if (not isinstance(edit, dict) or set(edit) != {'profile', 'views'} or edit['profile'] != PROFILE
            or not isinstance(edit['views'], dict) or not set(edit['views']) <= set(NAMES)):
        raise ValueError('only the closed title-menu-v1 profile and its named views are supported; actor/material references cross leaves')
    raw = leaf.read_bytes()
    meta, tokens = _descriptor(tree, raw)
    images, params = _decode(raw, tokens), _parameters(tokens)
    original_images = dict(images)
    for name, row in edit['views'].items():
        if not isinstance(row, dict) or set(row) != {'png', 'palette_size'}:
            raise ValueError('each view edit requires only png and palette_size')
        path = (spec.parent / row['png']).resolve()
        if not path.is_file():
            raise ValueError(f'PNG input missing for {name}')
        images[name] = png.read(path)
        params[name] = dict(width=images[name][0], height=images[name][1], palette_size=row['palette_size'])
    if images == original_images and params == _parameters(tokens):
        return dict(unchanged=True, native_sha256=_hash(raw), descriptor=str(tree / DESCRIPTOR) if meta else None)
    layout = _layout(params)
    rebuilt, words, decoded_hashes, loss = bytearray(raw), _upload(raw), {}, {}
    for name in NAMES:
        w, h, rgba = images[name]
        indices, palette, decoded = _indexed(rgba, params[name]['palette_size'], quantize)
        token = layout[name]['tex0']
        fields, mapping = textures._view_map(words, token, True)
        if len(mapping) != len(indices):
            raise ValueError('internal texture mapping length mismatch')
        for i, (at, mask, shift) in enumerate(mapping):
            rebuilt[at] = (rebuilt[at] & ~mask) | (indices[i] << shift)
        for color, at in zip(palette, textures._palette_map(words, fields['cbp'], fields['psm'])):
            rebuilt[at:at+4] = color[:3] + bytes(((color[3]+1)//2,))
        decoded_hashes[name] = _hash(decoded)
        if decoded != rgba:
            loss[name] = dict(changed_pixels=sum(decoded[i:i+4] != rgba[i:i+4] for i in range(0, len(rgba), 4)),
                              input_colors=len(set(rgba[i:i+4] for i in range(0, len(rgba), 4))))
    new_tokens = tuple(layout[n]['tex0'] for n in NAMES)
    decoded = _decode(rebuilt, new_tokens)
    if {n: _hash(decoded[n][2]) for n in NAMES} != decoded_hashes:
        raise ValueError('rebuilt native texture failed complete pixel validation')
    # Before changing either file, prove the authored bounded compositor's
    # behavior against the original function for this complete layout.
    boot = tree / 'iso/files/SCUS_971.12'
    _patch_elf(boot.read_bytes(), new_tokens)
    descriptor = dict(schema=SCHEMA, profile=PROFILE, views=params,
                      upload_sha256=_hash(rebuilt), decoded_sha256=decoded_hashes)
    _atomic(leaf, rebuilt)
    _atomic(tree / DESCRIPTOR, (json.dumps(descriptor, indent=2)+'\n').encode())
    return dict(unchanged=False, native_sha256=_hash(rebuilt), descriptor=str(tree / DESCRIPTOR),
                views=params, quantized=loss, gs_range=[BASE, END], native_size=len(rebuilt))


def _elf_offset(raw, address, size):
    if len(raw) < 52 or raw[:6] != b'\x7fELF\x01\x01':
        raise ValueError('texture patches require ELF32 little-endian input')
    ph = struct.unpack_from('<I', raw, 28)[0]
    stride, count = struct.unpack_from('<HH', raw, 42)
    if stride < 32 or ph+stride*count > len(raw):
        raise ValueError('invalid ELF program headers')
    candidates = []
    for i in range(count):
        kind, off, va, _, length = struct.unpack_from('<5I', raw, ph+i*stride)
        if kind == 1 and va <= address and address+size <= va+length and off+length <= len(raw):
            candidates.append(off+address-va)
    if len(candidates) != 1:
        raise ValueError('title function must be in exactly one complete ELF load segment')
    return candidates[0]


def _trace(code, selector, initial=1, *, with_state=False, with_abi=False, clobber_calls=True):
    """Bounded interpreter/dataflow audit for this single pinned function.

    Track immediate-bit origins, including shared constant materialization.
    Calls are observed, never executed; all three valid menu states and both
    initialization states are checked before and after any immediate edit.
    No original instruction stream is stored in this tooling.
    """
    mask = (1 << 64)-1
    def literal(value):
        return tuple(frozenset({(-1, (value >> b) & 1)}) for b in range(64))
    def value(expr):
        return sum(any((bit if at == -1 else (struct.unpack_from('<I', code, at)[0] >> bit)&1)
                       for at, bit in row) << b for b, row in enumerate(expr))
    regs = [literal(0x12340000+r) for r in range(32)]
    regs[0] = literal(0)
    high = [0x56780000+r if r else 0 for r in range(32)]
    regs[29], regs[31] = literal(0x100000), literal(0xFFFFFFFC)
    memory = {0x70003B6C:0x200000, 0x20000A:initial, 0x20000F:selector, 'writes':[]}
    saved, calls, pc, pending, steps = {}, [], 0, None, 0
    while 0 <= pc < len(code):
        steps += 1
        if steps > 400:
            raise ValueError('title constant trace did not terminate')
        word = struct.unpack_from('<I', code, pc)[0]
        op, rs, rt, rd, sa, fn = word>>26, (word>>21)&31, (word>>16)&31, (word>>11)&31, (word>>6)&31, word&63
        imm = word & 65535
        signed = imm if imm < 32768 else imm-65536
        old_pending, pending = pending, None
        if op == 15:
            regs[rt] = tuple(frozenset({(pc,b-16)}) if 16 <= b < 32 else
                             frozenset({(pc,15)}) if b >= 32 else literal(0)[b] for b in range(64))
        elif op == 13:
            regs[rt] = tuple(regs[rs][b] | (frozenset({(pc,b)}) if b < 16 else frozenset()) for b in range(64))
        elif op == 9:
            n = (value(regs[rs])+signed) & 0xFFFFFFFF
            regs[rt] = literal(n | (0xFFFFFFFF00000000 if n & 0x80000000 else 0))
        elif op == 0 and fn == 37 or op == 28 and fn == 40 and sa == 24:
            regs[rd] = tuple(a | b for a,b in zip(regs[rs], regs[rt]))
            if op == 28:
                high[rd] = high[rs] | high[rt]
        elif op == 0 and fn in (0,56,60):
            shift = sa + (32 if fn == 60 else 0)
            regs[rd] = literal(0)[:shift] + regs[rt][:64-shift]
        elif op in (35,36):
            addr = (value(regs[rs])+signed) & 0xFFFFFFFF
            regs[rt] = literal(memory.get(addr, 0))
        elif op == 55:
            addr = (value(regs[rs])+signed) & 0xFFFFFFFF
            relative = addr-FUNCTION
            if relative < 0 or relative+8 > len(code):
                raise ValueError('title constant table read is outside the function')
            regs[rt] = literal(struct.unpack_from('<Q', code, relative)[0])
        elif op == 30:
            addr = (value(regs[rs])+signed) & 0xFFFFFFFF
            regs[rt], high[rt] = saved.get(addr, (literal(0), 0))
        elif op == 31:
            addr = (value(regs[rs])+signed) & 0xFFFFFFFF
            saved[addr] = (regs[rt], high[rt])
        elif op in (40,41):
            addr = (value(regs[rs])+signed) & 0xFFFFFFFF
            memory[addr] = value(regs[rt]) & (255 if op == 40 else 65535)
            memory['writes'].append((addr, 1 if op == 40 else 2, memory[addr]))
        elif op in (4,5):
            equal = value(regs[rs]) == value(regs[rt])
            pending = ('jump', pc+4+signed*4 if equal == (op == 4) else pc+8)
        elif op == 3:
            pending = ('call', (word & 0x3FFFFFF)*4)
            regs[31] = literal(FUNCTION+pc+8)
        elif op == 0 and fn == 8:
            pending = ('jump', value(regs[rs])-FUNCTION)
        else:
            raise ValueError('unsupported instruction in pinned title constant trace')
        regs[0] = literal(0)
        pc += 4
        if old_pending:
            kind, target = old_pending
            if kind == 'jump':
                pc = target
            else:
                argc = {0x1ABF90:4, 0x207E40:7, 0x207D00:2, 0x1FB9F0:4}.get(target)
                if argc is None:
                    raise ValueError('title function calls outside the audited closed profile')
                calls.append((target, tuple(value(regs[r]) for r in range(4,4+argc)), tuple(regs[4:4+argc])))
                if clobber_calls:
                    for r in (*range(1,16),24,25):
                        regs[r], high[r] = literal(0x98760000+r), 0xABCD0000+r
    abi = {r:(value(regs[r]), high[r]) for r in (*range(16,24),28,29,30,31)}
    if with_abi:
        return calls, memory, abi
    return (calls, memory) if with_state else calls


def _compositor(tokens):
    """Assemble an authored equivalent into the existing function footprint.

    The compiler shared original TEX0 immediate fragments across unrelated
    menu words. Independent sizes/PSMs require a table, rather than editing
    those fragments. This replacement retains the original state writes,
    audio call, background call and sprite calls; only texture values differ.
    Its table lives after the return inside the same 556-byte function.
    """
    words, labels, branches = [], {}, []
    def emit(value):
        words.append(value)
    def i(op, rt, rs, imm):
        emit(op << 26 | rs << 21 | rt << 16 | imm & 65535)
    def label(name):
        labels[name] = len(words)*4
    def branch(op, rs, rt, name):
        branches.append((len(words), name)); i(op, rt, rs, 0); emit(0)
    def call(address):
        emit(3 << 26 | address // 4); emit(0)
    def task():
        i(15,2,0,0x7000); i(35,2,2,0x3B6C)
    def small(rt, number):
        i(13 if number >= 32768 else 9,rt,0,number)
    def sprite(y):
        for register, number in ((4,1),(5,0x77F0),(6,y),(7,256),(8,128)):
            small(register,number)
        i(15,9,0,0x8080); i(13,9,9,0x8080)
        call(0x207E40)

    i(9,29,29,-64)
    for register, offset in ((16,0),(17,16),(18,32),(31,48)):
        i(31,register,29,offset)
    task(); i(36,3,2,10)
    branch(4,3,0,'init')
    small(4,1); branch(5,3,4,'return')
    branch(4,0,0,'draw')
    label('init')
    small(3,1); i(40,3,2,10); i(41,0,2,24); i(40,0,2,16)
    for register, number in ((4,1500),(5,4096),(6,4096),(7,4096)):
        small(register,number)
    call(0x1FB9F0)
    label('draw')
    table_fix = len(words)
    i(15,16,0,0); i(13,16,16,0)
    for register, offset in ((4,0),(5,8),(6,16),(7,24)):
        i(55,register,16,offset)
    call(0x1ABF90)
    task(); i(36,17,2,15)
    small(4,1); small(5,0); call(0x207D00)
    i(55,10,16,56); branch(5,17,0,'new-game'); i(55,10,16,32)
    label('new-game'); sprite(0x8120)
    i(55,10,16,40); small(3,1); branch(5,17,3,'load-game'); i(55,10,16,64)
    label('load-game'); sprite(0x8230)
    i(55,10,16,48); small(3,2); branch(5,17,3,'options'); i(55,10,16,72)
    label('options'); sprite(0x8320)
    label('return')
    for register, offset in ((16,0),(17,16),(18,32),(31,48)):
        i(30,register,29,offset)
    i(9,29,29,64); emit(31 << 21 | 8); emit(0)
    while len(words)*4 % 16:
        emit(0)
    table = FUNCTION+len(words)*4
    words[table_fix] |= table >> 16
    words[table_fix+1] |= table & 65535
    for index, name in branches:
        delta = (labels[name]-(index*4+4))//4
        words[index] |= delta & 65535
    native = struct.pack('<'+'I'*len(words), *words) + struct.pack('<10Q', *tokens)
    if len(native) > FUNCTION_SIZE:
        raise ValueError('authored compositor exceeded its original code footprint')
    return native + bytes(FUNCTION_SIZE-len(native))


def _patch_elf(raw, tokens):
    offset = _elf_offset(raw, FUNCTION, FUNCTION_SIZE)
    code = raw[offset:offset+FUNCTION_SIZE]
    if _hash(code) != FUNCTION_SHA256:
        raise ValueError('title TEX0 patch requires the exact fresh canonical SCUS-97112 function; changed executable code is refused')
    if tuple(tokens) == TOKENS:
        return raw, []
    rebuilt = _compositor(tokens)
    replacements = dict(zip(TOKENS, tokens))
    for initial in (0,1,2,255):
        for selector in range(3):
            original, original_state, original_abi = _trace(code, selector, initial, with_abi=True)
            expected = []
            for target, args, _ in original:
                edited = tuple(replacements[a] if target == 0x1ABF90 or target == 0x207E40 and i == 6
                               else a for i,a in enumerate(args))
                expected.append((target,edited))
            actual, actual_state, actual_abi = _trace(rebuilt, selector, initial, with_abi=True)
            if ([(target,args) for target,args,_ in actual] != expected or actual_state != original_state
                    or actual_abi != original_abi):
                raise ValueError('authored compositor differs from original title calls, task state effects or preserved ABI registers')
    output = bytearray(raw); output[offset:offset+FUNCTION_SIZE] = rebuilt
    changed = [offset+i for i,(a,b) in enumerate(zip(code, rebuilt)) if a != b]
    return bytes(output), changed


def _patch_immediates(raw, tokens):
    """Retained narrow immediate solver for documenting compiler sharing limits."""
    offset = _elf_offset(raw, FUNCTION, FUNCTION_SIZE)
    code = raw[offset:offset+FUNCTION_SIZE]
    if _hash(code) != FUNCTION_SHA256:
        raise ValueError('title immediate proof requires the canonical function')
    replacements = dict(zip(TOKENS, tokens))
    zero, clauses, traces = set(), [], []
    for initial in (0,1):
        for selector in range(3):
            calls = _trace(code, selector, initial)
            seen = []
            for target, args, origins in calls:
                chosen = range(4) if target == 0x1ABF90 else (6,) if target == 0x207E40 else ()
                expected = list(args)
                for i in chosen:
                    if args[i] not in replacements:
                        raise ValueError('title trace produced an unexpected TEX0 consumer')
                    seen.append(args[i]); expected[i] = replacements[args[i]]
                for want, expression in zip(expected, origins):
                    for b, terms in enumerate(expression):
                        if (want >> b)&1:
                            if (-1,1) not in terms:
                                clauses.append(set(term for term in terms if term[0] >= 0))
                        else:
                            if (-1,1) in terms:
                                raise ValueError('requested TEX0 cannot fit the pinned constant materialization')
                            zero.update(term for term in terms if term[0] >= 0)
            if tuple(seen) != tuple(textures.TITLE) + tuple(textures.MENU[selector]):
                raise ValueError('closed title trace does not cover the expected background/menu draw sequence')
            traces.append((initial, selector, [(target, tuple(replacements.get(a,a) if
                (target == 0x1ABF90 or target == 0x207E40 and i == 6) else a for i,a in enumerate(args)))
                for target,args,_ in calls]))
    one = set()
    for clause in clauses:
        available = clause-zero
        if not available:
            raise ValueError('requested layout conflicts with shared title TEX0 immediate bits')
        existing = {t for t in available if (struct.unpack_from('<I',code,t[0])[0] >> t[1])&1}
        one.add(min(existing or available))
    rebuilt = bytearray(code)
    for at, bit in zero | one:
        word = struct.unpack_from('<I', rebuilt, at)[0]
        word = (word | 1 << bit) if (at,bit) in one else (word & ~(1 << bit))
        struct.pack_into('<I', rebuilt, at, word)
    for initial, selector, expected in traces:
        actual = [(target,args) for target,args,_ in _trace(rebuilt, selector, initial)]
        if actual != expected:
            raise ValueError('patched title function changed non-texture call behavior')
    output = bytearray(raw); output[offset:offset+FUNCTION_SIZE] = rebuilt
    changed = [offset+i for i,(a,b) in enumerate(zip(code, rebuilt)) if a != b]
    return bytes(output), changed


def apply_boot_patches(fresh_canonical_boot: Path, tree: Path, outboot: Path) -> dict:
    """Apply the profile's authored compositor/table after the source build."""
    if Path(outboot).is_symlink():
        raise ValueError('patched boot output may not be a symlink')
    report = validate_tree(tree)
    source, out = Path(fresh_canonical_boot).resolve(), safe_output(outboot)
    inputs = [source, Path(tree)/LEAF, Path(tree)/DESCRIPTOR, Path(tree)/'iso/files/SCUS_971.12']
    if any(out == p.resolve() or out.exists() and p.exists() and os.path.samefile(p,out) for p in inputs):
        raise ValueError('patched boot output aliases a texture upgrade input')
    raw = source.read_bytes()
    if report['active']:
        rebuilt, changed = _patch_elf(raw, tuple(report['tex0s'][n] for n in NAMES))
    else:
        rebuilt, changed = raw, []
    _atomic(out, rebuilt)
    return dict(output=str(out), sha256=_hash(rebuilt), original_elf_sha256=_hash(raw),
                profile=PROFILE, byte_identical=rebuilt == raw, changed_byte_offsets=changed,
                patch_kind='authored_compositor_and_tex0_table', function_vram=FUNCTION,
                function_size=FUNCTION_SIZE, original_function_sha256=FUNCTION_SHA256,
                tex0s=report.get('tex0s', {}))
