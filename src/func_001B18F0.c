// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.23% NEARMISS).
// Fixes read from the original instructions:
// - The object pointer lives in s0 and the two direction vectors in s2/s1:
//   mwcc 2.3.3 gives a local declared first the lowest saved register, so the
//   object is copied into a local (identity cast) before the other locals,
//   while the raw vector parameters take the remaining registers. This was the
//   whole "register-allocation permutation" residual, not a compiler wall.
//
// Probes the collision test func_001B1630 with up to four directions: +arg1,
// -arg1, +arg2, -arg2. Each direction is written to the scratchpad vector at
// 0x70003600 (w = 1.0), transformed in place by func_001026A0 with the
// object's matrix at +0xD0, and tested; the result byte is stored at object+1.
// The first hit runs func_001B1B70(object) and returns 1; no hit returns 0.
extern void func_001026A0(float *, void *, float *);
extern int func_001B1630(float, float, float);
extern void func_001B1B70(void *);
extern float D_70003600[4];

int func_001B18F0(char *arg0, float *arg1, float *arg2) {
    char *o = (char *)arg0;
    float *p = (float *)0x70003600;
    int r;

    p[0] = arg1[0];
    p[1] = arg1[1];
    p[2] = arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, o + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    o[1] = r;
    if (r & 0xFF) {
        goto hit;
    }

    p[0] = -arg1[0];
    p[1] = -arg1[1];
    p[2] = -arg1[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, o + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    o[1] = r;
    if (r & 0xFF) {
        goto hit;
    }

    p[0] = arg2[0];
    p[1] = arg2[1];
    p[2] = arg2[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, o + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    o[1] = r;
    if (r & 0xFF) {
        goto hit;
    }

    p[0] = -arg2[0];
    p[1] = -arg2[1];
    p[2] = -arg2[2];
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(D_70003600, o + 0xD0, D_70003600);
    r = func_001B1630(p[0], p[1], p[2]);
    o[1] = r;
    if (!(r & 0xFF)) {
        return 0;
    }

hit:
    func_001B1B70(o);
    return 1;
}
