// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.02% NEARMISS).
// Fixes read from the original instructions:
//  - The fraction term is the 64-bit product shifted right by one and truncated to
//    int, (int)(frac >> 1); the old spelling (int)((frac << 31) >> 32) computes the
//    same value but let the scheduler move the fourth call's first-argument copy
//    ahead of the truncation. Not a scheduler wall.
//
// SDK/MPEG timing words: fills *out_time from the context's 64-bit time (adjusted
// through the device block at D_002412F4 + 0x40 when it is negative and the device
// has a base: base + (rate * lo) / 2 plus a parity correction from the 64-bit
// multiply helper func_00125F48, bumping the device's counter when the parity term
// is set), applies a pending override (state 2 at 0xF8), then copies the 0x20 word
// to *out_a and packs the context's flag fields into *out_b.

extern unsigned char *D_002412F4;

extern long func_00125F48(long a, long b);

void func_00107CF0(unsigned char *ctx, long *out_time, long *out_a, long *out_b)
{
    unsigned char *dev;
    long t;
    long frac;
    long first;
    long p;
    long q;
    long r;
    int base;
    int lo;
    int bit;
    int cnt;
    int adj;

    dev = *(unsigned char **)(D_002412F4 + 0x40);
    if (*(int *)(dev + 0x70) != 0) {
        t = *(long *)(ctx + 0x18);
        if (t >= 0) {
            *out_time = t;
        } else {
            base = *(int *)(dev + 0x80);
            if (base < 0) {
                *out_time = t;
            } else {
                lo = (int)*(long *)(dev + 0x88);
                bit = lo & 1;
                first = func_00125F48(bit, *(long *)(dev + 0x78) & 1);
                cnt = *(int *)(dev + 0x90);
                adj = (int)func_00125F48(first, cnt & 1);
                frac = func_00125F48(*(long *)(dev + 0x78), lo);
                *out_time = base + ((int)(frac >> 1) + adj);
                if (func_00125F48(bit, *(long *)(dev + 0x78) & 1) != 0) {
                    *(int *)(dev + 0x90) = cnt + 1;
                }
            }
        }
    } else {
        *out_time = *(long *)(ctx + 0x18);
    }

    if (*(int *)(dev + 0xF8) == 2) {
        long pend = *(long *)(dev + 0xF0);
        if (pend >= 0) {
            *out_time = pend;
            *(int *)(dev + 0xF8) = 0;
            *(long *)(dev + 0xF0) = -1;
        }
    }

    q = ((long)*(int *)(ctx + 0x40) << 5) | ((long)*(int *)(ctx + 0x3C) << 6);
    *out_a = *(long *)(ctx + 0x20);
    p = ((long)*(int *)(ctx + 0x34) << 8) | *(int *)(ctx + 0x2C);
    r = ((long)*(int *)(ctx + 0x38) << 7) | ((long)*(int *)(ctx + 0x30) << 3);
    *out_b = (p | q) | r;
}
