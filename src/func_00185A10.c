// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.20% NEARMISS).
// Fixes read from the original instructions:
//  - Wrong body: the cone-radius distance is measured from self's own position
//    (arg0+0xA0..0xA8) minus the staged transform D_700038D0, not from the
//    cached copy D_700038B0 (as in the byte-matched twin func_00185E30).
//  - -sdatathreshold 8: the entity list D_00275B8C/D_00275B94 are gp-relative;
//    the remaining small externs are over-declared as arrays to stay absolute.
//  - The loop counter is an int (no 16-bit sign extension on decrement).
//  - Scratchpad vectors are float[4] arrays accessed by element; the dot result
//    lives in a relocated extern D_70003A20 (a literal address let the upper
//    half be hoisted into a branch delay slot).
//  - Locals declared pp, result, self0, n, cand, dist, radius, ang (saved
//    register colouring), and the facing angle is assigned inside its test.
//
// AI target-acquisition scan for actor arg0 (self). Copies self's position
// (arg0+0x20's +0xA0) into scratch D_700038B0, then builds a "look-ahead"
// point (D_700038D0) by rotating self's forward vector (+0xC0) out by a
// difficulty-dependent cone half-angle (110.0f normally; 55.0f only on easy
// mode when D_008106C7 is clear) and adding it to self's position. If that
// look-ahead point has line of sight to self (func_0019A570), and the
// currently-locked target (D_700031D4) is still alive/facing/visible (and
// not itself excluded by func_0019A570 against D_700031B0), keep it locked
// and return its id (+0x14). Otherwise scan the global entity list
// (D_00275B8C/D_00275B94): for each live, hostile, "has-model" (+0x34!=0)
// candidate within a 90-degree facing cone, compute its transform via
// func_00183C40, reject ones farther than the cone radius (var_f21) via the
// classic dx*dx+dy*dy+dz*dz -> func_0011E748 idiom (delta measured from
// self's own +0xA0 position to the staged transform), then require the
// candidate be roughly in front (func_00102738 dot >= 0.7071/0.8192 by
// distance) and within 60 degrees (1.0471976f) before re-confirming line of
// sight through a widened look-ahead point (func_00103230 scale 1.2f); the
// first candidate that also matches the still-locked target and clears the
// exclusion check wins and becomes the new result/radius.
extern float func_00102738(void *a, void *b);
extern void func_00102760(void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_00103230(void *a, void *b, float angle);
extern float func_0011DF78(float x);
extern float func_0011E748(float x);
extern int func_00183AC0(void *a);
extern void func_00183C40(void *a, void *b);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_001B0070(void);
extern float func_001B1240(void *p, float a, float b);
extern float func_001B1470(float a);

extern char **D_00275B8C;
extern short D_00275B94;
extern unsigned char D_008106C7[];
extern float D_700031B0[4];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_700038D0[4];
extern float D_70003A20[4];

int func_00185A10(char *arg0) {
    char **pp;
    int result;
    char *self0;
    int n;
    char *cand;
    float dist;
    float radius;
    float ang;

    self0 = *(char **)(arg0 + 0x20);
    func_00102948(D_700038B0, self0 + 0xA0);
    result = 0;

    if (func_001B0070() & 0x80) {
        if (D_008106C7[0] == 0) {
            radius = 55.0f;
        } else {
            radius = 110.0f;
        }
    } else {
        radius = 110.0f;
    }
    func_00103230(D_700038D0, self0 + 0xC0, radius);
    func_001028B8(D_700038D0, D_700038D0, self0 + 0xA0);
    D_700038D0[3] = 1.0f;

    if (func_0019A570(D_700038B0, D_700038D0, 1, 0x20) != 0) {
        cand = *(char **)0x700031D4;
        if (cand != 0 && (*(unsigned char *)cand & 1) && func_00183AC0(cand) != 0 &&
            *(short *)(cand + 0x34) != 0 && func_0019A570(D_700038B0, D_700031B0, 6, 0) == 0) {
            return *(int *)(cand + 0x14);
        }
    }

    n = D_00275B94;
    pp = D_00275B8C;
    if (n != 0) {
        do {
            cand = *pp;
            n -= 1;
            pp += 1;
            if ((*(unsigned char *)cand & 1) && func_00183AC0(cand) != 0 && *(short *)(cand + 0x34) != 0) {
                if ((ang = func_0011DF78(func_001B1470(func_001B1240(arg0 + 0xA0, *(float *)(cand + 0xB0), *(float *)(cand + 0xB8)) - *(float *)(arg0 + 0xC4)))) <= 1.5707964f) {
                    func_00183C40(cand, D_700038D0);
                    {
                        float dz, dy, dx;
                        dx = *(float *)(arg0 + 0xA0) - D_700038D0[0];
                        dy = *(float *)(arg0 + 0xA4) - D_700038D0[1];
                        dz = *(float *)(arg0 + 0xA8) - D_700038D0[2];
                        dist = func_0011E748((dx * dx) + (dy * dy) + (dz * dz));
                        if (dist < radius) {
                            float thresh;
                            D_700038C0[0] = D_700038D0[0] - D_700038B0[0];
                            D_700038C0[1] = D_700038D0[1] - D_700038B0[1];
                            D_700038C0[2] = D_700038D0[2] - D_700038B0[2];
                            D_700038C0[3] = 1.0f;
                            func_00102760(D_700038A0, D_700038C0);
                            D_70003A20[0] = func_00102738(self0 + 0xC0, D_700038A0);
                            if (dist <= 35.0f) {
                                thresh = 0.7071f;
                            } else {
                                thresh = 0.8192f;
                            }
                            if (!(D_70003A20[0] < thresh) && ang <= 1.0471976f) {
                                func_00103230(D_700038C0, D_700038C0, 1.2f);
                                func_001028B8(D_700038D0, D_700038B0, D_700038C0);
                                if (func_0019A570(D_700038B0, D_700038D0, 1, 0x20) != 0 &&
                                    cand == *(char **)0x700031D4 &&
                                    func_0019A570(D_700038B0, D_700031B0, 6, 0) == 0) {
                                    result = *(int *)(cand + 0x14);
                                    radius = dist;
                                }
                            }
                        }
                    }
                }
            }
        } while (n != 0);
    }
    return result;
}
