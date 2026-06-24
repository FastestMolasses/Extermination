// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Builds a packed visibility/clip-flag word for a transform.
//   func_00102948 fills a 4-float vector buf (b) from arg0; b[3]=1.0 (w).
//   func_001026A0 projects it into buf (a) using basis D_00810610; a[3]=0.
//   Then compares the projected component a[2] and four further projections
//   (func_00102738 at globalctx+0x2410..+0x2440) against +/-fparg0; any value
//   below -fparg0 short-circuits to -1, otherwise sets the matching bit
//   (1/2/4/8/0x10) in the returned flag word.
// D_00275670 sits in small data (gp-rel) so needs -sdatathreshold 8; the basis
// D_00810610 is a large object so it is declared sized to stay out of sdata
// (forces lui/%hi vs %gp_rel). The two stack vectors are float[4] so the w
// stores and the a[2] read use direct sp-relative addressing, matching the
// target frame (b at sp+0x40, a at sp+0x30). objdiff 100.0 on mwcc 2.3.3.
extern int D_00275670;
extern float D_00810610[16];
extern void func_00102948(void *, int);
extern void func_001026A0(void *, float *, void *);
extern float func_00102738(int, void *);

int func_001CA7B0(int arg0, float fparg0) {
    float a[4];
    float b[4];
    float f;
    float neg;
    int flags;

    flags = 0;
    func_00102948(b, arg0);
    b[3] = 1.0f;
    func_001026A0(a, D_00810610, b);
    a[3] = 0.0f;
    neg = -fparg0;
    f = a[2];
    if (f < neg) {
        return -1;
    }
    if (f < fparg0) {
        flags |= 1;
    }
    f = func_00102738(D_00275670 + 0x2410, a);
    if (f < neg) {
        return -1;
    }
    if (f < fparg0) {
        flags |= 2;
    }
    f = func_00102738(D_00275670 + 0x2420, a);
    if (f < neg) {
        return -1;
    }
    if (f < fparg0) {
        flags |= 4;
    }
    f = func_00102738(D_00275670 + 0x2430, a);
    if (f < neg) {
        return -1;
    }
    if (f < fparg0) {
        flags |= 8;
    }
    f = func_00102738(D_00275670 + 0x2440, a);
    if (f < neg) {
        return -1;
    }
    if (f < fparg0) {
        flags |= 0x10;
    }
    return flags;
}
