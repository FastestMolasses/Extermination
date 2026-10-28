// COMPILER: mwcc233
// CFLAGS: -O4,p

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.68% NEARMISS).
// Fixes read from the original instructions:
//  - wrong body in state 2: after func_001B1560 succeeds, a second
//    func_001B13F0 range test (40.0) gates the probe; the scratchpad probe
//    vector is (0, 3, 70, 1), not (3, 70, 1); func_001B3250 takes (p, vec, r).
//  - state 0: anim_clip_init starts clip 2 / clip 7 (the same constant the
//    clip test at p+0x2C compares against), and p[6] advances before the call.
//  - callee prototypes: func_001B13F0 is (a, b, float); D_00810360 is a
//    4-float vector (x at [0], z at [2]), not two small gp-relative scalars;
//    the 0x70003B68/0x70003B8A scratchpad words are relocated externs.
//  - the anim-row table is addressed as an integer base plus row*32 plus the
//    pre-scaled column; both timers at e+0x50/e+0x54 are unsigned shorts.
//
// 3-state actor tick keyed on p[6]. State 0 seeds anim fields, RNG-picks an
// anim duration from the 2x16 table D_00244F30 (row = e[0x61]) and starts a
// clip; state 1 waits for the e+0x58 bit 0x8000 to clear then advances;
// state 2 runs the steering/contact update (func_001333F0/func_00132FB0, the
// scratchpad probe at 0x700038A0/B0, func_001B1240/13F0/1560/3250). The
// shared tail decrements the e+0x50 timer; after the switch, when e[0x69] bit 0 is set, the
// e+0x54 SFX timer counts down (firing func_001EFD90 0x8000001D when it runs out).
extern int func_00122BB8(void);
extern float func_001B1470(float);
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(int self, int a, int b, float c);
extern int func_001333F0(unsigned char *p, unsigned char *e);
extern int func_00132FB0(unsigned char *p, unsigned char *e);
extern int func_001B13F0(void *a, void *b, float r);
extern float func_001B1240(unsigned char *origin, float x, float z);
extern void func_00131F20(unsigned char *p, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B3250(unsigned char *p, void *v, float r);
extern int func_001B1560(unsigned char *p, void *v, float r);
extern void func_00133640(unsigned char *p, unsigned char *e);
extern void func_00131F90(unsigned char *p, unsigned char *e);
extern void func_001EFD90(int mode, void *b, void *c);
extern void func_00132490(unsigned char *p, unsigned char *e);

extern unsigned short D_00244F30[];
extern float D_00810360[4];
extern int D_70003B68[16];
extern short D_70003B8A[16];
extern char D_700038A0[];
extern char D_700038B0[];

void func_0012F100(unsigned char *p, unsigned char *e) {
    int t;

    switch (p[6]) {
    case 0:
        e[0x63] = 0;
        *(short *)(e + 0x54) = 0;
        *(int *)(e + 0x3C) = 0;
        *(float *)(e + 0x30) =
            func_001B1470(6.2831855f * (float)((func_00122BB8() >> 13) & 0xFF) / 255.0f);
        t = ((func_00122BB8() >> 7) & 0xF) * 2;
        *(unsigned short *)(e + 0x50) =
            *(unsigned short *)(t + ((unsigned int)D_00244F30 + e[0x61] * 32));
        if (e[0x61] != 0) {
            if (*(short *)(p + 0x2C) == 2) {
                p[6] = 2;
                *(int *)(e + 0x3C) = 0x3DCCCCCD;
            } else {
                p[6] = p[6] + 1;
                anim_clip_init((int)p, 2, 10.0f, 0.0f);
            }
        } else {
            if (*(short *)(p + 0x2C) == 7) {
                p[6] = 2;
            } else {
                p[6] = p[6] + 1;
                anim_clip_init((int)p, 7, 10.0f, 0.0f);
            }
        }
        break;
    case 1:
        if (*(unsigned short *)(e + 0x58) & 0x8000) {
            break;
        }
        p[6] = p[6] + 1;
        *(int *)(e + 0x3C) = 0x3DCCCCCD;
        /* fall through */
    case 2:
        if (e[0x61] != 0) {
            if (*(float *)(p + 0x3C) == 95.0f || *(float *)(p + 0x3C) == 43.0f) {
                func_001FBD50((int)p, 0x7DE, 0, 300.0f);
            }
        }
        if (func_001333F0(p, e) == 0 && (p[0xD] & 3) == 3 && e[0x61] == 0) {
            p[5] = 9;
            p[6] = 0;
            break;
        }
        if (*(unsigned short *)(e + 0x56) != 0 && e[0x61] != 0) {
            p[5] = 0;
            p[6] = 0;
        }
        if (e[0x62] != 0) {
            e[0x62] = e[0x62] - 1;
            if (*(float *)(e + 0x30) == *(float *)(p + 0xC4)) {
                e[0x62] = 0;
            }
        } else if (func_00132FB0(p, e) == 0) {
            if (*(unsigned short *)(e + 0x56) != 0 && e[0x61] == 0) {
                if (func_001B13F0(D_00810360, p + 0xB0, 15.0f) == 0) {
                    *(float *)(e + 0x30) =
                        func_001B1240(p + 0xB0, D_00810360[0], D_00810360[2]);
                }
                if (func_001B1560(p, D_00810360, 0.034906585f) != 0 &&
                    func_001B13F0(D_00810360, p + 0xB0, 40.0f) == 0) {
                    *(float *)0x700038A0 = 0.0f;
                    *(float *)0x700038A4 = 3.0f;
                    *(float *)0x700038A8 = 70.0f;
                    *(float *)0x700038AC = 1.0f;
                    func_00131F20(p, D_700038A0, D_700038A0);
                    func_001028B8(D_700038A0, D_700038A0, p + 0xB0);
                    if (func_001B3250(p, D_700038A0, 15.0f) == 0) {
                        p[5] = 2;
                        p[6] = 0;
                    }
                }
            } else {
                if (((D_70003B68[0] + D_70003B8A[0]) & 0x3F) == 0) {
                    *(float *)0x700038A0 = *(float *)(p + 0xB0);
                    *(float *)0x700038A4 = *(float *)(p + 0xB4);
                    *(float *)0x700038A8 = *(float *)(p + 0xB8);
                    *(float *)0x700038AC = *(float *)(p + 0xBC);
                    *(float *)0x700038B0 = *(float *)(e + 0x0);
                    *(float *)0x700038B4 = *(float *)(e + 0x4);
                    *(float *)0x700038B8 = *(float *)(e + 0x8);
                    *(float *)0x700038BC = *(float *)(e + 0xC);
                    *(int *)0x700038B4 = 0;
                    *(int *)0x700038A4 = 0;
                    if (e[0x68] != 0) {
                        if (func_001B13F0(D_700038A0, D_700038B0, 15.0f) != 0) {
                            e[0x68] = 0;
                        }
                        *(float *)(e + 0x30) =
                            func_001B1240(p + 0xB0, *(float *)(e + 0x0), *(float *)(e + 0x8));
                    } else if (func_001B13F0(D_700038A0, D_700038B0, 30.0f) == 0) {
                        e[0x68] = 1;
                    }
                }
            }
        }
        /* .L0012F5C0 tail */
        if (--*(unsigned short *)(e + 0x50) == 0) {
            p[5] = 0;
            p[6] = 0;
        }
        if (p[5] == 1) {
            func_00133640(p, e);
        }
        func_00131F90(p, e);
        break;
    }

    if (e[0x69] & 1) {
        if (*(unsigned short *)(e + 0x54) != 0) {
            *(unsigned short *)(e + 0x54) = *(unsigned short *)(e + 0x54) - 1;
        } else {
            *(short *)(e + 0x54) = ((func_00122BB8() >> 9) & 0xF) + 7;
            *(float *)0x700038A0 = *(float *)(p + 0xB0);
            *(float *)0x700038A4 = *(float *)(p + 0xB4);
            *(float *)0x700038A8 = *(float *)(p + 0xB8);
            *(int *)0x700038AC = 0x3F800000;
            *(float *)0x700038A4 = *(float *)(e + 0x40);
            func_001EFD90(0x8000001D, D_700038A0, p + 0xC0);
        }
    }
    func_00132490(p, e);
}
