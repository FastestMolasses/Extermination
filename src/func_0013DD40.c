// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: enemy "turn / sidestep" AI state machine (self = entity, arg1 = its AI block).
// State 0 arms the wander timers (speed 0.15 at +0x4C, retry timer +0x20 = 120 + rand&255),
// picks a random heading arg1+0x44 and starts clip 2 unless the masked clip id already is 2.
// State 1 turns toward that heading at 1.8 deg/frame (3.6 in combat): when the clip finishes
// it counts three aligned frames (arg1+0x82) before re-rolling the heading, otherwise it runs
// the pursuit update func_001418F0 and, in combat, probes the travel direction (0.0349 cone,
// radius 40) and drops to behaviour 6 when blocked at radius 15; out of combat it LOS-tests
// self vs the target every 64th frame and keeps the visibility flag arg1+0x84.
// State 2 sweeps the heading +-4.4 (combat) / 2.2 degrees, probing a (0,3,13,1) ray through
// quat D_700036A0 both ways, and returns to state 1 when a clear direction is found.
// The shared tail ticks the 0x24/0x28 timers, plays the idle bark when arg1+0x30 == 9, and
// when the target is within 20 units picks the melee/attack behaviour (table D_002458F0, or
// 7/8 -> 2/9 via func_0021BE40); otherwise it counts down +0x20 and falls back to 1 or 3.
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001026A0(char *a, char *b, char *c);
extern void func_001028B8(char *a, char *b, char *c);
extern void func_001029C0(char *a);
extern void func_00102BB0(char *a, char *b, float f);
extern float func_0011DF78(float a);
extern int func_00122BB8(void);
extern int func_001416D0(char *a, char *b);
extern void func_001418F0(char *a, char *b);
extern float func_001B1240(char *a, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern int func_001B13F0(void *a, void *b, float c);
extern float func_001B1470(float a);
extern int func_001B1560(char *a, void *b, float c);
extern void func_001B2B10(char *a, char *b, char *c);
extern int func_001B3250(char *a, char *b, float c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(unsigned char *a, char *b);
extern unsigned char D_002458F0[];
extern unsigned char D_008102B0;
extern float D_00810354;
extern float D_00810360;
extern float D_00810368;
extern unsigned char D_0081050C;
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_0013DD40(char *self, char *arg1) {
    float rate;
    float deg;
    float spin;
    float tmp;
    float z;
    int zi;
    unsigned char st;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        *(signed char *)(arg1 + 0x81) = 0;
        *(int *)(arg1 + 0x4C) = 0x3E19999A;
        *(int *)(arg1 + 0x20) = ((func_00122BB8() >> 12) & 0xFF) + 0x78;
        *(int *)(arg1 + 0x24) = 0;
        *(int *)(arg1 + 0x28) = 0;
        *(signed char *)(arg1 + 0x82) = 0;
        *(float *)(arg1 + 0x44) = func_001B1470((6.2831855f * (float)((func_00122BB8() >> 15) & 0xFF)) / 255.0f);
        if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) != 2) {
            zi = 0;
            z = (float)zi;
            anim_clip_init(self, 2, 5.0f, z);
        }
        break;

    case 1:
        rate = 1.8f;
        if (func_001416D0(self, arg1) != 0) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            if (*(signed char *)(arg1 + 0x83) == 2) {
                if (func_0011DF78(func_001B1470(*(float *)(arg1 + 0x44) - func_001B1470(3.1415927f + *(float *)(self + 0xC4)))) <= 0.3926991f) {
                    if ((*(signed char *)(arg1 + 0x82) = *(signed char *)(arg1 + 0x82) + 1) > 2) {
                        *(signed char *)(arg1 + 0x82) = 0;
                        *(float *)(arg1 + 0x44) = func_001B1470(6.2831855f * ((float)((func_00122BB8() >> 12) & 0xFF) / 255.0f));
                    }
                }
            }
        } else {
            func_001418F0(self, arg1);
            if (*(short *)(arg1 + 0x70) != 0) {
                rate = 3.6f;
                if (!(*(float *)(arg1 + 0x5C) <= 20.0f)) {
                    if (*(int *)(arg1 + 0x24) == 0) {
                        *(float *)(arg1 + 0x44) = func_001B1240(self + 0xB0, D_00810360, D_00810368);
                    }
                    if (func_001B1560(self, &D_00810360, 0.034906585f) != 0) {
                        if (func_001B13F0(&D_00810360, self + 0xB0, 40.0f) == 0) {
                            *(int *)0x700038A0 = 0;
                            *(int *)0x700038A4 = 0x40400000;
                            *(int *)0x700038A8 = 0x42480000;
                            *(int *)0x700038AC = 0x3F800000;
                            func_001B2B10(self, D_700038A0, D_700038A0);
                            func_001028B8(D_700038A0, self + 0xB0, D_700038A0);
                            if (func_001B3250(self, D_700038A0, 15.0f) == 0) {
                                *(unsigned char *)(self + 5) = 6;
                                *(unsigned char *)(self + 6) = 0;
                            }
                        }
                    }
                }
            } else if (*(signed char *)(arg1 + 0x84) != 0) {
                if (*(int *)(arg1 + 0x24) == 0) {
                    if (!((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F)) {
                        *(float *)0x700038A0 = *(float *)(self + 0xB0);
                        *(float *)0x700038A4 = *(float *)(self + 0xB4);
                        *(float *)0x700038A8 = *(float *)(self + 0xB8);
                        *(float *)0x700038AC = *(float *)(self + 0xBC);
                        *(float *)0x700038B0 = *(float *)(arg1 + 0x0);
                        *(float *)0x700038B4 = *(float *)(arg1 + 0x4);
                        *(float *)0x700038B8 = *(float *)(arg1 + 0x8);
                        *(float *)0x700038BC = *(float *)(arg1 + 0xC);
                        *(int *)0x700038B4 = 0;
                        *(int *)0x700038A4 = 0;
                        if (func_001B13F0(D_700038A0, D_700038B0, 15.0f) != 0) {
                            *(signed char *)(arg1 + 0x84) = 0;
                        }
                        *(float *)(arg1 + 0x44) = func_001B1240(self + 0xB0, *(float *)(arg1 + 0x0), *(float *)(arg1 + 0x8));
                    }
                }
            } else if (!((*(int *)0x70003B68 + *(short *)0x70003B8A) & 0x3F)) {
                *(float *)0x700038A0 = *(float *)(self + 0xB0);
                *(float *)0x700038A4 = *(float *)(self + 0xB4);
                *(float *)0x700038A8 = *(float *)(self + 0xB8);
                *(float *)0x700038AC = *(float *)(self + 0xBC);
                *(float *)0x700038B0 = *(float *)(arg1 + 0x0);
                *(float *)0x700038B4 = *(float *)(arg1 + 0x4);
                *(float *)0x700038B8 = *(float *)(arg1 + 0x8);
                *(float *)0x700038BC = *(float *)(arg1 + 0xC);
                *(int *)0x700038B4 = 0;
                *(int *)0x700038A4 = 0;
                if (func_001B13F0(D_700038A0, D_700038B0, 30.0f) == 0) {
                    *(signed char *)(arg1 + 0x84) = 1;
                }
            }
        }
        *(float *)(self + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(self + 0xC4), (3.1415927f * rate) / 180.0f);
        break;

    case 2:
        if (*(short *)(arg1 + 0x70) != 0) {
            deg = 4.4f;
        } else {
            deg = 2.2f;
        }
        if (*(signed char *)(arg1 + 0x83) == 2) {
            tmp = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(self + 0xC4), 0.06981317f);
            *(float *)(self + 0xC4) = tmp;
            if (tmp == *(float *)(arg1 + 0x44)) {
                *(unsigned char *)(self + 6) = 1;
            }
        } else {
            if (*(signed char *)(arg1 + 0x83) != 0) {
                spin = -0.3926991f;
                *(float *)(self + 0xC4) = func_001B1470(*(float *)(self + 0xC4) + ((3.1415927f * deg) / 180.0f));
            } else {
                spin = 0.3926991f;
                *(float *)(self + 0xC4) = func_001B1470(*(float *)(self + 0xC4) - ((3.1415927f * deg) / 180.0f));
            }
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)(self + 0xC4));
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0x40400000;
            *(int *)0x700038A8 = 0x41500000;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            func_001028B8(D_700038A0, D_700038A0, self + 0xB0);
            if (func_001B3250(self, D_700038A0, 20.0f) == 0) {
                func_00102BB0(D_700036A0, D_700036A0, spin);
                *(int *)0x700038A0 = 0;
                *(int *)0x700038A4 = 0x40400000;
                *(int *)0x700038A8 = 0x41500000;
                *(int *)0x700038AC = 0x3F800000;
                func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                func_001028B8(D_700038A0, D_700038A0, self + 0xB0);
                if (func_001B3250(self, D_700038A0, 20.0f) == 0) {
                    *(unsigned char *)(self + 6) = 1;
                    *(float *)(arg1 + 0x44) = *(float *)(self + 0xC4);
                    *(int *)(arg1 + 0x24) = (func_00122BB8() >> 18) & 0x3F;
                }
            }
        }
        break;
    }

    if (*(int *)(arg1 + 0x24) != 0) {
        *(int *)(arg1 + 0x24) = *(int *)(arg1 + 0x24) - 1;
    }
    if (*(unsigned char *)(arg1 + 0x30) == 9) {
        func_001FBD50(self, ((func_00122BB8() >> 18) % 5) + 0x7FE, 0, 300.0f);
    }
    if (*(short *)(arg1 + 0x72) != 0) {
        if (*(int *)(arg1 + 0x28) != 0) {
            *(int *)(arg1 + 0x28) = *(int *)(arg1 + 0x28) - 1;
        } else {
            *(int *)(arg1 + 0x28) = 0x28;
            func_001FBD50(self, 0x803, 0, 300.0f);
        }
    }
    if (*(float *)(arg1 + 0x5C) <= 20.0f) {
        if (!(func_0011DF78(*(float *)(self + 0xB4) - D_00810354) < 13.5f)) {
            if (*(unsigned char *)(self + 6) != 2) {
                *(unsigned char *)(self + 5) = D_002458F0[(func_00122BB8() >> 17) & 0xF];
            }
        } else if ((func_00122BB8() >> 5) & 1) {
            *(unsigned char *)(self + 5) = 7;
        } else {
            *(unsigned char *)(self + 5) = 8;
        }
        *(unsigned char *)(self + 6) = 0;
        if (*(unsigned char *)(self + 5) == 7 || *(unsigned char *)(self + 5) == 8) {
            if (func_0021BE40(&D_008102B0, self) != 0) {
                *(unsigned char *)(self + 5) = 2;
            } else if (*(unsigned char *)(self + 5) == 8) {
                if (*(unsigned char *)(self + 0xD) & 0x80) {
                    *(unsigned char *)(self + 5) = 9;
                }
            }
        }
        return;
    }
    if (*(int *)(arg1 + 0x20) != 0) {
        *(int *)(arg1 + 0x20) = *(int *)(arg1 + 0x20) - 1;
    } else if (*(short *)(arg1 + 0x70) != 0 && D_0081050C != 0) {
        *(int *)(arg1 + 0x20) = ((func_00122BB8() >> 12) & 0xFF) + 0x78;
    } else {
        if ((func_00122BB8() >> 17) & 7) {
            *(unsigned char *)(self + 5) = 1;
        } else {
            *(unsigned char *)(self + 5) = 3;
        }
        *(unsigned char *)(self + 6) = 0;
    }
}
