// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.91% NEARMISS).
// Fixes read from the original instructions:
// - The scratch hover-offset word at 0x70003604 is a relocated extern
//   (D_70003604), not a literal address; with the literal, mwcc filled the
//   -25 clamp branch's delay slot with the store's address setup.
// - torso_done is set in both arms (if / else), not initialised before the if.
// - The final range test checks "+0x54 above the threshold" first (clamp
//   +0x14 up to it); the else arm re-seeds the scratch matrix and snaps the
//   torso, as the original's branch order shows.
//
// Per-tick move/aim update for an actor (arg0, "self") driven by a target
// context (arg1). Seeds scratch quat/matrix D_7000... buffers, blends the
// self's position toward arg1's row/torso reference (+0x20/+0xA0), computes
// a pitch/height target (+0x14) clamped against several thresholds derived
// from arg1's row scalar (+0x14 at 0x70003A14) and the arg1 hover offset
// (0x70003604), and finally drives either a move-toward (arg2==0) or
// direct-set (arg2!=0) blend of +0x20, plus a torso aim blend of +0x10,
// gated on whether the accumulated squared offset (+0x24/+0x2C) exceeds
// 7.0 (mula.s/madd.s dot-product test).
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *p);
extern void func_00102C58(void *dst, void *src, void *b);
extern void func_001031E0(void *a, void *b);
extern void func_00103230(void *a, void *b, float angle);
extern float func_0011E748(float a);
extern void func_0018C4B0(void *dst, float val, float rate);
extern void func_0018C6A0(void *dst, void *src, float rate);
extern void func_0018D7B0(char *a, int b);

extern char D_70003040[];
extern char D_70003400[];
extern char D_70003430[];
extern char D_70003600[];
extern char D_70003630[];
extern char D_70003A10[];
extern float D_008105D0;
extern float D_008105E0;
extern char D_70003B50[];
extern float D_70003604;

void func_00197870(char *arg0, char *arg1, int arg2) {
    char *row;
    int torso_done;
    float f20;

    row = *(char **)(arg1 + 0x20);
    if (*(int *)(arg1 + 0x230) == 0x2A) {
        func_00102948(D_70003A10, D_70003040);
    } else {
        func_00102948(D_70003A10, arg1 + 0xA0);
    }
    func_00103230(arg0 + 0x20, row + 0xC0, 16.0f);
    *(float *)(arg0 + 0x20) = *(float *)(arg0 + 0x20) + *(float *)0x70003A10;
    *(float *)(arg0 + 0x24) = *(float *)(arg0 + 0x24) + (19.0f + *(float *)0x70003A14);
    *(float *)(arg0 + 0x28) = *(float *)(arg0 + 0x28) + *(float *)0x70003A18;
    *(int *)(arg0 + 0x2C) = 0x3F800000;

    func_001029C0(D_70003400);
    func_00102C58(D_70003400, D_70003400, D_70003B50);
    *(int *)0x70003600 = 0;
    D_70003604 = 0.0f;
    *(float *)0x70003608 = -30.0f;
    *(float *)0x7000360C = 1.0f;
    func_001031E0(D_70003430, D_70003A10);
    func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
    func_00103230(D_70003600, row + 0xC0, -30.0f);
    *(float *)(arg0 + 0x14) = 19.0f + (D_70003604 + *(float *)0x70003A14);

    if (D_70003604 < -22.0f) {
        if (D_70003604 < -25.0f) {
            D_70003604 = -25.0f;
        }
        f20 = 22.0f + D_70003604;
    } else {
        f20 = 0.0f;
    }

    if (*(float *)(arg0 + 0x14) > 30.0f + *(float *)0x70003A14) {
        *(float *)(arg0 + 0x14) = 30.0f + *(float *)0x70003A14;
    } else if (*(short *)(arg0 + 0x5A) & 0x10) {
        if (*(float *)(arg0 + 0x14) < 11.0f + *(float *)0x70003A14) {
            *(float *)(arg0 + 0x14) = 11.0f + *(float *)0x70003A14;
        }
    } else if (*(float *)(arg0 + 0x14) < 2.0f + *(float *)0x70003A14) {
        *(float *)(arg0 + 0x14) = 2.0f + *(float *)0x70003A14;
    }

    if (arg2 == 0) {
        func_0018C4B0(&D_008105E0, *(float *)(arg0 + 0x24), 0.6f);
        func_0018C6A0(arg0 + 0x20, &D_008105E0, 0.6f);
    } else {
        func_00102948(&D_008105E0, arg0 + 0x20);
    }
    func_0018D7B0(arg0, 2);

    if (arg2 != 0) {
        func_00102948(&D_008105D0, arg0 + 0x10);
        torso_done = 1;
    } else {
        torso_done = 0;
    }

    func_001028D0(D_70003630, arg0 + 0x10, D_70003A10);
    if (func_0011E748(*(float *)0x70003630 * *(float *)0x70003630 +
                      *(float *)0x70003638 * *(float *)0x70003638) < 7.0f) {
        if (*(float *)(arg0 + 0x54) > 18.0f + *(float *)0x70003A14 + f20) {
            if (*(float *)(arg0 + 0x14) < 18.0f + *(float *)0x70003A14 + f20) {
                *(float *)(arg0 + 0x14) = 18.0f + *(float *)0x70003A14 + f20;
            }
        } else if (*(float *)(arg0 + 0x14) < 18.0f + *(float *)0x70003A14 + f20) {
            func_001029C0(D_70003400);
            func_00102C58(D_70003400, D_70003400, D_70003B50);
            *(int *)0x70003600 = 0;
            D_70003604 = 0.0f;
            *(float *)0x70003608 = 5.0f;
            *(float *)0x7000360C = 1.0f;
            func_001031E0(D_70003430, D_70003A10);
            func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
            *(float *)(arg0 + 0x14) = *(float *)0x70003A14 + 18.0f + f20;
            func_0018D7B0(arg0, 2);
            func_00102948(&D_008105D0, arg0 + 0x10);
            torso_done = 1;
        }
    }

    if (torso_done == 0) {
        func_0018C6A0(arg0 + 0x10, &D_008105D0, 4.0f);
        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
    }
}
