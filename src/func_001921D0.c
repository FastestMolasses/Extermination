// NEARMISS func_001921D0  (vram 0x001921D0, 0x1490 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 96.32% (mwcc233; 991202 = 89.50%). Logic/structure FULLY recovered: the top dispatch is a real `switch` whose SOURCE case order is the exact reverse of the target's beq compare chain (2,4,0xF,6,7,0x18,9,0x14,0x15,0x19/0xA,8->0x2C/0x2D fallthrough,0x13,0x2F,1/0x26/0x27) and every case body, the sh...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Camera follow/aim update, dispatched on the tracked actor's state
// (*(int *)(other + 0x230)).  For most states it chases the camera yaw
// (self+0x44) toward the actor's facing (other+0xC4) at a per-state rate,
// recomputes the camera eye position (self+0x10 / self+0x18) from that yaw
// and the boom length (self+0xC [+ self+0x94]), then pushes the height /
// pitch through func_00191D40 / func_00192010 and refreshes the view basis
// with func_0018D7B0 + func_0018C6A0 + func_0018C4B0.
//
// States 1, 0x26 and 0x27 (and any unhandled state) instead fall into the
// shared "free look / collision push-out" tail: `sign` is 1.0f for those
// three and 0.0f for the default case.  The tail measures the distance
// slack (D_00810690 - fabs(self+0xC)), and either pulls the camera in
// along the eye->target vector, or (when the slack is below the -20/-10
// threshold picked from self+0x64) orbits the camera around the target
// using the scratchpad vectors at 0x700038A0 / 0x700038B0, finally
// re-aiming the camera and running the auto-recentre timer at self+8.
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float angle);
extern float func_00191120(float, float, float, float);
extern void func_00191D40(unsigned char *, float, float);
extern void func_00192010(unsigned char *, float, float, float);
extern void func_0018C4B0(void *, float, float);
extern void func_0018C6A0(void *, void *, float);
extern void func_0018D7B0(unsigned char *, int);
extern float func_0011DF78(float);                  /* fabs */
extern float func_0011DE90(float);                  /* cos  */
extern float func_0011E2A8(float);                  /* sin  */
extern float func_001B1240(void *, float, float);
extern void func_00230000(unsigned char *, unsigned char *);
extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001028D0(void *, void *, void *);
extern float func_00102738(void *, void *);
extern void func_00102760(void *, void *);

extern char D_008105D0[];
extern float D_008105E0;
extern float D_008105E8;
extern float D_00810690;
extern float D_00810698;
extern float D_0081069C;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern char D_70003400[];
extern char D_70003430[];
extern char D_70003600[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_70003B50[];

void func_001921D0(unsigned char *self, unsigned char *other, int freelook, int arg3)
{
    float lim;
    float sign;
    float t;
    float zero;
    float slack;
    float ang;
    float d;

    sign = 0.0f;
    switch (*(int *)(other + 0x230)) {
    case 2:
    case 4:
    case 0xF:
        func_00230000(self, other);
        return;
    case 6:
        *(float *)(self + 0x44) = func_001B12B0(*(float *)(other + 0xC4),
                                                *(float *)(self + 0x44), 0.017453292f);
        *(signed char *)(self + 0x6C) = 0;
        *(float *)(self + 0x10) = D_008105E0
            + *(float *)(self + 0xC) * func_0011E2A8(*(float *)(self + 0x44));
        *(float *)(self + 0x18) = D_008105E8
            + *(float *)(self + 0xC) * func_0011DE90(*(float *)(self + 0x44));
        func_0018D7B0(self, 3);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 7:
        if (D_00810700 == 0x13 && D_00810701 == 1) {
            return;
        }
        *(float *)(self + 0x44) = func_001B12B0(3.1415927f + *(float *)(other + 0xC4),
                                                *(float *)(self + 0x44), 0.034906585f);
        *(signed char *)(self + 0x6C) = 0;
        *(float *)(self + 0x10) = D_008105E0
            + *(float *)(self + 0xC) * func_0011E2A8(*(float *)(self + 0x44));
        *(float *)(self + 0x18) = D_008105E8
            + *(float *)(self + 0xC) * func_0011DE90(*(float *)(self + 0x44));
        func_0018D7B0(self, 3);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 0x18:
        func_0018D7B0(self, 4);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        return;
    case 9:
        func_00191D40(self, *(float *)(self + 0x8C)
                            + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)), 1.0f);
        func_0018D7B0(self, 3);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 0x14:
        *(float *)(self + 0x44) = func_001B12B0(
            func_001B1470(3.1415927f + *(float *)(other + 0x218)),
            *(float *)(self + 0x44), 0.017453292f);
        t = func_0011E2A8(*(float *)(self + 0x44));
        *(float *)(self + 0x10) = D_008105E0
            + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
        t = func_0011DE90(*(float *)(self + 0x44));
        *(float *)(self + 0x18) = D_008105E8
            + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
        func_00191D40(self, 15.0f + (*(float *)(self + 0x8C)
                                     + (11.0f + (*(float *)(self + 0x5C)
                                                 + *(float *)(other + 0xA4)))), 1.0f);
        func_0018D7B0(self, 4);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 0x15:
        if (*(float *)(other + 0x38) != sign) {
            if (!(*(float *)(other + 0x38) < sign)) {
                *(float *)(self + 0x44) = func_001B12B0(
                    func_001B1470((3.1415927f + *(float *)(other + 0xC4)) - 0.7853982f),
                    *(float *)(self + 0x44), 0.005235988f);
            } else {
                *(float *)(self + 0x44) = func_001B12B0(
                    func_001B1470(0.7853982f + (3.1415927f + *(float *)(other + 0xC4))),
                    *(float *)(self + 0x44), 0.005235988f);
            }
            t = func_0011E2A8(*(float *)(self + 0x44));
            *(float *)(self + 0x10) = D_008105E0
                + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
            t = func_0011DE90(*(float *)(self + 0x44));
            *(float *)(self + 0x18) = D_008105E8
                + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
        } else {
            ang = func_00191120(3.1415927f + *(float *)(other + 0xC4),
                                *(float *)(self + 0x44), 0.034906585f, 0.7853982f);
            if (ang != *(float *)(self + 0x44)) {
                *(float *)(self + 0x44) = ang;
                t = func_0011E2A8(ang);
                *(float *)(self + 0x10) = D_008105E0
                    + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
                t = func_0011DE90(*(float *)(self + 0x44));
                *(float *)(self + 0x18) = D_008105E8
                    + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
            }
        }
        func_00191D40(self, 15.0f + (*(float *)(self + 0x8C)
                                     + (11.0f + (*(float *)(self + 0x5C)
                                                 + *(float *)(other + 0xA4)))), 1.0f);
        func_0018D7B0(self, 4);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 0x19:
    case 0xA:
        zero = 0.0f;
        if (*(float *)(other + 0x38) != zero) {
            if (!(*(float *)(other + 0x38) < zero)) {
                *(float *)(self + 0x44) = func_001B12B0(
                    func_001B1470(0.7853982f + *(float *)(other + 0xC4)),
                    *(float *)(self + 0x44), 0.005235988f);
            } else {
                *(float *)(self + 0x44) = func_001B12B0(
                    func_001B1470(*(float *)(other + 0xC4) - 0.7853982f),
                    *(float *)(self + 0x44), 0.005235988f);
            }
            t = func_0011E2A8(*(float *)(self + 0x44));
            *(float *)(self + 0x10) = D_008105E0
                + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
            t = func_0011DE90(*(float *)(self + 0x44));
            *(float *)(self + 0x18) = D_008105E8
                + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
        } else {
            ang = func_00191120(*(float *)(other + 0xC4), *(float *)(self + 0x44),
                                0.034906585f, 0.7853982f);
            if (ang != *(float *)(self + 0x44)) {
                *(float *)(self + 0x44) = ang;
                t = func_0011E2A8(ang);
                *(float *)(self + 0x10) = D_008105E0
                    + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
                t = func_0011DE90(*(float *)(self + 0x44));
                *(float *)(self + 0x18) = D_008105E8
                    + t * (*(float *)(self + 0xC) + *(float *)(self + 0x94));
            }
        }
        if (D_00810700 != 4) {
            func_00192010(self, *(float *)(self + 0x8C)
                                + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)),
                          15.0f, 10.0f);
        }
        func_0018D7B0(self, 3);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 8:
        if (D_00810700 == 0x13 && D_00810701 == 1) {
            return;
        }
        /* fallthrough */
    case 0x2C:
    case 0x2D:
        if (*(signed char *)(self + 0x6C) == 0) {
            *(float *)(self + 0x44) = func_001B12B0(*(float *)(other + 0xC4),
                                                    *(float *)(self + 0x44), 0.034906585f);
            if (*(float *)(self + 0x44) == *(float *)(other + 0xC4)) {
                *(signed char *)(self + 0x6C) = 1;
            }
            *(float *)(self + 0x10) = D_008105E0
                + *(float *)(self + 0xC) * func_0011E2A8(*(float *)(self + 0x44));
            *(float *)(self + 0x18) = D_008105E8
                + *(float *)(self + 0xC) * func_0011DE90(*(float *)(self + 0x44));
            func_0018D7B0(self, 3);
            func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
            func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
            return;
        }
        *(float *)(self + 0x44) = func_001B12B0(*(float *)(other + 0xC4),
                                                *(float *)(self + 0x44), 0.0034906587f);
        *(float *)(self + 0x10) = D_008105E0
            + *(float *)(self + 0xC) * func_0011E2A8(*(float *)(self + 0x44));
        *(float *)(self + 0x18) = D_008105E8
            + *(float *)(self + 0xC) * func_0011DE90(*(float *)(self + 0x44));
        func_00192010(self, *(float *)(self + 0x8C)
                            + (*(float *)(self + 0x5C) + *(float *)(other + 0xB4)),
                      25.0f, 20.0f);
        func_0018D7B0(self, 3);
        func_0018C6A0(self + 0x10, D_008105D0, 0.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 0.8f);
        return;
    case 0x13:
        *(float *)(self + 0x44) = func_001B12B0(*(float *)(other + 0xC4),
                                                *(float *)(self + 0x44), 0.017453292f);
        *(float *)(self + 0x10) = D_008105E0 + -40.0f * func_0011E2A8(*(float *)(self + 0x44));
        *(float *)(self + 0x18) = D_008105E8 + -40.0f * func_0011DE90(*(float *)(self + 0x44));
        func_00192010(self, 17.0f + (*(float *)(self + 0x8C)
                                     + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4))),
                      25.0f, sign);
        func_0018D7B0(self, 4);
        func_0018C6A0(self + 0x10, D_008105D0, 1.8f);
        func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 1.0f);
        return;
    case 0x2F:
        func_00102948(self + 0x30, D_70003B50);
        func_001029C0(D_70003400);
        func_00102C58(D_70003400, D_70003400, self + 0x30);
        *(volatile float *)0x70003600 = 0.0f;
        *(volatile float *)0x70003604 = 15.0f;
        *(volatile float *)0x70003608 = 6.0f;
        *(volatile float *)0x7000360C = 1.0f;
        func_001031E0(D_70003430, other + 0xA0);
        func_001026A0(self + 0x20, D_70003400, D_70003600);
        *(volatile float *)0x70003600 = 0.0f;
        *(volatile float *)0x70003604 = 19.0f;
        *(volatile float *)0x70003608 = -30.0f;
        *(volatile float *)0x7000360C = 1.0f;
        func_001026A0(self + 0x10, D_70003400, D_70003600);
        func_0018D7B0(self, 1);
        return;
    case 1:
    case 0x26:
    case 0x27:
        sign = 1.0f;
        break;
    }

    if (freelook == 0) {
        slack = D_00810690 - func_0011DF78(*(float *)(self + 0xC));
        *(volatile float *)0x70003A20 = slack;
        if (!(slack <= 0.0f)) {
            func_001028D0(D_700038A0, self + 0x20, self + 0x10);
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038AC = 0.0f;
            func_00102760(D_700038A0, D_700038A0);
            *(float *)(self + 0x10) = *(float *)(self + 0x10)
                + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A20;
            *(float *)(self + 0x18) = *(float *)(self + 0x18)
                + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A20;
            func_00191D40(self, *(float *)(self + 0x8C)
                                + (11.0f + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4))),
                          4.0f);
            *(unsigned char *)(self + 3) = 0;
        } else {
            if (-46.8f == *(float *)(self + 0x64)) {
                lim = -20.0f;
            } else {
                lim = -10.0f;
            }
            if (slack < lim) {
                *(volatile float *)0x700038B0 = func_0011E2A8(*(volatile float *)0x70003B54);
                *(volatile float *)0x700038B8 = func_0011DE90(*(volatile float *)0x70003B54);
                *(volatile float *)0x700038B4 = 0.0f;
                *(volatile float *)0x700038BC = 1.0f;
                func_00102760(D_700038B0, D_700038B0);
                func_001028D0(D_700038A0, self + 0x20, self + 0x10);
                *(volatile float *)0x700038A4 = 0.0f;
                *(volatile float *)0x700038AC = 1.0f;
                func_00102760(D_700038A0, D_700038A0);
                *(volatile float *)0x70003A28 = func_00102738(D_700038A0, D_700038B0);
                *(volatile float *)0x70003A24 = *(volatile float *)0x70003A20 - lim;
                if (!(D_00810690 <= 1.0f)) {
                    if (1.0f == sign) {
                        if ((*(unsigned char *)(self + 7) & 0x1F)
                            || (*(short *)(self + 0x5A) & 1)) {
                            sign = -1.0f;
                        } else if (!(D_00810698 < 23.3f)) {
                            sign = -1.0f;
                        }
                    } else if (!(*(volatile float *)0x70003A28 < 0.0f)
                               && (*(unsigned char *)(self + 7) & 0x1F)) {
                        sign = -1.0f;
                    }
                } else {
                    *(short *)(self + 0x5A) = *(short *)(self + 0x5A) & 0xFE;
                    *(unsigned char *)(self + 7) = *(unsigned char *)(self + 7) & 0xFE;
                }
                if (-1.0f != sign) {
                    if (!(D_0081069C <= 8.6f) || !(D_00810698 <= 23.3f)) {
                        if (((*(short *)(self + 0x5A) | *(unsigned char *)(self + 7)) & 1) == 0) {
                            *(float *)(self + 0x10) = *(float *)(self + 0x10)
                                + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A24;
                            *(float *)(self + 0x18) = *(float *)(self + 0x18)
                                + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A24;
                        }
                    } else {
                        if (*(unsigned char *)(self + 3) == 0) {
                            *(volatile float *)0x70003A28 = func_001B1240(self + 0x10,
                                                                          *(float *)(self + 0x20),
                                                                          *(float *)(self + 0x28));
                            *(volatile float *)0x70003A2C = *(float *)(self + 0x90)
                                - *(volatile float *)0x70003A28;
                            if (!(*(volatile float *)0x70003A2C <= 0.0f)) {
                                *(unsigned char *)(self + 3) = 1;
                            } else {
                                *(unsigned char *)(self + 3) = 2;
                            }
                        }
                        if (*(unsigned char *)(self + 3) == 1) {
                            *(volatile float *)0x70003A28 = *(volatile float *)0x70003A28
                                + 3.1415927f * (0.3f * *(volatile float *)0x70003A24) / 180.0f;
                        } else {
                            *(volatile float *)0x70003A28 = *(volatile float *)0x70003A28
                                - 3.1415927f * (0.3f * *(volatile float *)0x70003A24) / 180.0f;
                        }
                        *(volatile float *)0x700038A0 = func_0011E2A8(*(volatile float *)0x70003A28);
                        *(volatile float *)0x700038A4 = 0.0f;
                        *(volatile float *)0x700038A8 = func_0011DE90(*(volatile float *)0x70003A28);
                        *(volatile float *)0x700038AC = 0.0f;
                        *(float *)(self + 0x10) = *(float *)(self + 0x10)
                            + *(volatile float *)0x700038A0 * *(volatile float *)0x70003A24;
                        *(float *)(self + 0x18) = *(float *)(self + 0x18)
                            + *(volatile float *)0x700038A8 * *(volatile float *)0x70003A24;
                    }
                }
            }
            slack = D_00810690 - func_0011DF78(*(float *)(self + 0xC));
            *(volatile float *)0x70003A20 = slack;
            if (slack < lim) {
                if (-20.0f == lim) {
                    *(volatile float *)0x70003A24 = 0.5f * (slack - lim);
                } else {
                    d = slack - lim;
                    *(volatile float *)0x70003A24 = d;
                    if (d < -10.0f) {
                        *(volatile float *)0x70003A24 = -10.0f;
                    }
                }
                func_00191D40(self, 11.0f + (*(float *)(self + 0x8C)
                                             + (*(float *)(other + 0xA4)
                                                + (*(float *)(self + 0x5C)
                                                   - *(volatile float *)0x70003A24))), 4.0f);
            } else {
                func_00191D40(self, 11.0f + (*(float *)(self + 0x8C)
                                             + (*(float *)(self + 0x5C)
                                                + *(float *)(other + 0xA4))), 4.0f);
            }
            if (D_00810700 == 0 && (D_00810702 == 5 || D_00810702 == 6)) {
                if (*(float *)(self + 0x18) < -1449.0f) {
                    *(float *)(self + 0x18) = -1449.0f;
                }
            }
        }

        if (*(unsigned char *)(self + 7) & 9) {
            *(short *)(self + 8) = 0;
        } else if (*(int *)(other + 0x230) == 1 || *(int *)(other + 0x230) == 2) {
            *(short *)(self + 8) = *(short *)(self + 8) + 1;
            if (*(short *)(self + 8) >= 0x1E1) {
                d = func_001B1470(*(float *)(other + 0xC4) - *(float *)(self + 0x44));
                if (!(func_0011DF78(d) <= 0.05235988f)) {
                    if (d < 0.0f) {
                        if (!(*(unsigned char *)(self + 7) & 4)) {
                            *(float *)(self + 0x48) = *(float *)(other + 0xC4);
                            *(float *)(self + 0x4C) = func_0011DF78(D_0081069C);
                            *(signed char *)(self + 1) = 2;
                            *(unsigned char *)(self + 3) = 0;
                            *(float *)(self + 0x40) = 0.022222f * func_0011DF78(d);
                            if (*(float *)(self + 0x40) < 0.0034906587f) {
                                *(float *)(self + 0x40) = 0.0034906587f;
                            }
                        }
                    } else if (!(*(unsigned char *)(self + 7) & 2)) {
                        *(float *)(self + 0x48) = *(float *)(other + 0xC4);
                        *(float *)(self + 0x4C) = func_0011DF78(D_0081069C);
                        *(signed char *)(self + 1) = 2;
                        *(unsigned char *)(self + 3) = 1;
                        *(float *)(self + 0x40) = 0.022222f * func_0011DF78(d);
                        if (*(float *)(self + 0x40) < 0.0034906587f) {
                            *(float *)(self + 0x40) = 0.0034906587f;
                        }
                    }
                }
                *(short *)(self + 8) = 0;
            }
        } else {
            *(short *)(self + 8) = 0;
        }
        func_0018D7B0(self, 0);
        *(float *)(self + 0x44) = func_001B1240(D_008105D0, D_008105E0, D_008105E8);
    } else {
        *(signed char *)(self + 1) = 0;
        *(unsigned char *)(self + 3) = 0;
        *(float *)(self + 0x44) = func_001B1240(D_008105D0, D_008105E0, D_008105E8);
    }
}
