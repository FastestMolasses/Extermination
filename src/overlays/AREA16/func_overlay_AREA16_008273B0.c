// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA16 overlay, runtime 0x008273F0 (splat/link name 008273B0; overlay code is
// linked 0x40 below where it runs), 0x774 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0: +0x2E0 = 25.5 (+3 0x31) or 35.5, +0x2DC = +0x2E0 / 200,
//  +0x2D8 = 0; after func_001B0FD0: +0 = 1, +0x2E8 = the D_00275B40[2] +0x80
//  value; +0x2EC = 0 when D_00810809 > 1 (also +0x2D4 = 0) or bit 0 of +0xE,
//  else +0x2EC = 1 and +0x80 = +0x2E8 - +0x2E0; func_001C6380, func_001A2370;
//  with +0x2EC spawns a class 8 child (0x828200 behaviour) at +0xB4 - +0x2E0.
//  State 1 +5 0: +0x2E4 countdown (or D_00810789 == 0xFF) -> +5 = 1, +0x28 = 0
//  and, for +0x56 == 0, sound 0x452 (not 0x31) and three class 0x80000020
//  effects around self + 0xD0 (offsets (0, 3.6, 8), (8, 3.6, 0), (-8, 3.6,
//  0)); +5 1 moves the +0x80 value (and the child) by +0x2DC per frame for 200
//  frames, then toggles +0x2EC, sets the end values and +5 = 0 (+0x2D4 = +0x52
//  * 12 + 0xF0 when closing). +0x2D4 counts down only while D_00810809 ==
//  0xFF; at 0 with +0x2EC clear the child is retired with sound 0x453 and a
//  class 0x80000077 effect. Then func_001B1B70 and +0x4C. State 3
//  func_001AFC10.
extern unsigned char **D_00275B40;
extern unsigned char D_00810789[];
extern unsigned char D_00810809[];
extern char D_700036A0[];
extern char D_700036D0[];
extern char D_700036E0[];
extern char D_70003710[];
extern char D_70003720[];
extern float D_700038A0[];
extern float D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];
extern char D_overlay_AREA16_00828200[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *p);
extern unsigned char *func_001AFA90(int kind);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float f);
extern void func_00102BB0(void *dst, void *src, float f);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern unsigned char *func_001EFEB0(int id, void *m);
extern unsigned char *func_001EFD20(int id, void *a);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_008273B0(unsigned char *self) {
    float *f2e0;
    unsigned char **child;
    float *base;
    float *step;
    int *cnt;
    unsigned char *o;
    unsigned char *o2;
    float d;
    switch (self[4]) {
    case 0:
        if (self[3] != 0x31) {
            *(float *)(self + 0x2E0) = 35.5f;
        } else {
            *(float *)(self + 0x2E0) = 25.5f;
        }
        f2e0 = (float *)(self + 0x1F0) + 0x3C;
        child = (unsigned char **)((int *)(self + 0x1F0) + 0x3A);
        *(float *)(self + 0x2DC) = *(float *)(self + 0x2E0) / 200.0f;
        *(int *)(self + 0x2D8) = 0;
        if (func_001B0FD0(self) != 0) {
            break;
        }
        self[0] = 1;
        *(float *)(self + 0x2E8) = *(float *)(D_00275B40[2] + 0x80);
        base = (float *)(self + 0x1F0) + 0x3E;
        if (D_00810809[0] > 1) {
            *(int *)(self + 0x2EC) = 0;
            *(int *)(self + 0x2D4) = 0;
        } else if (*(unsigned short *)(self + 0xE) & 1) {
            *(int *)(self + 0x2EC) = 0;
        } else {
            *(int *)(self + 0x2EC) = 1;
            *(float *)(D_00275B40[2] + 0x80) = *base - *f2e0;
        }
        func_001C6380(self);
        func_001A2370(self, D_00275B40[2] + 0x90);
        if (*(int *)(self + 0x2EC) != 0) {
            o = func_001AFA90(8);
            if (o != 0) {
                *child = o;
                o[3] = 0;
                *(short *)(o + 0x2E) = 0;
                if (self[3] != 0x31) {
                    o[0xD] = 6;
                } else {
                    o[0xD] = 0x25;
                }
                *(unsigned short *)(o + 0xE) = 0xFF00;
                *(short *)(o + 0x56) = 0;
                *(short *)(o + 0x54) = 0;
                *(float *)(o + 0xB0) = *(float *)(self + 0xB0);
                *(float *)(o + 0xB4) = *(float *)(self + 0xB4) - *f2e0;
                *(float *)(o + 0xB8) = *(float *)(self + 0xB8);
                *(float *)(o + 0xC0) = *(float *)(self + 0xC0);
                *(float *)(o + 0xC4) = *(float *)(self + 0xC4);
                *(float *)(o + 0xC8) = *(float *)(self + 0xC8);
                *(char **)(o + 0x10) = D_overlay_AREA16_00828200;
            }
        }
        break;
    case 1:
        switch (self[5]) {
        case 0:
            cnt = (int *)(self + 0x1F0) + 0x3D;
            if (*(int *)(self + 0x2E4) != 0) {
                (*cnt)--;
                if (*cnt <= 0 || D_00810789[0] == 0xFF) {
                    *cnt = 0;
                    self[5] = 1;
                    *(short *)(self + 0x28) = 0;
                    if (*(short *)(self + 0x56) == 0) {
                        if (self[3] != 0x31) {
                            func_001FBD50(self, 0x452, 0, 300.0f);
                        }
                        func_00102958(D_700036A0, self + 0xD0);
                        D_700038A0[0] = 0.0f;
                        D_700038A0[1] = 3.6f;
                        D_700038A0[2] = 8.0f;
                        D_700038A0[3] = 1.0f;
                        func_001026A0(D_700038B0, D_700036A0, D_700038A0);
                        D_700038A0[0] = 8.0f;
                        D_700038A0[1] = 3.6f;
                        D_700038A0[2] = 0.0f;
                        D_700038A0[3] = 1.0f;
                        func_001026A0(D_700038C0, D_700036A0, D_700038A0);
                        D_700038A0[0] = -8.0f;
                        D_700038A0[1] = 3.6f;
                        D_700038A0[2] = 0.0f;
                        D_700038A0[3] = 1.0f;
                        func_001026A0(D_700038D0, D_700036A0, D_700038A0);
                        func_001029C0(D_70003720);
                        func_00102B08(D_70003720, D_70003720, -1.2217306f);
                        func_001026D0(D_700036A0, D_700036A0, D_70003720);
                        func_00102948(D_700036D0, D_700038B0);
                        func_001EFEB0(0x80000020, D_700036A0);
                        func_001029C0(D_70003720);
                        func_00102BB0(D_70003720, D_70003720, 1.5707964f);
                        func_001026D0(D_700036E0, D_70003720, D_700036A0);
                        func_00102948(D_70003710, D_700038C0);
                        func_001EFEB0(0x80000020, D_700036E0);
                        func_001029C0(D_70003720);
                        func_00102BB0(D_70003720, D_70003720, -1.5707964f);
                        func_001026D0(D_700036E0, D_70003720, D_700036A0);
                        func_00102948(D_70003710, D_700038D0);
                        func_001EFEB0(0x80000020, D_700036E0);
                    }
                }
            }
            break;
        case 1:
            (*(short *)(self + 0x28))++;
            if (*(short *)(self + 0x28) < 200 && D_00810789[0] != 0xFF) {
                if (*(int *)(self + 0x2EC) != 0) {
                    step = (float *)(self + 0x1F0) + 0x3B;
                    *(float *)(D_00275B40[2] + 0x80) += *(float *)(self + 0x2DC);
                    o = *(unsigned char **)(self + 0x2D8);
                    if (o != 0) {
                        *(float *)(o + 0xB4) += *step;
                    }
                } else {
                    step = (float *)(self + 0x1F0) + 0x3B;
                    *(float *)(D_00275B40[2] + 0x80) -= *(float *)(self + 0x2DC);
                    o = *(unsigned char **)(self + 0x2D8);
                    if (o != 0) {
                        *(float *)(o + 0xB4) -= *step;
                    }
                }
            } else {
                *(int *)(self + 0x2EC) = !*(int *)(self + 0x2EC);
                if (*(int *)(self + 0x2EC) != 0) {
                    f2e0 = (float *)(self + 0x1F0) + 0x3C;
                    *(float *)(D_00275B40[2] + 0x80) = *(float *)(self + 0x2E8) - *(float *)(self + 0x2E0);
                    o = *(unsigned char **)(self + 0x2D8);
                    if (o != 0) {
                        *(float *)(o + 0xB4) = *(float *)(self + 0xB4) - *f2e0;
                    }
                } else {
                    *(int *)(self + 0x2D4) = *(unsigned short *)(self + 0x52) * 12 + 0xF0;
                    *(float *)(D_00275B40[2] + 0x80) = *(float *)(self + 0x2E8);
                    o = *(unsigned char **)(self + 0x2D8);
                    if (o != 0) {
                        *(float *)(o + 0xB4) = *(float *)(self + 0xB4);
                    }
                }
                self[5] = 0;
            }
            func_001C6380(self);
            func_001A2370(self, D_00275B40[2] + 0x90);
            break;
        }
        cnt = (int *)(self + 0x1F0) + 0x39;
        if (*(int *)(self + 0x2D4) != 0) {
            if (D_00810809[0] == 0xFF) {
                (*cnt)--;
            }
            if (*cnt == 0 && *(int *)(self + 0x2EC) == 0) {
                o2 = *(unsigned char **)(self + 0x2D8);
                child = (unsigned char **)((int *)(self + 0x1F0) + 0x3A);
                if (o2 != 0) {
                    o2[4] = 3;
                    *child = 0;
                    func_00102948(D_700038B0, self + 0xB0);
                    func_001FBD50(self, 0x453, 0, 300.0f);
                    if (self[3] != 0x31) {
                        D_700038B0[1] += 20.0f;
                        func_001EFD20(0x80000077, D_700038B0);
                    } else {
                        D_700038B0[1] += 15.0f;
                        func_001EFD20(0x80000077, D_700038B0);
                    }
                }
            }
        }
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
