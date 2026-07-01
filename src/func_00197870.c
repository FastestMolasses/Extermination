// NEARMISS func_00197870  (vram 0x00197870, 0x4A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (two-exit float-clamp shape via the bc1t-jump-into-body idiom applied twice; correct arg counts and D_00275670-style reload conventions). Sole remaining residuals: (1) one idiom-13 genuine clean-constant-store delay-slot nop (target leaves nop before a lui/sw pair I...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
    *(float *)0x70003604 = 0.0f;
    *(float *)0x70003608 = -30.0f;
    *(float *)0x7000360C = 1.0f;
    func_001031E0(D_70003430, D_70003A10);
    func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
    func_00103230(D_70003600, row + 0xC0, -30.0f);
    *(float *)(arg0 + 0x14) = 19.0f + (*(float *)0x70003604 + *(float *)0x70003A14);

    if (*(float *)0x70003604 < -22.0f) {
        if (*(float *)0x70003604 < -25.0f) {
            *(float *)0x70003604 = -25.0f;
        }
        f20 = 22.0f + *(float *)0x70003604;
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

    torso_done = 0;
    if (arg2 != 0) {
        func_00102948(&D_008105D0, arg0 + 0x10);
        torso_done = 1;
    }

    func_001028D0(D_70003630, arg0 + 0x10, D_70003A10);
    if (!(func_0011E748(*(float *)0x70003630 * *(float *)0x70003630 +
                         *(float *)0x70003638 * *(float *)0x70003638) < 7.0f)) {
        /* out of dot-product range: nothing to do */
    } else if (*(float *)(arg0 + 0x54) <= 18.0f + *(float *)0x70003A14 + f20) {
        if (*(float *)(arg0 + 0x14) < 18.0f + *(float *)0x70003A14 + f20) {
            func_001029C0(D_70003400);
            func_00102C58(D_70003400, D_70003400, D_70003B50);
            *(int *)0x70003600 = 0;
            *(int *)0x70003604 = 0;
            *(float *)0x70003608 = 5.0f;
            *(float *)0x7000360C = 1.0f;
            func_001031E0(D_70003430, D_70003A10);
            func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
            *(float *)(arg0 + 0x14) = *(float *)0x70003A14 + 18.0f + f20;
            func_0018D7B0(arg0, 2);
            func_00102948(&D_008105D0, arg0 + 0x10);
            torso_done = 1;
        }
    } else if (*(float *)(arg0 + 0x14) < 18.0f + *(float *)0x70003A14 + f20) {
        *(float *)(arg0 + 0x14) = 18.0f + *(float *)0x70003A14 + f20;
    }

    if (torso_done == 0) {
        func_0018C6A0(arg0 + 0x10, &D_008105D0, 4.0f);
        func_0018C4B0(&D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
    }
}
