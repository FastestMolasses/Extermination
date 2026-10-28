// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 5.42% NEARMISS).
// Fixes read from the original instructions:
//  - D_00275670 is $gp-relative: -sdatathreshold 4, not 0
//  - spelled like the byte-matched packet family (func_002081A0 / func_002082B0):
//    channel record D_00275670 + (slot << 2), the +0x10 cursor re-read for each
//    header store, rec = cursor, cursor = rec + 0x80, stores at rec + offsets
//  - the texture size is computed as two locals reassigned in place
//    (tw = log2 width, then tw = 1 << tw; likewise th), which keeps the UV
//    computation ahead of the header stores; written inline it was sunk to
//    its store
//  - rgba is an unsigned int (zero-extended into both RGBAQ doublewords)
//
// Builds a one-sprite GIF packet into the per-channel buffer selected by slot.
// The channel record's +0x10 word is a bump-allocated cursor. A DMA tag is
// written at the cursor (byte 3 = 0x10, word 4 = 0, halfword 0 = 7 quadwords),
// the cursor advances by 0x80, and the packet gets a cleared quadword at +0x10
// with VIF DIRECT 6 (0x50000006) at +0x1C, a GIF tag 0xA400000000008001 with
// register list 0x8413413680 at +0x20/+0x28, PRIM 0x156 at +0x30, 5 at +0x38,
// tex0 at +0x40, then two vertices: UV 0 / RGBAQ rgba / XYZ (x0, y0 + 8*height)
// and UV (16 << tw, 16 << th) / RGBAQ rgba / XYZ (x0 + 16*width, y0), each XYZ
// with 0xFFFFFF in the upper word, where tw/th are tex0's 4-bit log2 width and
// height fields (bits 26 and 30); +0x78 is cleared.
typedef unsigned __int128 u128;
extern char *D_00275670;

void func_00207E40(int slot, int x0, int y0, int width, int height, unsigned int rgba, unsigned long long tex0) {
    char *rec;
    char *elem;
    int xyz1;
    int xyz2;
    int tw;
    int th;

    xyz1 = x0 | ((y0 + (height << 3)) << 16);
    xyz2 = (y0 << 16) | (x0 + (width << 4));
    tw = (int)((tex0 >> 26) & 0xF);
    th = (int)((tex0 >> 30) & 0xF);
    tw = 1 << tw;
    th = 1 << th;
    elem = D_00275670 + (slot << 2);
    (*(char **)(elem + 0x10))[3] = (char)0x10;
    *(int *)(*(char **)(elem + 0x10) + 4) = 0;
    *(short *)(*(char **)(elem + 0x10) + 0) = 7;
    rec = *(char **)(elem + 0x10);
    *(char **)(elem + 0x10) = rec + 0x80;
    *(u128 *)(rec + 0x10) = 0;
    *(int *)(rec + 0x1C) = 0x50000006;
    *(long long *)(rec + 0x20) = 0x8001 | ((long long)0xA4000000 << 32);
    *(long long *)(rec + 0x28) = 0x13413680 | ((long long)0x84 << 32);
    *(long long *)(rec + 0x30) = 0x156;
    *(long long *)(rec + 0x38) = 5;
    *(long long *)(rec + 0x40) = tex0;
    *(long long *)(rec + 0x48) = 0;
    *(long long *)(rec + 0x50) = rgba;
    *(long long *)(rec + 0x58) = (long long)xyz1 | ((long long)0xFFFFFF << 32);
    *(long long *)(rec + 0x60) = (tw << 4) | (th << 20);
    *(long long *)(rec + 0x68) = rgba;
    *(long long *)(rec + 0x70) = (long long)xyz2 | ((long long)0xFFFFFF << 32);
    *(long long *)(rec + 0x78) = 0;
}
