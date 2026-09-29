// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00825800 (splat/link name 008257C0; overlay code is
// linked 0x40 below where it runs), 0x998 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0 (after func_001B0FD0, func_001C6380) goes to 1 with
//  func_0019C6F0(0x28, 1) unless D_0081078B == 0xFF (state 2,
//  func_001B6660(0x828C90)), keeps +0x2EC = D_00810358, calls
//  func_001CA5F0(self, 3), func_001D8BF0(self, 1), +0x80..8C = (-120, -120,
//  -120, 0), func_001DAFA0, func_001DB240(0), +0x2D8 = -1. State 1 moves the
//  player's height (func_001FC520 / func_001FC3C0 sound 0x924, the D_00810358
//  height moves with 0x826A50, D_700031F0 = 1), then by the low bits of
//  D_0081080B goes to 0xC8 (script 0x82A3C0, sound 0x929) or, below 455 with a
//  downward speed, 0xC9 (script 0x82A1C0). 0xC9 raises D_00810358 by 1 up to
//  472 and copies D_00810360 to D_008105E0 / D_00810200 until the script ends
//  (state 1). 0xC8 raises it up to 480, copies the same and at frame 79 fills
//  0x82A5E0; +5 1 waits for +0x36, then state 2, D_0081080B = D_0081078B =
//  0xFF, func_001B6660, sound 0x92A, +0x2E0 = 452, +0x2DC = 5. State 2 emits
//  func_001EFD20 effects every 8th frame of D_70003B68 while +0x2DC counts
//  down (+0x2E0 -= 8.4). State 3 func_001AFC10.
extern unsigned char D_0081078B;
extern unsigned char D_0081080B;
extern unsigned char D_008102B5;
extern float D_00810350[];
extern float D_00810360[];
extern float D_008105E0[];
extern float D_00810200[];
extern int D_700031F0;
extern int D_70003B68;
extern float D_700038A0[];
extern float D_overlay_AREA16_0082A5E0[];
extern char D_overlay_AREA16_00828C90[];
extern char D_overlay_AREA16_0082A3C0[];
extern char D_overlay_AREA16_0082A1C0[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_0019C6F0(int a0, int a1);
extern unsigned char *func_001B6660(void *p);
extern void func_001CA5F0(unsigned char *self, int mode);
extern void func_001D8BF0(void *a0, int a1);
extern void func_001DAFA0(void);
extern void func_001DB240(int a0);
extern void func_001DB250(void);
extern void func_001DB480(void);
extern void func_001FC520(void *blk);
extern void func_001FC3C0(unsigned char *self, void *blk, int id, float f12, float f13);
extern void func_overlay_AREA16_00826A50(unsigned char *self, float *a1, float f);
extern unsigned char *func_001EFD20(int id, void *a);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_008257C0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    float *p;
    int *cnt;
    float *p2;
    int d;
    float t;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            func_001C6380(self);
            if (D_0081078B != 0xFF) {
                self[0] = 1;
                self[4] = 1;
                func_0019C6F0(0x28, 1);
            } else {
                self[4] = 2;
                func_001B6660(D_overlay_AREA16_00828C90);
            }
            *(float *)(self + 0x2EC) = D_00810350[2];
            *(float *)(self + 0x2E4) = 0.0f;
            *(float *)(self + 0x2E8) = 0.0f;
            func_001CA5F0(self, 3);
            func_001D8BF0(self, 1);
            *(float *)(self + 0x88) = -120.0f;
            *(float *)(self + 0x84) = -120.0f;
            *(float *)(self + 0x80) = -120.0f;
            *(float *)(self + 0x8C) = 0.0f;
            func_001DAFA0();
            func_001DB240(0);
            *(int *)(self + 0x2D8) = -1;
        }
        break;
    case 1:
        func_001DB250();
        func_001DB480();
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        if ((D_0081080B & 7) > 4) {
            func_001FC520((int *)(self + 0x1F0) + 0x3A);
        } else {
            /* matching device: 4096.0 staged through an int */
            { int zi = 4096; float b = (float)zi; func_001FC3C0(self, (int *)(self + 0x1F0) + 0x3A, 0x924, 200.0f, b); }
        }
        p = (float *)(self + 0x1F0) + 0x3F;
        if (!(*(float *)(self + 0x2EC) < 459.0f) && D_00810350[2] <= 459.0f) {
            D_700038A0[0] = D_00810350[0];
            D_700038A0[1] = D_00810350[1];
            D_700038A0[2] = 459.0f;
            func_overlay_AREA16_00826A50(self, D_700038A0, *(float *)(self + 0x2E4));
        }
        if (D_00810350[1] < 205.0f && D_00810350[2] > 400.0f) {
            if (D_00810350[2] < 430.0f) {
                t = *(float *)(self + 0x2E4) - 0.81f;
                D_700031F0 = 1;
                D_00810350[2] = 430.0f - t;
            } else if (D_00810350[2] < 466.0f) {
                func_001DB240(1);
                if (D_008102B5 < 2) {
                    D_00810350[2] -= *(float *)(self + 0x2E4) - 0.81f;
                    if (D_00810350[0] > 311.0f) {
                        D_00810350[0] -= 0.1f;
                    }
                    if (D_00810350[0] < 297.0f) {
                        D_00810350[0] += 0.1f;
                    }
                } else {
                    D_00810350[2] -= *(float *)(self + 0x2E4) - 0.18f;
                }
                D_700031F0 = 1;
            } else {
                func_001DB240(0);
            }
            *(float *)(self + 0x2E8) = *p - D_00810350[2];
            *(float *)(self + 0x2E4) = (*(float *)(self + 0x2E8) + *(float *)(self + 0x2E4)) / 2.0f;
            *p = D_00810350[2];
        }
        d = D_0081080B;
        if ((d & 7) >= 3) {
            if (!(d & 4)) {
                func_001EFD20(1, D_700038A0);
            }
            func_001DB240(3);
            if ((D_0081080B & 7) < 4) {
                func_001FBD50(self, 0x929, 0, 300.0f);
            }
            self[4] = 0xC8;
            *(short *)(self + 0x28) = 0;
            self[5] = 0;
            func_0019C6F0(0x28, 0);
            D_0081080B |= 4;
            if (D_00810350[2] < 475.0f) {
                func_001BA1A0(blk, D_overlay_AREA16_0082A3C0);
            } else {
                self[5] = 1;
            }
        } else if (D_00810350[2] < 455.0f) {
            if ((*(float *)(self + 0x2E4) < -0.055f && D_008102B5 == 1) ||
                *(float *)(self + 0x2E4) < -1.35f) {
                self[4] = 0xC9;
                func_001BA1A0(blk, D_overlay_AREA16_0082A1C0);
            }
        }
        break;
    case 0xC9:
        if (D_00810350[2] < 472.0f) {
            if (D_00810350[2] <= 459.0f && D_00810350[2] >= 458.0f) {
                D_700038A0[0] = D_00810350[0];
                D_700038A0[1] = D_00810350[1];
                D_700038A0[2] = 459.0f;
                func_overlay_AREA16_00826A50(self, D_700038A0, -0.8f);
            }
            D_00810350[2] += 1.0f;
        }
        if (func_001BA1F0(self) != 0) {
            self[4] = 1;
            func_001DB240(0);
        } else {
            D_00810200[0] = D_008105E0[0] = D_00810360[0];
            D_00810200[1] = D_008105E0[1] = D_00810360[1];
            D_00810200[2] = D_008105E0[2] = D_00810360[2];
        }
        func_001DB250();
        func_001DB480();
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 0xC8:
        switch (self[5]) {
        case 0:
            if (D_00810350[2] < 480.0f) {
                D_700031F0 = 1;
                D_00810350[2] += 1.0f;
            }
            if (func_001BA1F0(self) != 0) {
                self[5] = 1;
                *(short *)(self + 0x36) = 0;
            } else {
                (*(short *)(self + 0x28))++;
                if (*(short *)(self + 0x28) < 80) {
                    D_00810200[0] = D_008105E0[0] = D_00810360[0];
                    D_00810200[1] = D_008105E0[1] = D_00810360[1];
                    D_00810200[2] = D_008105E0[2] = D_00810360[2];
                    if (*(short *)(self + 0x28) == 79) {
                        D_overlay_AREA16_0082A5E0[0] = D_00810360[0];
                        D_overlay_AREA16_0082A5E0[1] = 19.0f + D_00810360[1];
                        D_overlay_AREA16_0082A5E0[2] = 46.8f + D_00810360[2];
                        D_overlay_AREA16_0082A5E0[4] = D_00810360[0];
                        D_overlay_AREA16_0082A5E0[5] = D_00810360[1];
                        D_overlay_AREA16_0082A5E0[6] = D_00810360[2];
                    }
                }
            }
            break;
        case 1:
            if (*(short *)(self + 0x36) != 0) {
                self[4] = 2;
                D_0081080B = 0xFF;
                D_0081078B = 0xFF;
                func_001B6660(D_overlay_AREA16_00828C90);
                func_001FBD50(self, 0x92A, 0, 300.0f);
                *(float *)(self + 0x2E0) = 452.0f;
                *(int *)(self + 0x2DC) = 5;
            }
            break;
        }
        func_001B1B70(self);
        func_001DB250();
        func_001DB480();
        break;
    case 2:
        cnt = (int *)(self + 0x1F0) + 0x3B;
        if (*(int *)(self + 0x2DC) > 0) {
            if (D_70003B68 % 8 == 0) {
                D_700038A0[0] = 304.0f;
                D_700038A0[1] = 188.0f;
                p2 = (float *)(self + 0x1F0) + 0x3C;
                D_700038A0[2] = *p2;
                D_700038A0[3] = 1.0f;
                func_001EFD20(0x8000003F, D_700038A0);
                func_001EFD20(3, D_700038A0);
                (*cnt)--;
                *p2 += -8.4f;
            }
            if (*cnt > 3) {
                func_001DB250();
                func_001DB480();
                (*(void (**)(unsigned char *))(self + 0x4C))(self);
            }
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
