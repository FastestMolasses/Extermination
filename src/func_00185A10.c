// NEARMISS func_00185A10  (vram 0x00185A10, 0x41C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (both the fast-path locked-target re-check and the full entity-list scan loop landed with the same shape/variable roles as the sibling functions in this AI-perception subsystem, e.g. func_00185E30.c/func_00185760.c which document the identical wall class). Remaining...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
// classic dx*dx+dy*dy+dz*dz -> func_0011E748 idiom, then require the
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
extern unsigned char D_008106C7;
extern char D_700031B0[];
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;
extern char *D_700031D4;

int func_00185A10(char *arg0) {
    char *self0 = *(char **)(arg0 + 0x20);
    int result;
    float radius;
    short n;
    char **pp;
    char *cand;

    func_00102948(&D_700038B0, self0 + 0xA0);
    result = 0;

    if (func_001B0070() & 0x80) {
        if (D_008106C7 == 0) {
            radius = 55.0f;
        } else {
            radius = 110.0f;
        }
    } else {
        radius = 110.0f;
    }
    func_00103230(&D_700038D0, self0 + 0xC0, radius);
    func_001028B8(&D_700038D0, &D_700038D0, self0 + 0xA0);
    *(int *)0x700038DC = 0x3F800000;

    if (func_0019A570(&D_700038B0, &D_700038D0, 1, 0x20) != 0) {
        cand = D_700031D4;
        if (cand != 0 && (*(unsigned char *)cand & 1) && func_00183AC0(cand) != 0 &&
            *(short *)(cand + 0x34) != 0 && func_0019A570(&D_700038B0, D_700031B0, 6, 0) == 0) {
            return *(int *)(cand + 0x14);
        }
    }

    n = D_00275B94;
    pp = (char **)D_00275B8C;
    if (n != 0) {
        do {
            cand = *pp;
            n -= 1;
            pp += 1;
            if ((*(unsigned char *)cand & 1) && func_00183AC0(cand) != 0 && *(short *)(cand + 0x34) != 0) {
                float ang = func_0011DF78(func_001B1470(func_001B1240(arg0 + 0xA0, *(float *)(cand + 0xB0), *(float *)(cand + 0xB8)) - *(float *)(arg0 + 0xC4)));
                if (ang <= 1.5707964f) {
                    func_00183C40(cand, &D_700038D0);
                    {
                        float dx = *(float *)0x700038D0 - *(float *)0x700038B0;
                        float dy = *(float *)0x700038D4 - *(float *)0x700038B4;
                        float dz = *(float *)0x700038D8 - *(float *)0x700038B8;
                        float dist = func_0011E748((dx * dx) + (dy * dy) + (dz * dz));
                        if (dist < radius) {
                            *(float *)0x700038C0 = *(float *)0x700038D0 - *(float *)0x700038B0;
                            *(float *)0x700038C4 = *(float *)0x700038D4 - *(float *)0x700038B4;
                            *(float *)0x700038C8 = *(float *)0x700038D8 - *(float *)0x700038B8;
                            *(int *)0x700038CC = 0x3F800000;
                            func_00102760(&D_700038A0, &D_700038C0);
                            *(float *)0x70003A20 = func_00102738(self0 + 0xC0, &D_700038A0);
                            {
                                float thresh;
                                if (dist <= 35.0f) {
                                    thresh = 0.7071f;
                                } else {
                                    thresh = 0.8192f;
                                }
                                if (!(*(float *)0x70003A20 < thresh) && ang <= 1.0471976f) {
                                    func_00103230(&D_700038C0, &D_700038C0, 1.2f);
                                    func_001028B8(&D_700038D0, &D_700038B0, &D_700038C0);
                                    if (func_0019A570(&D_700038B0, &D_700038D0, 1, 0x20) != 0 &&
                                        cand == D_700031D4 &&
                                        func_0019A570(&D_700038B0, D_700031B0, 6, 0) == 0) {
                                        result = *(int *)(cand + 0x14);
                                        radius = dist;
                                    }
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
