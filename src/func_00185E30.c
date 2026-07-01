// NEARMISS func_00185E30  (vram 0x00185E30, 0x26C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.10% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Pure FP odd/even register coloring (fv0/fv0f/fv1/fv1f family) on the sum-of-squares distance block (same adda.s/madd.s instruction sequence as target, different register assignment), plus a downstream store-order permutation in the second delta block (0x38c0/0x38c4). Not idiom-fixable; regalloc/p...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// AI target-acceptance test for a homing/lock-on style attack (self = arg0, a
// candidate target = arg1). Bails immediately unless arg1's flag byte0 bit0 is
// set and func_00183AC0(arg1) is true and arg1's short+0x34 is nonzero. Checks
// the aim angle via func_001B1240/func_001B1470/func_0011DF78 against 90 degrees
// (1.5707964f rad); on pass, stages the target's transform into D_700038D0 via
// func_00183C40 and computes the squared distance from self+0xA0 to it (sum of
// squares via mwcc's adda.s/madd.s idiom), rejecting if >= sqrt-domain 260.0f
// via func_0011E748. It then rebuilds the delta (D_700038D0 - self's cached
// transform D_700038B0, itself populated from self->0x20 sub-object +0xA0) into
// D_700038C0, measures func_00102738 against self->0x20+0xC0 and rejects if
// <= 0.5. On success it scales the delta by 1.2f (func_00103230), composes a
// candidate transform (func_001028B8), and confirms line-of-sight/eligibility
// via two func_0019A570 checks (the second against the caller-shared target
// pointer at 0x700031D4 and a global exclusion transform D_700031B0) before
// returning arg1 as the accepted target; otherwise returns NULL.

extern float func_00102738(void *a, void *b);
extern void func_00102760(void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_00103230(void *a, void *b, float c);
extern float func_0011DF78(float x);
extern float func_0011E748(float x);
extern int func_00183AC0(void *a);
extern void func_00183C40(void *a, void *b);
extern int func_0019A570(void *a, void *b, int c, int d);
extern float func_001B1240(void *p, float a, float b);
extern float func_001B1470(float a);

extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;

char *func_00185E30(char *arg0, char *arg1) {
    char *self0 = *(char **)(arg0 + 0x20);
    char *result;

    func_00102948(&D_700038B0, self0 + 0xA0);
    result = 0;

    if (*(unsigned char *)arg1 & 1) {
        if (func_00183AC0(arg1) != 0) {
            if (*(short *)(arg1 + 0x34) != 0 &&
                func_0011DF78(func_001B1470(func_001B1240(arg0 + 0xA0, *(float *)(arg1 + 0xB0), *(float *)(arg1 + 0xB8)) - *(float *)(arg0 + 0xC4))) <= 1.5707964f) {
                func_00183C40(arg1, &D_700038D0);
                {
                    float dx = *(float *)(arg0 + 0xA0) - *(float *)0x700038D0;
                    float dy = *(float *)(arg0 + 0xA4) - *(float *)0x700038D4;
                    float dz = *(float *)(arg0 + 0xA8) - *(float *)0x700038D8;
                    if (func_0011E748((dx * dx) + (dy * dy) + (dz * dz)) < 260.0f) {
                        float ex = *(float *)0x700038D0 - *(float *)0x700038B0;
                        float ey = *(float *)0x700038D4 - *(float *)0x700038B4;
                        *(float *)0x700038C0 = ex;
                        *(float *)0x700038C4 = ey;
                        *(float *)0x700038C8 = *(float *)0x700038D8 - *(float *)0x700038B8;
                        *(int *)0x700038CC = 0x3F800000;
                        func_00102760(&D_700038A0, &D_700038C0);
                        {
                            float d = func_00102738(self0 + 0xC0, &D_700038A0);
                            *(float *)0x70003A20 = d;
                            if (!(d <= 0.5f)) {
                                func_00103230(&D_700038C0, &D_700038C0, 1.2f);
                                func_001028B8(&D_700038D0, &D_700038B0, &D_700038C0);
                                if (func_0019A570(&D_700038B0, &D_700038D0, 1, 0x20) != 0 &&
                                    arg1 == *(char **)0x700031D4 &&
                                    func_0019A570(&D_700038B0, &D_700031B0, 6, 0) == 0) {
                                    result = arg1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}
