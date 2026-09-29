// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x00824560 (splat/link name 00824520; overlay code is
// linked 0x40 below where it runs), 0x18BC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role (read from the instructions): a placement-record driven object. REC is
// its record, D_0024D7C0[area][sub] + uid (+0x9A) * 0x28. Two 4-point
// polygons are copied from 0x827640 / 0x827680 and tested against the player
// position D_00810350 with func_001B1EA0.
//   State 0: +0 = 1, +0x2EC = +0x2E4 = 0, +0x2E0 = 0.002; func_001BA1C0(self,
//     0x10) set -> +0xD = 12, +0x2D0 = 0, pose and model id from REC,
//     func_001B0FD0, state 2 / sub 1; else func_001B0FD0, state 1 when bit 5
//     of D_00810845 is set (else 4), +0x2D0 = 1; then func_001C6380 and
//     func_001A2370(self, self + 0xD0).
//   State 4: while +0x2EC is set it advances the +0x2DC phase by 0.08
//     (wrapping to -pi) and sets +0xC8 / +0xB4 from REC and func_0011E2A8;
//     otherwise, with the player at y >= 55 inside polygon 1, it starts a
//     240-frame run (+0x2EC = 1). During a run: restore the REC pose at the
//     end, every 64 frames a random chance of a class 0x8000001F spawn
//     (func_001EFD20) near the object. Bit 5 of D_00810845 moves it to state
//     1. Then func_001B1B70 and the +0x4C method.
//   State 1, sub 0: player at y >= 55 inside polygon 2, D_008106B9 == 0 and
//     D_008102B5 < 2 or in 29..34 -> script 0x827180 (func_001BA1A0),
//     +0x2EC = 30, +0x2E8 = 6, +0x2E4 = 18, sub 1, D_00810768 = 1; the player
//     y/z are saved at +0x2D8/+0x2D4 and D_00810350 set to (-270, 55, -583),
//     D_00810374 to +-pi/2 by its sign, func_001FBD50(self, 0x8CC, 0, 300.0f).
//   State 1, sub 1: while +0x2E8 is set a 0.1 phase step drives +0xC8/+0xB4
//     and the player y (D_00810354); every 64 frames with +0x2E8 >= 2 a class
//     0x8000001F spawn; at +0x2E4 <= 0 +0x2E8 counts down (pose restore and a
//     0x80000021 spawn at 0, two spawns at 1, else +0x2E4 = 18). With +0x2E8
//     clear, +0x2EC > 0 counts down a lift (0.0002 * (31 - n)^2) and at zero
//     sets -50, +0x2E0 = 0.6 and spawns 0x80000015/0x80000021 at (-277, 43,
//     -582); +0x2EC < 0 counts up a fall with fixed-point spawns at -30, -25,
//     -20, -14 and -3, and at 0 reloads the pose from REC, func_001AF800,
//     func_001CB5B0(+9), func_001B0FD0, state 2 / sub 0. Then func_001BA1F0,
//     func_001C6380, func_001A2370, func_001B1B70 and the +0x4C method.
//   State 2: sub 0 sets D_00810768 = 0xFF and spawns six 0x80000015 objects
//     around +0xB0, then sub 1 runs func_001BA1F0 while +0x2D0 is set; then
//     func_001C6380, func_001B1B70 and the +0x4C method.
//   State 3 and any other value: func_001AFC10.
// Matching notes: REC must be int arithmetic (uid * 0x28 + (int)row) for the
// operand order; blk = self + 0x1F0 is assigned before the two polygon
// copies (fixes the copy registers); the 0x700038A0 decrement in state 4 and
// the 0x70003680 store in state 1 are relocated externs (idiom-32); the
// player-y test of state 1 sub 0 reads the D_00810354 extern; `<= 1` / `> 1`
// respellings pick $at (idiom-28).
typedef struct { float v[16]; } Poly __attribute__((aligned(16)));
#define SPF(a) (*(float *)(a))
#define REC ((unsigned char *)(self[0x9A] * 0x28 + (int)D_0024D7C0[D_00810700][D_00810701]))
#define RF(o) (*(float *)(REC + (o)))
#define F(o) (*(float *)(self + (o)))
#define I(o) (*(int *)(self + (o)))
extern unsigned char **D_0024D7C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810845;
extern unsigned char D_008106B9;
extern unsigned char D_008102B5;
extern unsigned char D_00810768;
extern float D_00810350[];
extern float D_00810354;
extern float D_700038A0[];
extern float D_70003680;
extern Poly D_overlay_AREA06_00827640;
extern Poly D_overlay_AREA06_00827680;
extern char D_overlay_AREA06_00827180[];
extern int func_001BA1C0(unsigned char *self, int n);
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern float func_0011E2A8(float a);
extern int func_001B1EA0(int a0, float *pos, void *poly, int n);
extern int func_00122BB8(void);
extern void func_00102948(void *dst, void *src);
extern void func_001EFD20(int id, void *pos);
extern void func_001B1B70(unsigned char *self);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001AF800(unsigned char *self);
extern void func_001CB5B0(int a0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA06_00824520(unsigned char *self) {
    Poly pa;
    Poly pb;
    unsigned char *blk;
    int *cnt;
    int *tmr;
    float *ang;
    int x;
    int v;
    blk = self + 0x1F0;
    pa = D_overlay_AREA06_00827640;
    pb = D_overlay_AREA06_00827680;
    switch (self[4]) {
    case 0:
        self[0] = 1;
        I(0x2EC) = 0;
        I(0x2E4) = 0;
        F(0x2E0) = 0.002f;
        if (func_001BA1C0(self, 0x10) != 0) {
            self[0xD] = 0xC;
            I(0x2D0) = 0;
            *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 0x2E);
            F(0xC0) = RF(0x40);
            F(0xC4) = RF(0x44);
            F(0xC8) = RF(0x48);
            F(0xB0) = RF(0x34);
            F(0xB4) = RF(0x38);
            F(0xB8) = RF(0x3C);
            func_001B0FD0(self);
            self[4] = 2;
            self[5] = 1;
        } else {
            func_001B0FD0(self);
            if (D_00810845 & 0x20) {
                self[4] = 1;
            } else {
                self[4] = 4;
            }
            I(0x2D0) = 1;
        }
        func_001C6380(self);
        func_001A2370(self, self + 0xD0);
        break;
    case 4:
        cnt = (int *)(blk + 0xFC);
        if (I(0x2EC) != 0) {
            ang = (float *)(blk + 0xEC);
            F(0x2DC) += 0.08f;
            if (!(F(0x2DC) <= 3.1415927f)) {
                *ang = -3.1415927f;
            }
            F(0xC8) = RF(0x20) + F(0x2E0) * func_0011E2A8(*ang);
            F(0xB4) = RF(0x10) - 45.0f * func_0011E2A8(F(0xC8));
        } else if (!(SPF(0x810354) < 55.0f) && func_001B1EA0(0, D_00810350, &pa, 4) == 1) {
            *cnt = 1;
            F(0x2DC) = 0.0f;
            I(0x2E4) = 0xF0;
        }
        if (*cnt != 0) {
            tmr = (int *)(self + 0x1F0) + 0x3D;
            I(0x2E4)--;
            if (I(0x2E4) <= 0) {
                *cnt = 0;
                F(0xC4) = RF(0x1C);
                F(0xC8) = RF(0x20);
                F(0xB4) = RF(0x10);
                F(0xB8) = RF(0x14);
            }
            if ((*tmr & 0x3F) == 0) {
                x = func_00122BB8() >> 16;
                x *= 2;
                x >>= 15;
                if (x != 0) {
                    func_00102948(D_700038A0, self + 0xB0);
                    D_700038A0[0] -= 46.0f;
                    SPF(0x700038A4) = 1.5f + (RF(0x10) - 90.0f * func_0011E2A8(F(0xC8)));
                    x = func_00122BB8() >> 16;
                    x *= 256;
                    x >>= 15;
                    SPF(0x700038A8) += 0.0156862735748291f * (float)x - 1.0f;
                    func_001EFD20(0x8000001F, D_700038A0);
                }
            }
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
        }
        if (D_00810845 & 0x20) {
            self[4] = 1;
        }
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (!(D_00810354 < 55.0f) && func_001B1EA0(0, D_00810350, &pb, 4) == 1 &&
                D_008106B9 == 0 &&
                (D_008102B5 <= 1 || (D_008102B5 >= 29 && D_008102B5 < 35))) {
                func_001BA1A0(blk, D_overlay_AREA06_00827180);
                I(0x2EC) = 30;
                I(0x2E8) = 6;
                I(0x2E4) = 18;
                self[5] = 1;
                D_00810768 = 1;
                F(0x2D8) = SPF(0x810354);
                F(0x2D4) = SPF(0x810358);
                F(0x2E0) = 0.006f;
                SPF(0x810350) = -270.0f;
                SPF(0x810354) = 55.0f;
                SPF(0x810358) = -583.0f;
                if (SPF(0x810374) > 0.0f) {
                    SPF(0x810374) = 1.5707964f;
                } else {
                    SPF(0x810374) = -1.5707964f;
                }
                func_001FBD50(self, 0x8CC, 0, 300.0f);
            }
            break;
        case 1:
            cnt = (int *)(blk + 0xF8);
            if (I(0x2E8) != 0) {
                ang = (float *)(blk + 0xEC);
                F(0x2DC) += 0.1f;
                if (!(F(0x2DC) <= 3.1415927f)) {
                    *ang = -3.1415927f;
                }
                F(0xC8) = RF(0x20) + F(0x2E0) * (1.0f + func_0011E2A8(*ang));
                F(0xB4) = RF(0x10) - 45.0f * func_0011E2A8(F(0xC8));
                SPF(0x810354) = F(0x2D8) - (45.0f + (F(0xB0) - SPF(0x810360))) * func_0011E2A8(F(0xC8));
                tmr = (int *)(self + 0x1F0) + 0x3D;
                I(0x2E4)--;
                if ((I(0x2E4) & 0x3F) == 0 && *cnt > 1) {
                    func_00102948(D_700038A0, self + 0xB0);
                    x = func_00122BB8() >> 16;
                    x *= 90;
                    x >>= 15;
                    SPF(0x700038A0) += (float)x - 45.0f;
                    SPF(0x700038A4) += 1.5f;
                    func_001EFD20(0x8000001F, D_700038A0);
                }
                if (*tmr <= 0) {
                    (*cnt)--;
                    if (*cnt <= 0) {
                        F(0xC4) = RF(0x1C);
                        F(0xC8) = RF(0x20);
                        F(0xB4) = RF(0x10);
                        F(0xB8) = RF(0x14);
                        func_00102948(D_700038A0, self + 0xB0);
                        SPF(0x700038A0) -= 30.0f;
                        func_001EFD20(0x80000021, D_700038A0);
                    } else {
                        if (*cnt == 1) {
                            func_00102948(D_700038A0, self + 0xB0);
                            SPF(0x700038A0) += 40.0f;
                            func_001EFD20(0x80000015, D_700038A0);
                            SPF(0x700038A0) = F(0xB0) - 10.0f;
                            SPF(0x700038A4) = F(0xB4) - 10.0f;
                            SPF(0x700038A8) = 4.0f + F(0xB8);
                            func_001EFD20(0x80000021, D_700038A0);
                        }
                        *tmr = 0x12;
                    }
                }
            } else {
                cnt = (int *)(blk + 0xFC);
                v = I(0x2EC);
                if (v > 0) {
                    D_70003680 = (float)(31 - v);
                    F(0xC8) = 0.0002f * (SPF(0x70003680) * SPF(0x70003680)) + RF(0x20);
                    F(0xB4) = RF(0x10) - 45.0f * func_0011E2A8(F(0xC8));
                    SPF(0x810354) = F(0x2D8) - (45.0f + (F(0xB0) - SPF(0x810360))) * func_0011E2A8(F(0xC8));
                    (*cnt)--;
                    if (*cnt == 0) {
                        *cnt = -50;
                        F(0x2E0) = 0.6f;
                        SPF(0x700038A0) = -277.0f;
                        SPF(0x700038A4) = 43.0f;
                        SPF(0x700038A8) = -582.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        func_001EFD20(0x80000021, D_700038A0);
                    }
                } else if (v < 0) {
                    F(0x2E0) -= 0.04f;
                    F(0xC8) -= 0.006f;
                    F(0xB4) = F(0xB4) + F(0x2E0);
                    if (*cnt < -25) {
                        SPF(0x810354) = 5.0f + F(0xB4);
                    } else {
                        SPF(0x810354) = 8.0f + F(0xB4);
                    }
                    (*cnt)++;
                    v = *cnt;
                    if (v == 0) {
                        self[0xD] = REC[0x2C];
                        *(unsigned short *)(self + 0xE) = *(unsigned short *)(REC + 0x2E);
                        F(0xC0) = RF(0x40);
                        F(0xC4) = RF(0x44);
                        F(0xC8) = RF(0x48);
                        F(0xB0) = RF(0x34);
                        F(0xB4) = RF(0x38);
                        F(0xB8) = RF(0x3C);
                        func_001AF800(self);
                        func_001CB5B0(self[9]);
                        func_001B0FD0(self);
                        self[4] = 2;
                        self[5] = 0;
                    } else if (v == -30) {
                        SPF(0x700038A0) = -208.0f;
                        SPF(0x700038A4) = 28.0f;
                        SPF(0x700038A8) = -594.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        func_001EFD20(0x80000021, D_700038A0);
                    } else if (v == -25) {
                        SPF(0x700038A0) = -258.0f;
                        SPF(0x700038A4) = 28.0f;
                        SPF(0x700038A8) = -584.0f;
                        func_001EFD20(0x80000021, D_700038A0);
                        SPF(0x700038A0) += 45.0f;
                        func_001EFD20(0x80000021, D_700038A0);
                        SPF(0x700038A0) = F(0xB0);
                        SPF(0x700038A4) = F(0xB4);
                        SPF(0x700038A8) = F(0xB8);
                        func_001EFD20(0x80000015, D_700038A0);
                    } else if (v == -20) {
                        SPF(0x700038A0) = -228.0f;
                        SPF(0x700038A4) = 28.0f;
                        SPF(0x700038A8) = -594.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        func_001EFD20(0x80000021, D_700038A0);
                        SPF(0x700038A0) = -268.0f;
                        SPF(0x700038A4) = 28.0f;
                        SPF(0x700038A8) = -594.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                    } else if (v == -14) {
                        SPF(0x700038A0) = -258.0f;
                        SPF(0x700038A4) = 28.0f;
                        SPF(0x700038A8) = -594.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        func_001EFD20(0x80000021, D_700038A0);
                    } else if (v == -3) {
                        SPF(0x700038A0) = -277.0f;
                        SPF(0x700038A4) = 28.0f;
                        SPF(0x700038A8) = -594.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        func_00102948(D_700038A0, self + 0xB0);
                        SPF(0x700038A0) -= 35.0f;
                        SPF(0x700038A4) += 3.0f;
                        SPF(0x700038A8) -= 20.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        SPF(0x700038A0) += 10.0f;
                        SPF(0x700038A4) -= 2.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                        SPF(0x700038A0) += 10.0f;
                        func_001EFD20(0x80000015, D_700038A0);
                    }
                }
            }
            func_001BA1F0(self);
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
            break;
        }
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
        switch (self[5]) {
        case 0:
            D_00810768 = 0xFF;
            self[5] = 1;
            func_00102948(D_700038A0, self + 0xB0);
            SPF(0x700038A0) -= 45.0f;
            SPF(0x700038A4) -= 4.0f;
            SPF(0x700038A8) -= 10.0f;
            func_001EFD20(0x80000015, D_700038A0);
            SPF(0x700038A0) += 10.0f;
            SPF(0x700038A4) += 1.0f;
            func_001EFD20(0x80000015, D_700038A0);
            SPF(0x700038A0) += 10.0f;
            SPF(0x700038A4) -= 2.0f;
            func_001EFD20(0x80000015, D_700038A0);
            SPF(0x700038A0) += 10.0f;
            func_001EFD20(0x80000015, D_700038A0);
            SPF(0x700038A0) += 10.0f;
            SPF(0x700038A4) += 2.0f;
            func_001EFD20(0x80000015, D_700038A0);
            SPF(0x700038A0) += 10.0f;
            SPF(0x700038A4) += 2.0f;
            func_001EFD20(0x80000015, D_700038A0);
        case 1:
            if (I(0x2D0) != 0) {
                func_001BA1F0(self);
            }
            break;
        }
        func_001C6380(self);
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
