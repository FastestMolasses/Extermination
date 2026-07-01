// NEARMISS func_001854E0  (vram 0x001854E0, 0x280 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring on the sum-of-squares (adda.s/madd.s) idiom -- identical wall class already documented on the sibling func_00185E30 (parked at 96.1%). Body and structure fully recovered: AI candidate-target scoring, transform staging via func_00103230/func_001028B8/func_00102948, func_0019A5...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// AI candidate-target validation / lock-on scoring, sibling of func_00185E30.
// Stages arg0's transform: scales arg0+0xC0 by 65.0f (0x43820000) into
// D_700038A0 via func_00103230, composes with arg0+0xA0 via func_001028B8,
// sets D_700038AC=1.0f, copies arg0+0xA0 into D_700038B0 via func_00102948.
// If func_0019A570(D_700038B0, D_700038A0, 7, 0x20) succeeds: if the shared
// target pointer at 0x700031D4 is set and func_00183AC0() accepts it, marks
// its flag byte +0xA = 0x80. Runs func_001031E0(D_700038A0, D_700031B0),
// sets arg0+0xBC=1.0f, arg0's sub-object (+0x1F0)+0x20=1, composes
// (+0x1F0+0x10) via func_00102948, then func_001028D0 computes a delta into
// D_700038D0. Computes sqrt(sum of squares) of that delta (mwcc adda.s/
// madd.s idiom) via func_0011E748; if >= 260.0f, sub-object+0x24 = 0.0f.
// Else stores the distance, subtracts 20.0f: if negative, +0x24 = 1.0f;
// else +0x24 = (240.0f - dist) / 240.0f (falloff ramp). Finally packs a
// render-state word from func_00122BB8()'s bits and issues a
// func_001CD520 draw/effect call with a packed 64-bit color/id argument.

extern void func_00102948(void *dst, void *src);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00103230(void *a, void *b, float c);
extern void func_001031E0(void *a, void *b);
extern int func_0019A570(void *a, void *b, int c, int d);
extern float func_0011E748(float x);
extern int func_00122BB8(void);
extern int func_00183AC0(void *a);
extern int func_001CD520(int, int, void *, long long, int, float, float, float);

extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038D0;

static inline long long shl64(long long x, int n) {
    return x << n;
}

void func_001854E0(char *arg0) {
    char *sub = arg0 + 0x1F0;
    void *target;

    func_00103230(&D_700038A0, arg0 + 0xC0, 65.0f);
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xA0);
    *(int *)0x700038AC = 0x3F800000;
    func_00102948(&D_700038B0, arg0 + 0xA0);

    if (func_0019A570(&D_700038B0, &D_700038A0, 7, 0x20) != 0) {
        target = *(void **)0x700031D4;
        if (target != 0 && func_00183AC0(target) != 0) {
            *((char *)target + 0xA) = 0x80;
        }
        func_001031E0(&D_700038A0, &D_700031B0);
        *(int *)(arg0 + 0xBC) = 0x3F800000;
        *(int *)(sub + 0x20) = 1;
        func_00102948(sub + 0x10, &D_700038A0);
        func_001028D0(&D_700038D0, &D_700038A0, &D_700038B0);
        {
            float dx = *(float *)0x700038D0;
            float dy = *(float *)0x700038D4;
            float dz = *(float *)0x700038D8;
            float d = func_0011E748((dx * dx) + (dy * dy) + (dz * dz));
            *(float *)0x70003A24 = d;
            if (!(d < 260.0f)) {
                goto zero_out;
            }
            {
                float d2 = d - 20.0f;
                *(float *)0x70003A24 = d2;
                if (d2 < 0.0f) {
                    *(float *)(sub + 0x24) = 1.0f;
                } else {
                    *(float *)(sub + 0x24) = (240.0f - d2) / 240.0f;
                }
            }
        }
        goto after;
    }
zero_out:
    *(float *)(sub + 0x24) = 0.0f;
after:

    *(int *)0x700038B0 = ((func_00122BB8() >> 0xF) & 0x1F) + 0x40;
    *(int *)0x700038B4 = 0;
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0x80;

    {
        int packed = *(int *)0x700038B0 |
                      (*(int *)0x700038B4 << 8) |
                      (*(int *)0x700038B8 << 16) |
                      (*(int *)0x700038BC << 24);
        func_001CD520(0, 2, &D_700038A0,
                      shl64((long long)0x20045BA5, 0x20) | 0x154222DC,
                      packed, 3.0f, 3.0f, 2.0f);
    }
}
