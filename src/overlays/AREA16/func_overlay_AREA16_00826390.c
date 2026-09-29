// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA16 overlay, runtime 0x008263D0 (splat/link name 00826390; overlay code is
// linked 0x40 below where it runs), 0x674 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// Role: state 0 (after func_001B0FD0): func_001C6380, +0 = +8 = 1, +0x30 =
//  0x82AD00, +0x2EC = 0, three rand fractions; with D_0081080B >= 4 state 2
//  and +0x2D0 = 0, else +0x2D0 = func_001EFD20(4, +0xB0) and the D_00275B40[2]
//  object +0x80 = -1.5 (bit 0 set) or 0. State 1: bit 2 of D_0081080B retires
//  the effect; +5 0 waits for bit 2 of +0xB and starts one of three scripts
//  (0x82A6C0 / 0x82AA80 / 0x82A840 by bits 0/1); 1 plays sound 0x19B at frame
//  120 and between frames 121 and 139 of +0x2DC slides the +0x80 value by
//  0.075 (at 139 sets it to -1.5 or 0 and toggles bit 0 of D_0081080B). Bit 0
//  fades +0x2EC in (effect mode 1/2), else out (mode 3); while +0x2EC != 0
//  draws a packet (rotated matrix, (318, 176.3, 486), table 0x82AD20); +0x2E8
//  += 0.02; animate; with +0 == 1 func_001F5940(7, position + (0.25, -1.2,
//  1)). State 2 animates and retires the effect. State 3 func_001AFC10.
extern unsigned char **D_00275B40;
extern unsigned char D_0081080B[];
extern char D_700036A0[];
extern float D_700036D0[];
extern float D_700038A0[];
extern char D_overlay_AREA16_0082AD00[];
extern char D_overlay_AREA16_0082A6C0[];
extern char D_overlay_AREA16_0082AA80[];
extern char D_overlay_AREA16_0082A840[];
extern char D_overlay_AREA16_0082AD20[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern int func_00122BB8(void);
extern unsigned char *func_001EFD20(int id, void *a);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float f);
extern void func_00102BB0(void *dst, void *src, float f);
extern int func_001CCF70(void *p);
extern void func_001CFAE0(void *dst, int a1, void *src, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern int func_001B17A0(unsigned char *self);
extern void func_001F5940(int a0, void *v, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00826390(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    unsigned char *o;
    int *p;
    int pkt[24];
    int h;
    int d;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[0] = 1;
        self[8] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA16_0082AD00;
        *(float *)(self + 0x2EC) = 0.0f;
        *(float *)(self + 0x2E8) = (float)func_00122BB8() / 2147483648.0f;
        *(float *)(self + 0x2E4) = (float)func_00122BB8() / 2147483648.0f;
        *(float *)(self + 0x2E0) = (float)func_00122BB8() / 2147483648.0f;
        if (D_0081080B[0] >= 4) {
            self[4] = 2;
            self[8] = 0;
            self[0] = 0;
            *(int *)(self + 0x2D0) = 0;
        } else {
            *(unsigned char **)(self + 0x2D0) = func_001EFD20(4, self + 0xB0);
            if (D_0081080B[0] & 1) {
                *(int *)(self + 0x2D4) = 1;
                *(float *)(D_00275B40[2] + 0x80) = -1.5f;
            } else {
                *(int *)(self + 0x2D4) = 0;
                *(float *)(D_00275B40[2] + 0x80) = 0.0f;
            }
        }
        break;
    case 1:
        if (D_0081080B[0] & 4) {
            self[0] = 2;
            o = *(unsigned char **)(self + 0x2D0);
            p = (int *)(self + 0x1F0) + 0x38;
            if (o != 0) {
                o[5] = 0x63;
                *p = 0;
            }
        }
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                *(short *)(self + 0x28) = 0;
                d = D_0081080B[0];
                if (d & 1) {
                    func_001BA1A0(blk, D_overlay_AREA16_0082A6C0);
                    *(int *)(self + 0x2D4) = 0;
                } else {
                    if (d & 2) {
                        func_001BA1A0(blk, D_overlay_AREA16_0082AA80);
                    } else {
                        func_001BA1A0(blk, D_overlay_AREA16_0082A840);
                    }
                    *(int *)(self + 0x2D4) = 1;
                }
                *(int *)(self + 0x2DC) = 0;
                self[5] = 1;
            }
            break;
        case 1:
            if (*(short *)(self + 0x28) < 120) {
                (*(short *)(self + 0x28))++;
                if (*(short *)(self + 0x28) == 120) {
                    func_001FBD50(self, 0x19B, 0, 300.0f);
                }
            }
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                self[0xB] = 0;
            } else {
                (*(int *)(self + 0x2DC))++;
                if (*(int *)(self + 0x2DC) > 120 && *(int *)(self + 0x2DC) < 140) {
                    if (*(int *)(self + 0x2DC) == 139) {
                        if (*(int *)(self + 0x2D4) != 0) {
                            *(float *)(D_00275B40[2] + 0x80) = -1.5f;
                        } else {
                            *(float *)(D_00275B40[2] + 0x80) = 0.0f;
                        }
                        d = D_0081080B[0];
                        if (d & 1) {
                            D_0081080B[0] = d & 0xFE;
                        } else {
                            D_0081080B[0] = d | 1;
                        }
                    } else if (*(int *)(self + 0x2D4) != 0) {
                        *(float *)(D_00275B40[2] + 0x80) -= 0.075f;
                    } else {
                        *(float *)(D_00275B40[2] + 0x80) += 0.075f;
                    }
                }
            }
            break;
        }
        if (D_0081080B[0] & 1) {
            *(float *)(self + 0x2EC) += 0.02f;
            if (*(float *)(self + 0x2EC) > 1.0f) {
                *(float *)(self + 0x2EC) = 1.0f;
            }
            o = *(unsigned char **)(self + 0x2D0);
            if (o != 0) {
                if (D_0081080B[0] & 2) {
                    o[5] = 1;
                } else {
                    o[5] = 2;
                }
            }
        } else {
            *(float *)(self + 0x2EC) -= 0.02f;
            if (*(float *)(self + 0x2EC) < 0.0f) {
                *(float *)(self + 0x2EC) = 0.0f;
            }
            o = *(unsigned char **)(self + 0x2D0);
            if (o != 0) {
                o[5] = 3;
            }
        }
        if (*(float *)(self + 0x2EC) != 0.0f) {
            func_001029C0(D_700036A0);
            func_00102B08(D_700036A0, D_700036A0, -0.34906587f);
            func_00102BB0(D_700036A0, D_700036A0, -1.5707964f);
            D_700036D0[0] = 318.0f;
            D_700036D0[1] = 176.3f;
            D_700036D0[2] = 486.0f;
            h = func_001CCF70(D_700036D0);
            func_001CFAE0(pkt, 0, D_700036A0, *(float *)(self + 0x2E8), *(float *)(self + 0x2E4),
                          *(float *)(self + 0x2EC), 0.1f);
            func_001CFBE0(h, 1, D_overlay_AREA16_0082AD20, pkt, 1);
        }
        *(float *)(self + 0x2E8) += 0.02f;
        if (*(float *)(self + 0x2E8) > 2.0f) {
            *(float *)(self + 0x2E8) -= 1.0f;
        }
        func_001C6380(self);
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        if (self[0] == 1) {
            D_700038A0[3] = 1.0f;
            D_700038A0[0] = 0.25f + *(float *)(self + 0xB0);
            D_700038A0[1] = -1.2f + *(float *)(self + 0xB4);
            D_700038A0[2] = 1.0f + *(float *)(self + 0xB8);
            func_001F5940(7, D_700038A0, 0);
        }
        break;
    case 2:
        func_001B17A0(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        o = *(unsigned char **)(self + 0x2D0);
        p = (int *)(self + 0x1F0) + 0x38;
        if (o != 0) {
            o[5] = 0x63;
            *p = 0;
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
