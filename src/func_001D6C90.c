// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.14% NEARMISS).
// Fixes read from the original instructions:
//  - The returned packet-data pointer (write pointer + 0x10) is its own local,
//    taken right after the write pointer is read and before the slot pointer is
//    advanced by 0x60; the zeroed quadword is stored through it and it is the
//    return value. That one extra live value moved the whole register colouring
//    (index temp, sign-extended arg1, packet pointer, constants) onto the
//    original's assignment; the body was already correct.
//
// Builds a 0x60-byte GS/GIF DMA packet into the per-slot buffer at
// (*(D_00275670 + arg0*4 + 0x10)) and advances that pointer by 0x60.
// Packs the int args into the A+D register doublewords at p+0x30..p+0x58:
// arg1 is the TEXA (0x3B) data, arg2..arg9 the TEST_1 (0x47) fields
// (ATE, ATST<<1, AREF<<4, AFAIL<<12, DATE<<14, DATM<<15, ZTE<<16, ZTST<<17),
// arg10..arg14 the ALPHA_1 (0x42) fields (A, B<<2, C<<4, D<<6, FIX<<32).
// Zeroes the 0x10 quadword, writes the 0x1C control word 0x50000004, the
// GIFtag at 0x20 (0x8003 | 0x10000000<<32), the A+D register list 0xE at
// 0x28, and the 0x00/0x03/0x04 header fields.
typedef int u128 __attribute__((mode(TI)));
extern char *D_00275670;

char *func_001D6C90(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5,
                    int arg6, int arg7, int arg8, int arg9, int arg10,
                    int arg11, int arg12, int arg13, int arg14) {
    char *base;
    char *q;
    char *p;

    base = D_00275670 + (arg0 * 4);
    *(char *)(*(char **)(base + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(base + 0x10) + 4) = 0;
    *(short *)(*(char **)(base + 0x10) + 0) = 5;
    p = *(char **)(base + 0x10);
    q = p + 0x10;
    *(char **)(base + 0x10) = p + 0x60;
    *(u128 *)q = 0;
    *(int *)(p + 0x1C) = 0x50000004;
    *(long long *)(p + 0x20) = 0x8003 | ((long long)0x10000000 << 32);
    *(long long *)(p + 0x28) = 0xE;
    *(long long *)(p + 0x30) = (long long)(int)arg1;
    *(long long *)(p + 0x38) = 0x3B;
    *(long long *)(p + 0x40) =
        (long long)(int)arg2 |
        ((long long)(int)arg3 << 1) |
        ((long long)(int)arg4 << 4) |
        ((long long)(int)arg5 << 12) |
        ((long long)(int)arg6 << 14) |
        ((long long)(int)arg7 << 15) |
        ((long long)arg8 << 16) |
        ((long long)arg9 << 17);
    *(long long *)(p + 0x48) = 0x47;
    *(long long *)(p + 0x50) =
        (long long)arg10 |
        ((long long)arg11 << 2) |
        ((long long)arg12 << 4) |
        ((long long)arg13 << 6) |
        ((long long)arg14 << 32);
    *(long long *)(p + 0x58) = 0x42;
    return q;
}
