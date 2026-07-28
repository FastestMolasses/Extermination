// NEARMISS func_0013DD40  (vram 0x0013DD40, 0x988 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduling/regalloc permutation (permuter class, NOT the clean-store nop; 991202 lane best at 95.83%, 2.3.3 essentially tied). Body fully recovered, 614==614 instructions on 991202, 35 diff rows in 4 artifact clusters: (1) entry list-schedule permutation - target issues lbu st,6(a0) into a2 BEFOR...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// entity_logic: AI turn/steer behavior tick, state machine on the byte at
// pself+6 (self = actor, arg1 = paired control record).
//   state 0: reset flags/timers (arg1+0x81/0x82, speed 0.15f at +0x4C, cooldown
//     +0x20 = 120+rand&255), pick a random heading arg1+0x44 = norm(2pi*rand/255)
//     via func_001B1470, start anim clip 2 (5.0f blend) unless the current
//     masked clip id (self+0x2C & ~0x8000) is already 2; advance to 1.
//   state 1: turn-toward-heading at 1.8 deg/frame (3.6 in combat, arg1+0x70).
//     If the clip is done (func_001416D0): bump state; when the pair mode
//     (arg1+0x83) is 2, check facing error (|norm(arg1+0x44 - norm(pi +
//     self+0xC4))| <= 0.3927) and after 3 aligned checks (counter arg1+0x82,
//     signed-char wrap) re-roll a random heading. Otherwise run the steering
//     update func_001418F0: combat path probes the travel dir (func_001B1560
//     0.0349 cone, func_001B13F0 radius 40 vs D_00810360) and on obstruction
//     builds the scratch probe (0,3,50,1) -> func_001B2B10/func_001028B8; if
//     blocked at radius 15 (func_001B3250) switch to behavior 6 / state 0.
//     Non-combat: every 64th tick (frame counter words 0x70003B68+0x70003B8A)
//     LOS-test self pos (0x700038A0, y zeroed) vs pair pos (0x700038B0, y
//     zeroed) -- visible flag arg1+0x84 set on a clear radius-30 test, cleared
//     on a blocked radius-15 test (which also retargets the heading via
//     func_001B1240 toward the pair). Tail slews self+0xC4 toward arg1+0x44 by
//     pi*rate/180 (func_001B12B0).
//   state 2: sidestep scan, step 4.4 (combat) / 2.2 deg. Pair mode 2: slew
//     straight to the stored heading (rate 0.0698) and return to state 1 when
//     reached. Else rotate the heading by +-step (sign from arg1+0x83), then
//     probe the (0,3,13,1) ray through quat D_700036A0 rotated by the heading
//     (func_001029C0/func_00102BB0/func_001026A0/func_001028B8); if clear at
//     radius 20 re-probe rotated a further +-22.5 deg (spin) and when both are
//     clear commit the heading (arg1+0x44 = self+0xC4), state 1, random
//     cooldown arg1+0x24 = rand&0x3F.
// Shared tail: decrement cooldown arg1+0x24; anim frame 9 (arg1+0x30) fires SFX
// 0x7FE+rand%5 (func_001FBD50, 300.0f); while arg1+0x72 is set fire SFX 0x803
// every 40 ticks (arg1+0x28). Low altitude (arg1+0x5C <= 20): near the ground
// plane (|self.y - D_00810354| < 13.5) pick behavior 7/8 at random, else (when
// substate != 2) pick from table D_002458F0[rand&0xF]; behaviors 7/8 are
// validated against the zone record D_008102B0 (func_0021BE40 fail -> 2;
// 8 with self flag 0x80 -> 9). High altitude: count down the retry timer
// arg1+0x20 (re-armed 120+rand&255 while in combat with D_0081050C set),
// otherwise drop to behavior 1 (7-in-8) or 3.

extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001026A0(char *a, char *b, char *c);
extern void func_001028B8(char *a, char *b, char *c);
extern void func_001029C0(char *a);
extern void func_00102BB0(char *a, char *b, float f);
extern float func_0011DF78(float a);
extern int func_00122BB8(void);
extern int func_001416D0(char *a);
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

void func_0013DD40(char *pself, char *parg1) {
    float rate;
    float deg;
    float spin;
    float tmp;
    float z;
    int st;
    char *self;
    char *arg1;

    self = pself;
    arg1 = parg1;
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
        *(float *)(arg1 + 0x44) = func_001B1470(
            (6.2831855f * (float)((func_00122BB8() >> 15) & 0xFF)) / 255.0f);
        if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) != 2) {
            z = 0.0f;
            anim_clip_init(self, 2, 5.0f, z);
        }
        break;
    case 1:
        rate = 1.8f;
        if (func_001416D0(self) != 0) {
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            if (*(signed char *)(arg1 + 0x83) == 2) {
                if (func_0011DF78(func_001B1470(*(float *)(arg1 + 0x44) -
                        func_001B1470(3.1415927f + *(float *)(self + 0xC4)))) <= 0.3926991f) {
                    if ((*(signed char *)(arg1 + 0x82) =
                            *(signed char *)(arg1 + 0x82) + 1) >= 3) {
                        *(signed char *)(arg1 + 0x82) = 0;
                        *(float *)(arg1 + 0x44) = func_001B1470(
                            6.2831855f * ((float)((func_00122BB8() >> 12) & 0xFF) / 255.0f));
                    }
                }
            }
        } else {
            func_001418F0(self, arg1);
            if (*(short *)(arg1 + 0x70) != 0) {
                rate = 3.6f;
                if (!(*(float *)(arg1 + 0x5C) <= 20.0f)) {
                    if (*(int *)(arg1 + 0x24) == 0) {
                        *(float *)(arg1 + 0x44) =
                            func_001B1240(self + 0xB0, D_00810360, D_00810368);
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
                        *(float *)(arg1 + 0x44) = func_001B1240(
                            self + 0xB0, *(float *)(arg1 + 0x0), *(float *)(arg1 + 0x8));
                    }
                }
            } else {
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
                    if (func_001B13F0(D_700038A0, D_700038B0, 30.0f) == 0) {
                        *(signed char *)(arg1 + 0x84) = 1;
                    }
                }
            }
        }
        *(float *)(self + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44),
            *(float *)(self + 0xC4), (3.1415927f * rate) / 180.0f);
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
                *(float *)(self + 0xC4) = func_001B1470(
                    *(float *)(self + 0xC4) + (3.1415927f * deg) / 180.0f);
            } else {
                spin = 0.3926991f;
                *(float *)(self + 0xC4) = func_001B1470(
                    *(float *)(self + 0xC4) - (3.1415927f * deg) / 180.0f);
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
        } else {
            if ((func_00122BB8() >> 5) & 1) {
                *(unsigned char *)(self + 5) = 7;
            } else {
                *(unsigned char *)(self + 5) = 8;
            }
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
    } else {
        if (*(int *)(arg1 + 0x20) != 0) {
            *(int *)(arg1 + 0x20) = *(int *)(arg1 + 0x20) - 1;
        } else if ((*(short *)(arg1 + 0x70) != 0) && (D_0081050C != 0)) {
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
}
