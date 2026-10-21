// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003194 0x700031C4 0x700031D0 0x700031D4 0x700031D8 0x7000324E 0x70003254
//
// Sibling of func_0019BA80. Copies arg1[0..2] into a local float[3] and into
// the D_70003190 block (offsets 0 and 0x10), subtracts the float at arg2+4 from
// the float at 0x70003194 and adds a bias (+0.001f when that float is negative,
// else -0.001f), clears the words
// at 0x700031AC / 0x7000319C / 0x700031D4 and stores the byte at arg0+2 & 0x1F
// as the halfword at 0x7000324E. With arg3 bit 2 it stores the word at
// arg0+0x14 to 0x70003254 and r = 2 when func_0019F730() returns 0; with bit 4,
// r = 4 when func_0019C830() returns 0. The bias is removed again. If r != 0 the
// saved local vector is copied back to block+0x10, block+0x30 = block+0x20 -
// block+0x10, and with arg3 bit 31 the float at arg0+0xB4 gains the float at
// 0x700031C4; else the word at 0x700031D0 is cleared. r is stored at
// 0x700031D8 and returned.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 95.99 NEARMISS. Keys: the scratchpad globals at 0x70003194 and 0x700031D8
// are relocated externs (MATCHING_GUIDE idiom-32; the other externs here score
// the same either way), the clears of 0x700031AC / 0x7000319C stay literal
// stores, func_0019F730 takes no arguments (round 5), and the local declaration
// order is the round-5 sweep's.
extern float D_70003194;
extern float D_700031C4;
extern int D_700031D0;
extern int D_700031D4;
extern int D_700031D8;
extern short D_7000324E;
extern int D_70003254;
extern int func_0019F730(void);
extern int func_0019C830(void);
extern char D_70003190[];

int func_0019AB20(char *arg0, float *arg1, char *arg2, int arg3) {
    float bias;
    float local[3];
    int i;
    float *blk;
    int r;
    float *dst;
    float *saved;
    float *src;

    r = 0;
    src = arg1;
    blk = (float *)D_70003190;
    saved = local;
    i = 0;
    dst = saved;
    do {
        float t = *src;
        i += 1;
        *(float *)((char *)blk + 0x10) = t;
        *blk = t;
        *dst = t;
        src += 1;
        blk += 1;
        dst += 1;
    } while (i < 3);
    D_70003194 = D_70003194 - *(float *)(arg2 + 4);
    if (*(float *)(arg2 + 4) < 0.0f) {
        bias = 0.001f;
    } else {
        bias = -0.001f;
    }
    D_70003194 = D_70003194 + bias;
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    D_700031D4 = 0;
    D_7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;
    if (arg3 & 2) {
        D_70003254 = *(int *)(arg0 + 0x14);
        if (func_0019F730() == 0) {
            r = 2;
        }
    }
    if ((arg3 & 4) && func_0019C830() == 0) {
        r = 4;
    }
    D_70003194 = D_70003194 - bias;
    if (r != 0) {
        float *blk2 = (float *)D_70003190;
        i = 0;
        do {
            i += 1;
            *(float *)((char *)blk2 + 0x10) = *saved;
            saved += 1;
            *(float *)((char *)blk2 + 0x30) = *(float *)((char *)blk2 + 0x20) - *(float *)((char *)blk2 + 0x10);
            blk2 += 1;
        } while (i < 3);
        if (arg3 & 0x80000000) {
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + D_700031C4;
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = r;
    return r;
}
