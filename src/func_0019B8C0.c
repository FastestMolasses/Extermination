// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003194 0x700031D0 0x700031D4 0x700031D8 0x7000324E 0x70003254
//
// Sibling of func_0019BA80 with the same instruction shape; it calls
// func_001A32C0 (arg3 bit 2) and func_0019E640 (arg3 bit 4) instead.
// Loop1 broadcasts arg1[0..2] into a local float[3] and into the
// D_70003190 block (offsets 0/0x10); a bias of +/-0.001f (from sign of
// *(arg2+4)) is folded into the global at 0x70003194 around the two calls,
// which select a result r in {0,2,4}; if r!=0 loop2 copies the saved locals
// back and computes a delta (0x20-0x10 -> 0x30), else clears 0x700031D0;
// result stored at 0x700031D8.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 91.32 NEARMISS whose header called the residual a register-allocation
// artifact. It was the source: this is func_0019BA80's body (separate cursors
// for the two loops, its declaration order, `i = 0` before `d` and `t`), with
// the scratchpad globals at 0x70003194 and 0x700031D8 as relocated externs
// (MATCHING_GUIDE idiom-32) and the clears of 0x700031AC / 0x7000319C literal.
extern float D_70003194;
extern int D_700031D0;
extern int D_700031D4;
extern int D_700031D8;
extern short D_7000324E;
extern int D_70003254;
extern int func_001A32C0(void);
extern int func_0019E640(void);
extern char D_70003190[];

int func_0019B8C0(char *arg0, float *arg1, char *arg2, int arg3) {
    int i;
    char *d;
    float bias;
    int r;
    char *d2;
    float *q;
    float *t;
    float local[3];
    float *p;

    q = local;
    r = 0;
    p = arg1;
    i = 0;
    d = D_70003190;
    t = q;
    do {
        float v = *p;
        i += 1;
        *(float *)(d + 0x10) = v;
        *(float *)(d + 0x0) = v;
        *t = v;
        p += 1;
        d += 4;
        t += 1;
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
        if (func_001A32C0() == 0) {
            r = 2;
        }
    }
    if ((arg3 & 4) && func_0019E640() == 0) {
        r = 4;
    }
    D_70003194 = D_70003194 - bias;

    if (r != 0) {
        d2 = D_70003190;
        i = 0;
        do {
            i += 1;
            *(float *)(d2 + 0x10) = *q;
            q += 1;
            *(float *)(d2 + 0x30) = *(float *)(d2 + 0x20) - *(float *)(d2 + 0x10);
            d2 += 4;
        } while (i < 3);
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = r;
    return r;
}
