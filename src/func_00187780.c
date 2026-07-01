// NEARMISS func_00187780  (vram 0x00187780, 0x53C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 78.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two independent non-idiom-fixable classes. (1) The stack frame carries a dead 64-bit magic constant (built via lui/ori/dsll/or into $s1 at both switch arms) that is unconditionally clobbered before any use -- an mwcc dead-value artifact with no discoverable C-level trigger (omitted from source si...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Muzzle-flash / weapon-glow setup: dispatches on (arg2, arg1) to seed a base
// intensity/size table (spF0/spB0/spC0), builds a transform (identity + Y-rotate by
// pi/2 via func_00102BB0/func_001026D0, offset by arg0+0xB0) into spE0, then probes
// nearby geometry via func_0019A570 to pick a fade term: on hit, computes
// length-squared of the probe vector via the FPU multiply-accumulate idiom
// (mul.s/adda.s/madd.s -- the "sum of 3 squares" pattern mwcc recognizes) and passes
// it through func_0011E748 (sqrt-family) to derive a distance-based intensity/size
// scale; on miss, uses a flat 1.0 scale and a different spD0 seed via
// func_00102900. arg2==0 additionally attenuates spB0 by view-angle via
// func_00102738/fabsf (func_0011DF78). Finishes by pushing 2 GS alpha-blend consts
// (func_0021B9A0) and drawing the glow sprite (func_00187690) once, twice if
// arg2==1 (second pass scaled 2x/1x/0.75x), gated on a global flag bit from
// func_001B0070.
//
// WALL: two independent non-idiom-fixable classes. (1) The stack frame carries a
// dead 64-bit magic constant (built via lui/ori/dsll/or into $s1 at both switch
// arms) that is unconditionally clobbered before any use -- an mwcc dead-value
// artifact with no discoverable C-level trigger. (2) Register-pressure/callee-save
// count differs (mwcc keeps 6 saved regs live across the whole function; this
// source keeps fewer), which cascades into a smaller stack frame (0x120/0x140 vs
// target 0x160) and reshuffles most stack-relative offsets even though the control
// flow, arithmetic, and FPU-MAC pattern all match. Best measured: mwcc233 78.6%,
// mwcc233(-sdatathreshold 4/8 tried, worse), 991202 70.9%.

extern int float_to_int(float);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001026D0(void *a0, void *a1, void *a2);
extern float func_00102738(void *a0, void *a1);
extern void func_00102760(void *a0, void *a1);
extern void func_001028D0(void *a0, void *a1, void *a2);
extern void func_00102900(void *a0, void *a1, float a2);
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_00102948(void *a0, void *a1);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float a2);
extern void func_00103230(void *a0, void *a1, float a2);
extern float func_0011DF78(float a0);
extern float func_0011E748(float a0);
extern int func_00122BB8(void);
extern void func_00187690(void *a0, void *a1, void *a2, int a3, float a4, float a5, float a6);
extern int func_0019A570(void *a0, void *a1, int a2, int a3);
extern int func_001B0070(void);
extern void func_001D9530(void *a0, void *a1, void *a2, void *a3, float a4);
extern void func_0021B9A0(int a0, float a1, float a2);
extern int D_00275B40;
extern float D_008105D0;

void func_00187780(char *arg0, int arg1, int arg2) {
    float sp120[16]; /* 0x40: 4x4 matrix (func_001029C0 identity target) */
    int sp110[4];    /* 0x10 */
    float sp100[4];  /* 0x10 */
    float spF0_0, spF0_4, spF0_8, spF0_C; /* 0x10 */
    float spE0[4];   /* 0x10 */
    float spD0[4];   /* 0x10 */
    float spC8;
    float spC4;
    float spC0;
    float spB8;
    float spB4;
    float spB0;
    float spA0[4];   /* 0x10 */
    float sp90[4];   /* 0x10 */
    float r;
    float len2;
    float f22, f23, f20;
    float dist;
    float f21;

    switch (arg2) {
    case 0:
        switch (arg1) {
        case 0:
        case 1:
            spF0_C = 64.0f;
            f22 = 17.0f;
            f23 = 3.5f;
            spB8 = 16.0f;
            spB4 = 16.0f;
            spB0 = 16.0f;
            r = 5.0f + (3.0f * ((float) func_00122BB8() / 2.1474836e9f));
            spC8 = r;
            spC4 = r;
            spC0 = r;
            break;
        }
        break;
    case 1:
        switch (arg1) {
        case 0:
        case 1:
            spF0_C = 32.0f;
            f22 = 17.0f;
            f23 = 3.5f;
            spB8 = 8.0f;
            spB4 = 8.0f;
            spB0 = 8.0f;
            r = 2.0f + ((float) func_00122BB8() / 2.1474836e9f);
            spC8 = r;
            spC4 = r;
            spC0 = r;
            break;
        }
        break;
    }

    func_001029C0(spE0);
    func_00102948(spE0, (char *) D_00275B40 + 0x90);
    func_00102948(&spF0_0, (char *) D_00275B40 + 0xA0);
    func_00102948(sp100, (char *) D_00275B40 + 0xB0);
    func_001029C0(sp120);
    func_00102BB0(sp120, sp120, 1.5707964f);
    func_001026D0(spE0, spE0, sp120);
    func_00102918(spE0, spE0, arg0 + 0xB0);

    sp90[0] = 0.0f;
    sp90[1] = 0.0f;
    *(unsigned int *) &sp90[2] = 0x437A0000;
    *(unsigned int *) &sp90[3] = 0x3F800000;
    func_001026A0(sp90, spE0, sp90);

    if (func_0019A570(sp110, sp90, 6, 0) != 0) {
        func_001028D0(sp90, &D_008105D0, sp110);
        len2 = sp90[1] * sp90[1] + sp90[0] * sp90[0];
        len2 += sp90[2] * sp90[2];
        r = func_0011E748(len2);
        f20 = r;
        f21 = r;
        if (!(f20 <= 17.0f)) {
            f20 = 17.0f;
        }
        if (!(f21 <= 250.0f)) {
            f21 = 250.0f;
        }
        f20 = f20 / f22;
        func_00102948(spD0, (char *) *(int *) 0x700031D0 + 0x24);
    } else {
        f21 = 250.0f;
        f20 = 1.0f;
        func_00102900(spD0, sp100, -1.0f);
    }

    f22 = f22 * f20;
    f23 = f23 * f20;
    spF0_C = spF0_C * f20;

    if (arg2 == 0) {
        func_001028D0(sp90, arg0, &D_008105D0);
        func_00102760(sp90, sp90);
        func_00102760(spA0, sp100);
        dist = func_0011DF78(func_00102738(sp90, spA0));
        if (!(dist <= 0.7f)) {
            func_00103230(&spB0, &spB0, 1.0f - (0.8f * ((dist - 0.7f) / 0.3f)));
        }
    }

    func_0021B9A0(2, 0.0f, -140.0f);
    func_0021B9A0(3, 0.0f, 450.5f);
    func_00187690(spE0, &spB0, &spC0, float_to_int(spF0_C), f22, 0.1f, f23);
    if (!(func_001B0070() & 0x20000000)) {
        func_001D9530(spE0, &spB0, &spC0, spD0, f21);
    }
    if (arg2 == 1) {
        func_00103230(&spB0, &spB0, 2.0f);
        func_00103230(&spC0, &spC0, 1.0f);
        func_00187690(spE0, &spB0, &spC0, float_to_int(spF0_C), f22, 0.1f, f23 * 0.75f);
    }
    func_0021B9A0(1, 0.0f, 0.0f);
}
