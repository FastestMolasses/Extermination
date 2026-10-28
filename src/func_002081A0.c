// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 50.85% NEARMISS).
// Fixes read from the original instructions:
//  - the packet length n is a plain int (no 16-bit sign extension); the old
//    C declared it short
//  - spelled like the byte-matched sibling func_002082B0: the channel record
//    is D_00275670 + (arg0 << 2), the +0x10 quadword is cleared with one
//    128-bit store through ret = rec + 0x10, and the vertex cursor is ret + 0x40
//  - the loop counter is zeroed right before the `0 < arg3` guard
//  - local declaration order fixes the register assignment
//
// Builds a GIF/GS DMA packet into the per-channel buffer selected by arg0.
// The channel record's +0x10 word is a bump-allocated cursor. A header is
// written at the cursor (byte 3 = 0x10, word 4 = 0, halfword 0 = n where
// n = arg3/2 + 4), the cursor advances by (n + 1) * 0x10, and the record
// gets a cleared quadword at +0x10, NLOOP (n - 1) | 0x50000000 at +0x1C, a
// GIF tag 0x2400000000008001 / 0x10 at +0x20/+0x28, arg2 | 0x140 and the
// zero-extended arg4 at +0x30/+0x38, a second tag arg3 | 0x1400000000008000
// with register list 4 at +0x40/+0x48, then arg3 doublewords at +0x50: each
// packs the two ints at arg1+0/+4 (low | high << 16) with 0xFFFFFF in the
// upper word, stepping arg1 by 0x10 per entry.
typedef unsigned __int128 u128;
extern char *D_00275670;

void func_002081A0(int arg0, char *arg1, int arg2, int arg3, int arg4) {
    char *rec;
    int i;
    char *elem;
    long long *q;
    int n;
    char *ret;

    n = (arg3 >> 1) + 4;
    elem = D_00275670 + (arg0 << 2);
    (*(char **)(elem + 0x10))[3] = (char)0x10;
    *(int *)(*(char **)(elem + 0x10) + 4) = 0;
    *(short *)(*(char **)(elem + 0x10) + 0) = n;
    rec = *(char **)(elem + 0x10);
    *(char **)(elem + 0x10) = rec + ((n + 1) * 0x10);
    ret = rec + 0x10;
    *(u128 *)ret = 0;
    *(int *)(rec + 0x1C) = (n - 1) | 0x50000000;
    *(long long *)(rec + 0x20) = 0x8001 | ((long long)0x24000000 << 32);
    *(long long *)(rec + 0x28) = 0x10;
    q = (long long *)(ret + 0x40);
    *(long long *)(rec + 0x30) = (long long)arg2 | 0x140;
    *(long long *)(rec + 0x38) = (unsigned int)arg4;
    *(long long *)(rec + 0x40) = (long long)arg3 | (0x8000 | ((long long)0x14000000 << 32));
    *(long long *)(rec + 0x48) = 4;
    i = 0;
    if (0 < arg3) {
        do {
            i += 1;
            *q = (long long)(*(int *)(arg1 + 0) | (*(int *)(arg1 + 4) << 0x10)) | ((long long)0xFFFFFF << 32);
            q += 1;
            arg1 += 0x10;
        } while (i < arg3);
    }
}
