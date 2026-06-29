// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Reload/cooldown tick for one weapon entity: gates on pending count 0x36, when
// a reload-lock 0x52 is busy just resets to state 1; otherwise advances state 2,
// computes per-tick step d = sign-extend16(0x36 & 0xFFF), x5 when the 0x8000
// (fast) flag is set, accumulates into 0x54, sets a 0x19 timer at 0x5A, fires a
// one-shot event func_001EFE00(0x80000027) once per 0x4000 flag via the 0x5B
// gate. When step >= remaining 0x34 the magazine finishes (state 2, sub 1) and
// func_001B4CF0 commits; else decrements 0x34, handles the 0x8000/0x2000 flags,
// emits a sound func_001FBD50(e,0x866,0,300.0f), and finally idles state 1 and
// eases a 0.0..0.9 ramp at 0x20 by +0.1.
// 233-specific: compare written `0x34 <= (short)d` to get the target's blt/slt-$at
// branch shape; the 0.1f added via a named local `c` so the loaded value stays in
// f1 and the add emits `f1 + f0` (FP-coloring match) instead of recoloring to f0.
extern void func_001B4CF0(unsigned char *e);
extern void func_001EFE00(int a, unsigned char *e);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);

int func_0014E9C0(unsigned char *e, unsigned char *p) {
    short m;
    int d;

    if (*(short *)(e + 0x36) == 0) {
        return 0;
    }
    if (*(short *)(p + 0x52) != 0) {
        e[0] = 1;
        *(short *)(e + 0x36) = 0;
        return 0;
    }
    e[0] = 2;
    *(short *)(p + 0x52) = 0xFF;
    m = *(short *)(e + 0x36);
    d = (short)(m & 0xFFF);
    if (m & 0x8000) {
        d = (short)(d * 5);
    }
    if (*(signed char *)(p + 0x5A) != 0) {
        *(short *)(p + 0x54) = *(short *)(p + 0x54) + d;
    } else {
        *(short *)(p + 0x54) = d;
    }
    p[0x5A] = 0x19;
    if (*(signed char *)(p + 0x5B) == 0 && (*(short *)(e + 0x36) & 0x4000)) {
        p[0x5B] = 0x3C;
        func_001EFE00(0x80000027, e);
    }
    if (*(short *)(e + 0x34) <= (short)d) {
        *(short *)(e + 0x34) = 0;
        e[4] = 2;
        e[5] = 1;
        e[6] = 0;
        func_001B4CF0(e);
        return 1;
    }
    *(short *)(e + 0x34) = *(short *)(e + 0x34) - d;
    m = *(short *)(e + 0x36);
    if (m & 0x8000) {
        e[4] = 2;
        e[5] = 0;
        e[6] = 0;
        *(short *)(p + 0x54) = 0;
        return 1;
    }
    if (*(short *)(p + 0x54) >= 0x19 || (m & 0x2000)) {
        *(short *)(p + 0x54) = 0;
        p[0x5C] = 1;
        func_001FBD50(e, 0x866, 0, 300.0f);
    }
    m = *(short *)(e + 0x36);
    if (m & 0x5000) {
        *(short *)(p + 0x52) = 0x1E;
    } else if (m & 0x2000) {
        *(short *)(p + 0x52) = 0x50;
    } else {
        *(short *)(p + 0x52) = 0;
    }
    e[0] = 1;
    *(short *)(e + 0x36) = 0;
    *(short *)(p + 0x50) = 0;
    if (*(signed char *)(p + 0x5D) != 0) {
        float f = *(float *)(p + 0x20);
        if (f <= 0.9f) {
            float c = 0.1f;
            *(float *)(p + 0x20) = f + c;
        }
    }
    return 0;
}
