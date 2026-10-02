// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA07 overlay, runtime 0x00823F80 (splat/link name 00823F40; overlay code
//  is linked 0x40 below where it runs), 0x14F0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA07; lane OVLC).
// Role: sub 2 deferred group 0x825F80 member (x4); the AREA01 0x826D40 C
//  (state 0x64 and a story-flag test, here flag 0x15) with the AREA03
//  0x823930 hit branch (hit point and colour in D_70003600 / D_70003610,
//  effect 0x80000003 for every kind but 1). Flag 0x15 inverts the AREA01 use:
//  set at spawn gives state 0x64 (func_001C6380, func_001B17A0 and the +0x4C
//  method only); state 2 runs its turn-back only while the flag is clear; at
//  the end of states 4 and 1 the flag sends it to 0x64 and zeroes the +0x220
//  child's +0xA0..+0xAC. Calls 0x825470 and 0x825940.
typedef struct {
    float f1F0;
    float f1F4;
    float f1F8;
    float f1FC;
    int f200;
    unsigned char *f204;
    int f208;
    int f20C;
    float f210;
    int f214;
    float f218;
    int f21C;
    unsigned char *f220;
    int f224;
} Work;

#define W ((Work *)(self + 0x1F0))
#define S16(o) (*(short *)(self + (o)))
#define SPF(a) (*(float *)(a))
#define SPI(a) (*(int *)(a))
#define SPP(a) (*(unsigned char **)(a))
#define REC_A (*(unsigned char **)(D_00275B40 + 8))
#define REC_B (*(unsigned char **)(D_00275B40 + 0xC))
#define SETRAND(lv, m, add) { int x_ = func_00122BB8() >> 16; x_ *= (m); x_ >>= 15; lv = x_ + (add); }

extern char *D_00275B40;
extern char D_001C5680[];
extern int D_700031D8[4];
extern float D_70003190[4];
extern float D_700031A0[4];
extern float D_700031B0[4];
extern float D_70003600[4];
extern float D_70003610[4];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_70003910[4];
extern int func_001B0FD0(unsigned char *self);
extern int func_001BA1C0(unsigned char *self, int id);
extern int func_00122BB8(void);
extern float func_0011E2A8(float a);
extern float func_0011DF78(float a);
extern float func_0011DBB8(float a);
extern float func_0011E748(float a);
extern float func_0011E520(float a);
extern float func_001B1470(float a);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *mtx);
extern unsigned char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_001B17A0(unsigned char *self);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f);
extern int func_overlay_AREA07_00825470(unsigned char *self, void *mtx);
extern void func_overlay_AREA07_00825940(void *mtx);
extern void func_001EFD90(int id, void *a, void *b);
extern int func_0019B6C0(void *a, void *b, void *c);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA07_00823F40(unsigned char *self) {
    unsigned char *comp;
    unsigned char *hit;
    unsigned char *t;
    int kind;
    int r;
    float k;
    float at;
    float side;
    char *blk;
    unsigned char **pa;
    unsigned char **pc;

    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self)) {
            break;
        }
        if (func_001BA1C0(self, 0x15)) {
            self[4] = 0x64;
        } else {
            self[4] = 4;
        }
        self[0] = 1;
        SETRAND(S16(0x28), 300, 300)
        W->f200 = 1;
        W->f204 = 0;
        W->f208 = 0;
        W->f210 = 0.10471976f;
        W->f218 = 0.10471976f;
        W->f20C = 0;
        W->f214 = 0;
        W->f1F4 = 0.01620663f;
        W->f1F8 = 0.044879895f;
        W->f1FC = 0.0f;
        *(float *)(REC_B + 0x78) = -1.134464f + -0.8290314f * func_0011E2A8(W->f1FC);
        func_001C6380(self);
        func_001A2370(self, REC_B + 0x90);
        W->f220 = 0;
        comp = func_001AFA90(0xC);
        if (comp) {
            comp[0x9A] = 0;
            comp[3] = 0;
            *(short *)(comp + 0x2E) = 0;
            comp[0xD] = 0x7A;
            *(unsigned short *)(comp + 0xE) = 0xFFFF;
            *(short *)(comp + 0x54) = 0;
            *(short *)(comp + 0x56) = 0;
            *(float *)(comp + 0xA0) = 0.0f;
            *(float *)(comp + 0xA4) = 0.0f;
            *(float *)(comp + 0xA8) = 0.0f;
            *(float *)(comp + 0xAC) = 0.25f;
            func_00102948(comp + 0xB0, self + 0xB0);
            func_00102948(comp + 0xC0, self + 0xC0);
            *(char **)(comp + 0x10) = D_001C5680;
            W->f220 = comp;
            W->f224 = 0;
        }
        break;
    case 0x64:
        func_001C6380(self);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 4:
        if (W->f208 > 0) {
            W->f208--;
            if (W->f208 > 0x1E) {
                if (--W->f20C > 0) {
                    *(float *)(REC_A + 0x74) += W->f210;
                    if (*(float *)(REC_A + 0x74) > 1.134464f) {
                        *(float *)(REC_A + 0x74) = 1.134464f;
                        W->f210 = -W->f210;
                    }
                    if (*(float *)(REC_A + 0x74) < -1.134464f) {
                        *(float *)(REC_A + 0x74) = -1.134464f;
                        W->f210 = -W->f210;
                    }
                } else {
                    func_001FBD50(self, 0x428, 0, 300.0f);
                    SETRAND(W->f20C, 30, 0)
                    if (W->f20C & 4) {
                        W->f210 = -W->f210;
                    }
                }
                if (--W->f214 > 0) {
                    W->f1FC += W->f218;
                    if (W->f1FC > 3.1415927f) {
                        W->f1FC = -3.1415927f;
                    }
                    *(float *)(REC_B + 0x78) = -0.8290314f + 0.30543262f * func_0011E2A8(W->f1FC);
                } else {
                    SETRAND(W->f214, 30, 0)
                    if (W->f214 & 8) {
                        W->f218 = -W->f218;
                    }
                }
                *(float *)(W->f220 + 0xA0) = 0.0f;
                *(float *)(W->f220 + 0xA4) = 0.0f;
                *(float *)(W->f220 + 0xA8) = 0.0f;
                *(float *)(W->f220 + 0xAC) = 0.0f;
            } else {
                SPF(0x70003A20) = (float)(0x80 - W->f208 * 4) / 128.0f;
                *(float *)(W->f220 + 0xA0) = 0.0f;
                *(float *)(W->f220 + 0xA4) = SPF(0x70003A20);
                *(float *)(W->f220 + 0xA8) = 0.0f;
                *(float *)(W->f220 + 0xAC) = 0.25f;
                func_00102958(*(unsigned char **)(W->f220 + 0x11C) + 0x90, *(unsigned char **)(self + 0x11C) + 0x90);
            }
            func_001C6380(self);
            func_001A2370(self, REC_B + 0x90);
            func_001B17A0(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
            S16(0x36) = 0;
            break;
        }
        pc = &W->f220;
        *(float *)(W->f220 + 0xA0) = 0.0f;
        *(float *)(W->f220 + 0xA4) = 1.0f;
        *(float *)(W->f220 + 0xA8) = 0.0f;
        *(float *)(W->f220 + 0xAC) = 0.25f;
        func_00102958(*(unsigned char **)(W->f220 + 0x11C) + 0x90, *(unsigned char **)(self + 0x11C) + 0x90);
        S16(0x28)--;
        if (S16(0x28) < 0) {
            if (W->f200) {
                SETRAND(S16(0x28), 180, 0x3C)
            } else {
                SETRAND(S16(0x28), 300, 300)
            }
            W->f200 = !W->f200;
        }
        if (W->f200) {
            if ((S16(0x28) & 0x2F) == 2) {
                func_001FBD50(self, 0x423, 0, 60.0f);
            }
            *(float *)(REC_A + 0x74) += W->f1F4;
            if (W->f1F4 > 0.0f) {
                if (*(float *)(REC_A + 0x74) > 1.134464f) {
                    *(float *)(REC_A + 0x74) = 1.134464f;
                    W->f1F4 = -W->f1F4;
                }
            } else if (*(float *)(REC_A + 0x74) < -1.134464f) {
                *(float *)(REC_A + 0x74) = -1.134464f;
                W->f1F4 = -W->f1F4;
            }
            W->f1FC += W->f1F8;
            if (W->f1FC > 3.1415927f) {
                W->f1FC = -3.1415927f;
            }
            *(float *)(REC_B + 0x78) = -0.8290314f + 0.30543262f * func_0011E2A8(W->f1FC);
        }
        func_001C6380(self);
        func_001A2370(self, REC_B + 0x90);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        func_overlay_AREA07_00825470(self, REC_B + 0x90);
        if (S16(0x36) && W->f208 == 0) {
            W->f208 = 0x1E0;
            SETRAND(W->f20C, 30, 0)
            if (W->f20C & 8) {
                W->f210 = -W->f210;
            }
            SETRAND(W->f214, 30, 0)
            if (W->f214 & 4) {
                W->f218 = -W->f218;
            }
        }
        S16(0x36) = 0;
        if (W->f204) {
            self[4] = 1;
            W->f224 = 1;
            S16(0x28) = -0x2B;
            S16(0x2A) = 0x12C;
            W->f200 = 0;
            if ((SPI(0x70003B68) & 0x3F) == 0) {
                func_001FBD50(self, 0x424, 0, 300.0f);
            }
        }
        if (func_001BA1C0(self, 0x15)) {
            self[4] = 0x64;
            if (*pc) {
                *(float *)(*pc + 0xA0) = 0.0f;
                *(float *)(*pc + 0xA4) = 0.0f;
                *(float *)(*pc + 0xA8) = 0.0f;
                *(float *)(*pc + 0xAC) = 0.0f;
            }
        }
        break;
    case 1:
        if (W->f208 > 0) {
            W->f208--;
            if (W->f208 > 0x1E) {
                if (--W->f20C > 0) {
                    *(float *)(REC_A + 0x74) += W->f210;
                    if (*(float *)(REC_A + 0x74) > 1.134464f) {
                        *(float *)(REC_A + 0x74) = 1.134464f;
                        W->f210 = -W->f210;
                    }
                    if (*(float *)(REC_A + 0x74) < -1.134464f) {
                        *(float *)(REC_A + 0x74) = -1.134464f;
                        W->f210 = -W->f210;
                    }
                } else {
                    func_001FBD50(self, 0x428, 0, 300.0f);
                    SETRAND(W->f20C, 30, 0)
                    if (W->f20C & 4) {
                        W->f210 = -W->f210;
                    }
                }
                if (--W->f214 > 0) {
                    W->f1FC += W->f218;
                    if (W->f1FC > 3.1415927f) {
                        W->f1FC = -3.1415927f;
                    }
                    *(float *)(REC_B + 0x78) = -0.8290314f + 0.30543262f * func_0011E2A8(W->f1FC);
                } else {
                    SETRAND(W->f214, 30, 0)
                    if (W->f214 & 8) {
                        W->f218 = -W->f218;
                    }
                }
                *(float *)(W->f220 + 0xA0) = 0.0f;
                *(float *)(W->f220 + 0xA4) = 0.0f;
                *(float *)(W->f220 + 0xA8) = 0.0f;
                *(float *)(W->f220 + 0xAC) = 0.0f;
            } else {
                SPF(0x70003A20) = (float)(0x80 - W->f208 * 4) / 128.0f;
                *(float *)(W->f220 + 0xA0) = SPF(0x70003A20);
                *(float *)(W->f220 + 0xA4) = 0.0f;
                *(float *)(W->f220 + 0xA8) = 0.0f;
                *(float *)(W->f220 + 0xAC) = 0.25f;
                func_00102958(*(unsigned char **)(W->f220 + 0x11C) + 0x90, *(unsigned char **)(self + 0x11C) + 0x90);
            }
            func_001C6380(self);
            func_001A2370(self, REC_B + 0x90);
            func_001B17A0(self);
            (*(void (**)(unsigned char *))(self + 0x4C))(self);
            S16(0x36) = 0;
            break;
        }
        if ((SPI(0x70003B68) & 0x3F) == 0) {
            func_001FBD50(self, 0x424, 0, 300.0f);
        }
        *(float *)(W->f220 + 0xA0) = 1.0f;
        *(float *)(W->f220 + 0xA4) = 0.0f;
        *(float *)(W->f220 + 0xA8) = 0.0f;
        *(float *)(W->f220 + 0xAC) = 0.25f;
        func_00102958(*(unsigned char **)(W->f220 + 0x11C) + 0x90, *(unsigned char **)(self + 0x11C) + 0x90);
        func_001028D0(D_70003600, W->f204 + 0xB0, REC_A + 0xC0);
        func_00102948(D_70003610, D_70003600);
        SPF(0x7000360C) = 0.0f;
        SPF(0x70003604) = 0.0f;
        func_00102760(D_70003600, D_70003600);
        pa = (unsigned char **)(D_00275B40 + 8);
        side = SPF(0x70003600) * -*(float *)(*pa + 0xB0) + SPF(0x70003608) * -*(float *)(*pa + 0xB8);
        SPF(0x70003680) = side;
        if (side < -0.011635528f) {
            *(float *)(*pa + 0x74) -= 0.011635528f;
        } else if (side > 0.011635528f) {
            *(float *)(*pa + 0x74) += 0.011635528f;
        } else if (func_0011DF78(D_70003610[0]) > 0.001f) {
            if (D_70003610[0] < 0.0f) {
                SPF(0x70003684) = D_70003610[2] / D_70003610[0];
                SPF(0x70003684) = 3.1415927f - func_0011DBB8(SPF(0x70003684));
                *(float *)(REC_A + 0x74) = func_001B1470(SPF(0x70003684) - *(float *)(self + 0xC4));
            } else {
                SPF(0x70003684) = D_70003610[2] / D_70003610[0];
                SPF(0x70003684) = -func_0011DBB8(SPF(0x70003684));
                *(float *)(REC_A + 0x74) = func_001B1470(SPF(0x70003684) - *(float *)(self + 0xC4));
            }
        }
        if (*(float *)(REC_A + 0x74) < -1.134464f) {
            S16(0x2A) -= 4;
            *(float *)(REC_A + 0x74) = -1.134464f;
        }
        if (*(float *)(REC_A + 0x74) > 1.134464f) {
            S16(0x2A) -= 4;
            *(float *)(REC_A + 0x74) = 1.134464f;
        }
        func_00102948(D_70003600, D_70003610);
        SPF(0x70003680) = func_0011E748(SPF(0x70003600) * SPF(0x70003600) + SPF(0x70003608) * SPF(0x70003608));
        at = func_0011DBB8(SPF(0x70003604) / SPF(0x70003680));
        blk = D_00275B40;
        k = 0.011635528f;
        SPF(0x70003680) = at;
        if (SPF(0x70003680) > *(float *)(*(unsigned char **)(blk + 0xC) + 0x78) - k) {
            *(float *)(*(unsigned char **)(blk + 0xC) + 0x78) += k;
        } else if (SPF(0x70003680) < k + *(float *)(*(unsigned char **)(blk + 0xC) + 0x78)) {
            *(float *)(*(unsigned char **)(blk + 0xC) + 0x78) -= k;
        } else {
            *(float *)(*(unsigned char **)(blk + 0xC) + 0x78) = SPF(0x70003680);
        }
        if (*(float *)(REC_B + 0x78) < -1.134464f) {
            S16(0x2A) -= 4;
            *(float *)(REC_B + 0x78) = -1.134464f;
        }
        if (*(float *)(REC_B + 0x78) > -0.5235988f) {
            S16(0x2A) -= 4;
            *(float *)(REC_B + 0x78) = -0.5235988f;
        }
        func_001C6380(self);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        S16(0x28)++;
        r = func_overlay_AREA07_00825470(self, REC_B + 0x90);
        if (r == 2) {
            S16(0x2A) = 0x12C;
        } else {
            S16(0x2A)--;
        }
        if (W->f200) {
            W->f200++;
            if (W->f200 > 0xD) {
                if (r) {
                    func_001FBD50(self, 0x425, 0, 300.0f);
                    func_overlay_AREA07_00825940(REC_B + 0x90);
                    func_00102948(D_70003600, D_700031B0);
                    hit = SPP(0x700031D0);
                    kind = D_700031D8[0];
                    D_70003610[0] = *(float *)(hit + 0x24);
                    D_70003610[1] = *(float *)(hit + 0x28);
                    D_70003610[2] = *(float *)(hit + 0x2C);
                    D_70003610[3] = 1.0f;
                    D_70003600[3] = 1.0f;
                    if (kind == 1) {
                        t = SPP(0x700031D4);
                        if (!(t[2] & 0x1F)) {
                            if (!(t[0] & 2)) {
                                func_001EFD90(0x80000006, D_70003600, D_70003610);
                                *(float *)(SPP(0x700031D4) + 0x224) = 5.0f;
                                *SPP(0x700031D4) |= 2;
                                func_001028D0(D_70003910, D_700031A0, D_70003190);
                                SPF(0x7000391C) = 0.0f;
                                func_00102760(D_70003910, D_70003910);
                                func_00102948(SPP(0x700031D4) + 0x70, D_70003910);
                            }
                        } else {
                            func_001EFD90(0x80000007, D_70003600, D_70003610);
                            *(short *)(SPP(0x700031D4) + 0x36) = 5;
                        }
                    } else {
                        func_001EFD90(0x80000003, D_70003600, D_70003610);
                    }
                }
                W->f200 = 1;
            }
        } else if (S16(0x28) > 0) {
            W->f200 = 1;
            S16(0x28) = 0;
        }
        if (S16(0x36) && W->f208 == 0) {
            W->f208 = 0x1E0;
            SETRAND(W->f20C, 30, 0)
            if (W->f20C & 8) {
                W->f210 = -W->f210;
            }
            SETRAND(W->f214, 30, 0)
            if (W->f214 & 4) {
                W->f218 = -W->f218;
            }
        }
        S16(0x36) = 0;
        if (S16(0x2A) < 0) {
            self[4] = 4;
            W->f224 = 0;
            W->f204 = 0;
            W->f1FC = func_001B1470(func_0011E520(-(*(float *)(REC_B + 0x78) - -1.134464f) / -0.8290314f));
            SETRAND(S16(0x28), 300, 300)
            W->f200 = 1;
        }
        pc = &W->f220;
        if (func_001BA1C0(self, 0x15)) {
            self[4] = 0x64;
            if (*pc) {
                *(float *)(*pc + 0xA0) = 0.0f;
                *(float *)(*pc + 0xA4) = 0.0f;
                *(float *)(*pc + 0xA8) = 0.0f;
                *(float *)(*pc + 0xAC) = 0.0f;
            }
        }
        break;
    case 2:
        if (!func_001BA1C0(self, 0x15)) {
            if (W->f1FC != -1.5707964f) {
                if (W->f1FC < -1.5707964f) {
                    W->f1FC += W->f1F8;
                    if (W->f1FC > -1.5707964f) {
                        W->f1FC = -1.5707964f;
                    }
                } else {
                    W->f1FC -= W->f1F8;
                    if (W->f1FC < -1.5707964f) {
                        W->f1FC = -1.5707964f;
                    }
                }
                *(float *)(REC_B + 0x78) = -0.8290314f + 0.30543262f * func_0011E2A8(W->f1FC);
                func_00102958(*(unsigned char **)(W->f220 + 0x11C) + 0x90, *(unsigned char **)(self + 0x11C) + 0x90);
            } else if (W->f21C > 0) {
                W->f21C--;
                SPF(0x70003A20) = (float)W->f21C / 90.0f;
                if (W->f224) {
                    *(float *)(W->f220 + 0xA0) = SPF(0x70003A20);
                    *(float *)(W->f220 + 0xA4) = 0.0f;
                    *(float *)(W->f220 + 0xA8) = 0.0f;
                    *(float *)(W->f220 + 0xAC) = 0.25f;
                } else {
                    *(float *)(W->f220 + 0xA0) = 0.0f;
                    *(float *)(W->f220 + 0xA4) = SPF(0x70003A20);
                    *(float *)(W->f220 + 0xA8) = 0.0f;
                    *(float *)(W->f220 + 0xAC) = 0.25f;
                }
            }
        }
        func_001C6380(self);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
