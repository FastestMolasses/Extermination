// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.10% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad vectors D_700038A0/B0/C0/D0 are float[4] arrays accessed by
//    element, not literal float addresses: the delta D_700038C0 = D_700038D0 -
//    D_700038B0 is three direct element assignments, which gives the original's
//    computation and store order (the old temps reordered it).
//  - The distance-block locals are declared dz, dy, dx (first declared gets the
//    lowest FP register), which fixed the FP register colouring.
//
// AI target-acceptance test for a homing/lock-on style attack (self = arg0, a
// candidate target = arg1). Bails immediately unless arg1's flag byte0 bit0 is
// set and func_00183AC0(arg1) is true and arg1's short+0x34 is nonzero. Checks
// the aim angle via func_001B1240/func_001B1470/func_0011DF78 against 90 degrees
// (1.5707964f rad); on pass, stages the target's transform into D_700038D0 via
// func_00183C40 and computes the squared distance from self+0xA0 to it (sum of
// squares), rejecting if >= sqrt-domain 260.0f
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
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_700038D0[4];

char *func_00185E30(char *arg0, char *arg1) {
    char *self0 = *(char **)(arg0 + 0x20);
    char *result;

    func_00102948(D_700038B0, self0 + 0xA0);
    result = 0;

    if (*(unsigned char *)arg1 & 1) {
        if (func_00183AC0(arg1) != 0) {
            if (*(short *)(arg1 + 0x34) != 0 &&
                func_0011DF78(func_001B1470(func_001B1240(arg0 + 0xA0, *(float *)(arg1 + 0xB0), *(float *)(arg1 + 0xB8)) - *(float *)(arg0 + 0xC4))) <= 1.5707964f) {
                func_00183C40(arg1, D_700038D0);
                {
                    float dz, dy, dx;
                    dx = *(float *)(arg0 + 0xA0) - D_700038D0[0];
                    dy = *(float *)(arg0 + 0xA4) - D_700038D0[1];
                    dz = *(float *)(arg0 + 0xA8) - D_700038D0[2];
                    if (func_0011E748((dx * dx) + (dy * dy) + (dz * dz)) < 260.0f) {
                        D_700038C0[0] = D_700038D0[0] - D_700038B0[0];
                        D_700038C0[1] = D_700038D0[1] - D_700038B0[1];
                        D_700038C0[2] = D_700038D0[2] - D_700038B0[2];
                        D_700038C0[3] = 1.0f;
                        func_00102760(D_700038A0, D_700038C0);
                        {
                            float d = func_00102738(self0 + 0xC0, D_700038A0);
                            *(float *)0x70003A20 = d;
                            if (!(d <= 0.5f)) {
                                func_00103230(D_700038C0, D_700038C0, 1.2f);
                                func_001028B8(D_700038D0, D_700038B0, D_700038C0);
                                if (func_0019A570(D_700038B0, D_700038D0, 1, 0x20) != 0 &&
                                    arg1 == *(char **)0x700031D4 &&
                                    func_0019A570(D_700038B0, &D_700031B0, 6, 0) == 0) {
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
