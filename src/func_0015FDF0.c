// NEARMISS func_0015FDF0  (vram 0x0015FDF0, 0x42C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.01% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// see above
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (objdiff 91.01% vs expected on mwcc 2.3.3; 78.49% on pinned 991202).
// Logic fully recovered: a line-of-sight / aim solver toward the target actor
// returned by func_001AA4E0. Computes the horizontal delta to the target
// (staged at 0x700038A0/0x700038A8), the desired facing as pi/2 + atan2(-dy,dx)
// wrapped by func_001B1470. If the actor is in mode>=2 (func_00174AC0 + the
// arg0+0x23F byte) and the angular error to its aim field (arg0+0x218) exceeds
// 40deg (0.69813174 rad, tested via func_0011DF78), it snaps arg0+0xC4 to that
// aim and returns. Otherwise it tests the error to the current facing arg0+0xC4;
// if within the cone, it builds a probe direction at +/-40deg (sign chosen by
// `branch`), orients a ray (func_001029C0/func_00102BB0/func_00102918), stages a
// 25-unit ray descriptor and casts via func_0019AD00. On a clear first cast it
// commits that angle; on a hit it measures the squared contact distance with the
// mula.s/madd.s FMA (dx*dx+dy*dy via func_0011E748), repeats the cast at the
// opposite +/-40deg angle, and commits whichever angle has the larger clearance.
// Returns 1 when a target exists, 0 otherwise.
//
// The mula.s/madd.s FMA idiom matches exactly under 2.3.3 (not the FPU-MAC
// wall). Residual is FP-register-half coloring (target odd-half fv0f/fs0f vs
// mwcc even-half) plus the consequent load/store scheduling -- permuter/coloring
// territory, not a clean-store delay-slot nop.
extern char *func_001AA4E0(void);
extern float func_0011E620(float y, float x);
extern float func_001B1470(float a);
extern int func_00174AC0(char *p, int n);
extern float func_0011DF78(float a);
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float t);
extern void func_00102918(void *a, void *b, void *c);
extern void func_001026A0(void *a, void *b, void *c);
extern int func_0019AD00(char *p, void *a, int n);
extern float func_0011E748(float a);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

int func_0015FDF0(char *arg0) {
    char *tgt;
    float ang;
    float a0;
    float a1;
    float dx;
    float dy;
    float len0;
    int branch;

    tgt = func_001AA4E0();
    if (tgt != 0) {
    *(float *)0x700038A0 = *(float *)(tgt + 0xB0) - *(float *)(arg0 + 0xB0);
    dy = *(float *)(tgt + 0xB8) - *(float *)(arg0 + 0xB8);
    *(float *)0x700038A8 = dy;
    ang = func_001B1470(1.5707964f + func_0011E620(-dy, *(float *)0x700038A0));
    if (func_00174AC0(arg0, 2) != 0 && (int)*(unsigned char *)(arg0 + 0x23F) >= 2) {
        *(float *)0x70003A20 = func_001B1470(ang - *(float *)(arg0 + 0x218));
        if (!(func_0011DF78(*(float *)0x70003A20) < 0.69813174f)) {
            *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
            return 1;
        }
    }
    *(float *)0x70003A20 = func_001B1470(ang - *(float *)(arg0 + 0xC4));
    if (!(func_0011DF78(*(float *)0x70003A20) < 0.69813174f)) {
        return 1;
    }
    if (!(func_001B1470(ang - *(float *)(arg0 + 0xC4)) < 0.0f)) {
        a1 = func_001B1470(ang - 0.69813174f);
        branch = 0;
    } else {
        branch = 1;
        a1 = func_001B1470(0.69813174f + ang);
    }
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, a1);
    func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
    *(float *)0x700038A0 = 0.0f;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 25.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038B0, D_700036A0, D_700038A0);
    if (func_0019AD00(arg0, D_700038B0, 7) == 0) {
        *(float *)(arg0 + 0xC4) = a1;
        return 1;
    }
    *(float *)0x70003A20 = *(float *)0x700031B0 - *(float *)(arg0 + 0xB0);
    dx = *(float *)0x700031B8 - *(float *)(arg0 + 0xB8);
    *(float *)0x70003A28 = dx;
    dy = *(float *)0x70003A20;
    len0 = func_0011E748(dy * dy + dx * dx);
    if (branch == 0) {
        a0 = func_001B1470(0.69813174f + ang);
    } else {
        a0 = func_001B1470(ang - 0.69813174f);
    }
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, a0);
    func_00102918(D_700036A0, D_700036A0, arg0 + 0xB0);
    *(float *)0x700038A0 = 0.0f;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 25.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(D_700038B0, D_700036A0, D_700038A0);
    if (func_0019AD00(arg0, D_700038B0, 7) == 0) {
        *(float *)(arg0 + 0xC4) = a0;
        return 1;
    }
    *(float *)0x70003A20 = *(float *)0x700031B0 - *(float *)(arg0 + 0xB0);
    dx = *(float *)0x700031B8 - *(float *)(arg0 + 0xB8);
    *(float *)0x70003A28 = dx;
    dy = *(float *)0x70003A20;
    if (!(func_0011E748(dy * dy + dx * dx) < len0)) {
        *(float *)(arg0 + 0xC4) = a0;
    } else {
        if (branch == 0) {
            *(float *)(arg0 + 0xC4) = func_001B1470(ang - 0.69813174f);
        } else {
            *(float *)(arg0 + 0xC4) = func_001B1470(0.69813174f + ang);
        }
    }
    return 1;
    }
    return 0;
}
