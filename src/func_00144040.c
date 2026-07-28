// NEARMISS func_00144040  (vram 0x00144040, 0xBD4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.79% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (757 insns, 70 diff rows, all clusters are documented compiler artifacts; ~10 source-reshape iterations exhausted): (1) case-1 bc1f delay slot — mwcc233 (and 991202) speculate the else-block's scratchpad-address 'lui at,0x7000' into the bc1f slot and retarget the br...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Boss/enemy AI update: state machine on the entity state byte self[6]
// (states 0..6 plus 0x63 = "abort/retreat"). `self` is the entity, `arg1` its
// per-instance AI work area. Uses the scratchpad vectors at 0x700038A0 /
// 0x700038B0 (treated as volatile — the original keeps all accesses in source
// order) to aim at / test line-of-sight to the player object D_008102B0.
//
//   state 0: init work vars, play clip 5 when not already in move 5
//   state 1: if target within 40 units and angle test passes -> approach
//            (clip 0x10), optionally leap (clip 0x18) when flag self[0xD]&0x80;
//            otherwise turn toward the target point and chase its heading
//   state 2: approach; at <=71 units try to latch onto the player
//            (func_0021BE40 + func_001A7B80), storing the grab point in
//            D_00810320..2C; at <=47 units either abort (0x63) or attack clip 5
//   state 3: close approach variant (thresholds 17/8/4); on a blocked ray it
//            throws the player off (func_0021BF90, clip 0x19) and enters the
//            "carried" pose; otherwise grabs (clip 0x10)
//   state 4: track the carry point each frame; count down arg1+0x20, then
//            clip 0x16 with sink rate -0.05
//   state 5: carried/lift phase: raise the target point by +10 units while
//            arg1+0x4C velocity is nonzero; release the player
//            (func_0021C040) once below 84 units; at <=74 units begin descent
//   state 6: on landed flag (arg1[0x74]&2) reset velocity; on ground contact
//            (arg1+0x30 bit 0x1000) return to state 0
//   state 0x63: retreat — clip 6, speed 2.0, sink -0.05, go to state 4

extern void anim_clip_init(unsigned char *self, int clip, float a, float b);
extern int func_001B1560(unsigned char *self, float *v, float ang);
extern void func_001B2B10(unsigned char *self, void *a, void *b);
extern void func_001028B8(void *out, void *a, void *b);
extern int func_001B3250(unsigned char *self, void *v, float r);
extern float func_001B1240(void *pos, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float angle);
extern void func_001FBD50(unsigned char *self, int sound, int a2, float radius);
extern int func_0021BE40(unsigned char *player, unsigned char *self);
extern int func_0021BED0(unsigned char *player);
extern void func_0021BF90(unsigned char *player, unsigned char *self);
extern void func_0021C040(unsigned char *player, unsigned char *self);
extern int func_001A7B80(unsigned char *self);
extern void func_001B55E0(unsigned char *self, int a);
extern void func_00102948(void *dst, void *src);
extern int func_0019AFE0(unsigned char *self, void *from, void *to, int mask);
extern void func_00182F90(unsigned char *player, void *pt);
extern int func_00146F80(unsigned char *self);
extern unsigned char D_008102B0;
extern unsigned char D_008102BF;
extern float D_00810320;
extern float D_00810324;
extern float D_00810328;
extern float D_0081032C;
extern float D_00810360;
extern float D_00810368;
extern float D_00810374;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern float D_00245950;
extern float D_700038A0;
extern float D_700038B0;

void func_00144040(unsigned char *self, unsigned char *arg1) {
    unsigned char st = self[6];

    switch (st) {
    case 0:
        self[6] = st + 1;
        *(int *)(arg1 + 0x20) = 0;
        *(int *)(arg1 + 0x24) = 0;
        arg1[0x75] = 0;
        *(int *)(arg1 + 0x40) = 0x3F800000; /* 1.0f  */
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0x3F933333; /* 1.15f */
        if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) != 5) {
            anim_clip_init(self, 5, 0.0f, 0.0f);
        }
        break;
    case 1: {
        float yaw;
        if (*(float *)(arg1 + 0x5C) <= 40.0f &&
            func_001B1560(self, &D_00810360, 0.174532935f) != 0) {
            self[6] = self[6] + 1;
            *(int *)(arg1 + 0x40) = 0x40000000; /* 2.0f */
            anim_clip_init(self, 0x10, 5.0f, 20.0f);
            if (self[0xD] & 0x80) {
                *(volatile int *)0x700038A0 = 0;
                *(volatile int *)0x700038A4 = 0x40400000; /*  3.0f */
                *(volatile int *)0x700038A8 = 0x42480000; /* 50.0f */
                *(volatile int *)0x700038AC = 0x3F800000; /*  1.0f */
                func_001B2B10(self, &D_700038A0, &D_700038A0);
                func_001028B8(&D_700038A0, self + 0xB0, &D_700038A0);
                if (func_001B3250(self, &D_700038A0, 20.0f) == 0) {
                    self[6] = 3;
                    anim_clip_init(self, 0x18, 5.0f, 23.0f);
                }
            }
            break;
        }
        *(volatile int *)0x700038A0 = 0;
        *(volatile int *)0x700038A4 = 0x40400000; /*  3.0f */
        *(volatile int *)0x700038A8 = 0x41900000; /* 18.0f */
        *(volatile int *)0x700038AC = 0x3F800000; /*  1.0f */
        func_001B2B10(self, &D_700038A0, &D_700038A0);
        func_001028B8(&D_700038A0, self + 0xB0, &D_700038A0);
        if (*(short *)(arg1 + 0x62) == 0 ||
            func_001B3250(self, &D_700038A0, 20.0f) != 0 ||
            (*(char *)(arg1 + 0x74) & 1)) {
            self[6] = 0x63;
        } else if (func_001B1560(self, &D_00810360, 1.39626348f) == 0) {
            self[5] = 2;
            self[6] = 2;
        }
        yaw = func_001B1240(self + 0xB0, D_00810360, D_00810368);
        *(float *)(arg1 + 0x44) = yaw;
        *(float *)(self + 0xC4) = func_001B12B0(yaw, *(float *)(self + 0xC4), 0.017453292f);
        break;
    }
    case 2:
        if (*(int *)(arg1 + 0x24) == 0 && *(float *)(self + 0x3C) <= 84.0f) {
            *(int *)(arg1 + 0x24) = 1;
            func_001FBD50(self, 0x837, 0, 300.0f);
        }
        if (!(*(short *)(self + 0x2C) & 0x8000)) {
            if (*(float *)(self + 0x3C) <= 75.0f) {
                *(int *)(arg1 + 0x40) = 0x3F800000; /* 1.0f */
                if (*(int *)(arg1 + 0x20) == 0 && *(float *)(self + 0x3C) <= 71.0f &&
                    func_0021BE40(&D_008102B0, self) == 0 &&
                    func_001A7B80(self) != 0) {
                    float t0, t1, t2, t3;
                    unsigned char b;
                    *(int *)(arg1 + 0x20) = 1;
                    *(volatile int *)0x700038A0 = 0;
                    *(volatile int *)0x700038A4 = 0;
                    b = D_008102B0;
                    *(volatile int *)0x700038A8 = 0x3F800000; /* 1.0f */
                    *(volatile int *)0x700038AC = 0x3F800000; /* 1.0f */
                    D_008102B0 = b | 2;
                    D_008102BF = 2;
                    func_001B2B10(self, &D_700038A0, &D_700038A0);
                    t0 = *(volatile float *)0x700038A0;
                    t1 = *(volatile float *)0x700038A4;
                    t2 = *(volatile float *)0x700038A8;
                    t3 = *(volatile float *)0x700038AC;
                    D_00810320 = t0;
                    D_00810324 = t1;
                    D_00810328 = t2;
                    D_0081032C = t3;
                    D_008104D4 = 0x41C80000; /* 25.0f */
                    func_001B55E0(self, 1);
                }
            }
            if (*(float *)(self + 0x3C) <= 47.0f || (*(int *)(arg1 + 0x30) & 0x1000)) {
                if (*(int *)(arg1 + 0x20) != 0) {
                    self[6] = 0x63;
                } else {
                    self[5] = 2;
                    self[6] = 2;
                    *(int *)(arg1 + 0x40) = 0x3F800000; /* 1.0f */
                    anim_clip_init(self, 5, 5.0f, 0.0f);
                }
            }
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0x24) == 0 && *(float *)(self + 0x3C) <= 17.0f) {
            *(int *)(arg1 + 0x24) = 1;
            func_001FBD50(self, 0x837, 0, 300.0f);
        }
        if (*(int *)(arg1 + 0x20) == 0 && !(*(short *)(self + 0x2C) & 0x8000) &&
            *(float *)(self + 0x3C) <= 8.0f) {
            *(int *)(arg1 + 0x40) = 0x3F800000; /* 1.0f */
            if (*(float *)(self + 0x3C) <= 4.0f && *(int *)(arg1 + 0x20) == 0 &&
                func_0021BED0(&D_008102B0) == 0 && func_001A7B80(self) != 0) {
                *(int *)(arg1 + 0x20) = 1;
                *(volatile int *)0x700038A0 = 0;
                *(volatile int *)0x700038A4 = 0x40400000; /*  3.0f */
                *(volatile int *)0x700038A8 = 0x42480000; /* 50.0f */
                *(volatile int *)0x700038AC = 0x3F800000; /*  1.0f */
                func_001B2B10(self, &D_700038A0, &D_700038A0);
                func_001028B8(&D_700038A0, self + 0xB0, &D_700038A0);
                if (func_001B3250(self, &D_700038A0, 20.0f) != 0) {
                    float t0, t1, t2, t3;
                    unsigned char b;
                    self[6] = 2;
                    *(volatile int *)0x700038A0 = 0;
                    *(volatile int *)0x700038A4 = 0;
                    b = D_008102B0;
                    *(volatile int *)0x700038A8 = 0x3F800000; /* 1.0f */
                    *(volatile int *)0x700038AC = 0x3F800000; /* 1.0f */
                    D_008102B0 = b | 2;
                    D_008102BF = 2;
                    func_001B2B10(self, &D_700038A0, &D_700038A0);
                    t0 = *(volatile float *)0x700038A0;
                    t1 = *(volatile float *)0x700038A4;
                    t2 = *(volatile float *)0x700038A8;
                    t3 = *(volatile float *)0x700038AC;
                    D_00810320 = t0;
                    D_00810324 = t1;
                    D_00810328 = t2;
                    D_0081032C = t3;
                    D_008104D4 = 0x41C80000; /* 25.0f */
                    anim_clip_init(self, 0x10, 5.0f, 48.0f);
                } else {
                    self[6] = self[6] + 1;
                    *(int *)(arg1 + 0x20) = 0x64;
                    func_0021BF90(&D_008102B0, self);
                    arg1[0x7B] = 1;
                    D_00810374 = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
                    anim_clip_init(self, 0x19, 0.0f, 0.0f);
                    func_00102948(&D_700038A0, &D_00245950);
                    func_001B2B10(self, &D_700038A0, &D_700038A0);
                    func_001028B8(&D_700038A0, self + 0xB0, &D_700038A0);
                    func_00182F90(&D_008102B0, &D_700038A0);
                }
                break;
            }
        }
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            self[5] = 2;
            self[6] = 2;
            *(int *)(arg1 + 0x40) = 0x3F800000; /* 1.0f */
            anim_clip_init(self, 5, 5.0f, 0.0f);
        }
        break;
    case 4: {
        int t;
        func_00102948(&D_700038A0, &D_00245950);
        func_001B2B10(self, &D_700038A0, &D_700038A0);
        func_001028B8(&D_700038A0, self + 0xB0, &D_700038A0);
        func_00182F90(&D_008102B0, &D_700038A0);
        if (func_00146F80(self) != 0 || (*(char *)(arg1 + 0x74) & 1)) {
            *(int *)(arg1 + 0x20) = 0;
        }
        t = *(int *)(arg1 + 0x20);
        if (t != 0) {
            *(int *)(arg1 + 0x20) = t - 1;
        } else {
            self[6] = self[6] + 1;
            *(int *)(arg1 + 0x50) = 0xBD4CCCCD; /* -0.05f */
            anim_clip_init(self, 0x16, 0.0f, 0.0f);
        }
        break;
    }
    case 5:
        if (!(*(float *)(arg1 + 0x4C) == 0.0f)) {
            float lift;
            func_00102948(&D_700038A0, self + 0xB0);
            *(volatile int *)0x700038B0 = 0;
            *(volatile int *)0x700038B4 = 0;
            lift = *(volatile float *)0x700038A4 + 10.0f;
            *(volatile int *)0x700038B8 = 0x42200000; /* 40.0f */
            *(volatile int *)0x700038BC = 0x3F800000; /*  1.0f */
            *(volatile float *)0x700038A4 = lift;
            func_001B2B10(self, &D_700038B0, &D_700038B0);
            func_001028B8(&D_700038B0, &D_700038A0, &D_700038B0);
            if (func_0019AFE0(self, &D_700038A0, &D_700038B0, 6) != 0 ||
                *(float *)(arg1 + 0x4C) < 0.0f) {
                *(int *)(arg1 + 0x4C) = 0;
                *(int *)(arg1 + 0x50) = 0;
            }
        }
        if (*(char *)(arg1 + 0x7B) != 0 && !(*(short *)(self + 0x2C) & 0x8000) &&
            *(float *)(self + 0x3C) <= 84.0f) {
            arg1[0x7B] = 0;
            D_008102B0 = D_008102B0 | 2;
            if (D_0081070A != 0) {
                D_008104D4 = 0x420C0000; /* 35.0f */
            } else {
                D_008104D4 = 0x41F00000; /* 30.0f */
            }
            func_0021C040(&D_008102B0, self);
            func_001B55E0(self, 1);
        }
        if (*(char *)(arg1 + 0x7B) != 0) {
            func_00102948(&D_700038A0, &D_00245950);
            func_001B2B10(self, &D_700038A0, &D_700038A0);
            func_001028B8(&D_700038A0, self + 0xB0, &D_700038A0);
            func_00182F90(&D_008102B0, &D_700038A0);
        } else if (*(float *)(self + 0x3C) <= 74.0f) {
            self[6] = self[6] + 1;
            *(int *)(arg1 + 0x48) = 0x3FC00000; /*  1.5f  */
            *(int *)(arg1 + 0x4C) = 0xBF333333; /* -0.7f  */
            *(int *)(arg1 + 0x50) = 0;
            arg1[0x7A] = 1;
        }
        break;
    case 6:
        if (*(char *)(arg1 + 0x74) & 2) {
            *(int *)(arg1 + 0x4C) = 0;
            if (*(int *)(arg1 + 0x30) & 0x1000) {
                self[5] = 0;
                self[6] = 0;
                *(short *)(arg1 + 0x64) = 0;
                arg1[0x7A] = 0;
            }
        }
        break;
    case 0x63:
        self[5] = 2;
        self[6] = 4;
        *(int *)(arg1 + 0x40) = 0x40000000; /*  2.0f  */
        *(int *)(arg1 + 0x50) = 0xBD4CCCCD; /* -0.05f */
        anim_clip_init(self, 6, 0.0f, 0.0f);
        break;
    }
}
