// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00828700 (splat/link name 008286C0; overlay code
//  is linked 0x40 below where it runs), 0x77C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [61], keyed to the object D_008102B0: +5 = 1 when
//  its +0x230 == 0x28 and +0x1F0 == 0x39; while its +0x1F1 == 1, 0x829310 on
//  D_00810E64 / D_00810E65 moves +0xC4 (clamped to +-0.75049156) and the
//  +0x118 model's +0x70 (-0.17453294 .. 0.6108653), sound 0x455 at the
//  clamps; a set D_00810E70 & (D_70003B74 | D_70003B78) bit starts sound
//  0x454, a 0x828E80 child and func_001B1E20(9, 0), then a +7 / +0x28 /
//  +0x2A sequence on the model's +0x7C..+0x84; D_008102B0 +0xC4 = +0xC4 + pi.
//  Three func_001F5940(7) markers on the +0x110 model; from D_0081080E ==
//  0x10 it ends after 255 frames.
typedef void (*ActorFn)(unsigned char *);
#define F(o) (*(float *)(self + (o)))
#define W(o) (*(float *)(w + (o)))
#define WI(o) (*(int *)(w + (o)))
#define S16(o) (*(short *)(self + (o)))
#define MDL (*(unsigned char **)(self + 0x118))
#define MDL2 (*(unsigned char **)(self + 0x110))
#define SPF(a) (*(float *)(a))
extern unsigned char D_008102B0[];
extern unsigned char D_00810E64[];
extern unsigned char D_00810E65[];
extern unsigned short D_00810E70[];
extern unsigned char D_0081080E[];
extern unsigned short D_70003B74[];
extern unsigned short D_70003B78[];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_70003400[16];
extern float D_70003600[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern float func_001B1470(float a);
extern void func_overlay_AREA21_00828410(unsigned char *self, unsigned char *w);
extern void func_overlay_AREA21_00829310(float *v, int key);
extern void func_overlay_AREA21_00828E80(unsigned char *self, unsigned char *w, int which);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001B1E20(int a, int b);
extern float func_0011DE90(float a);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001B17A0(unsigned char *self);
extern void func_001F5940(int a0, void *v, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_008286C0(unsigned char *self) {
    unsigned char *pl = D_008102B0;
    unsigned char *w = self + 0x1F0;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[0] = 1;
        WI(0x58) = 0;
        WI(0x5C) = 0;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (*(int *)(pl + 0x230) == 0x28 && pl[0x1F0] == 0x39) {
                self[5]++;
                self[6] = 0;
                WI(0x40) = 0;
            }
            break;
        case 1:
            if (*(int *)(pl + 0x230) != 0x28) {
                self[5] = 0;
                WI(0x40) = 0;
                WI(0x3C) = 0;
                W(0x30) = 0.0f;
                W(0x34) = 0.0f;
                W(0x38) = 0.0f;
                F(0xC4) = 0.0f;
                *(float *)(MDL + 0x70) = 0.0f;
                *(float *)(MDL + 0x7C) = 0.0f;
                *(float *)(MDL + 0x80) = 0.0f;
                *(float *)(MDL + 0x84) = 0.0f;
                func_001C6380(self);
                *(float *)(pl + 0xC4) = func_001B1470(3.1415927f + F(0xC4));
                break;
            }
            switch (self[6]) {
            case 0:
                if (pl[0x1F1] == 1) {
                    self[6]++;
                    self[7] = 0;
                    W(0x30) = 0.0f;
                    W(0x34) = 0.0f;
                    WI(0x3C) = 0;
                    func_overlay_AREA21_00828410(self, w);
                    WI(0x40) = 0;
                }
                break;
            case 1:
                switch (self[7]) {
                case 0:
                    if (D_00810E70[0] & (D_70003B74[0] | D_70003B78[0])) {
                        self[7] = 1;
                        S16(0x2A) = 20;
                        S16(0x28) = 10;
                        W(0x38) = 1.5707964f;
                        W(0x30) = 0.0f;
                        W(0x34) = 0.0f;
                        func_001FB9F0(0x454, 0x1000, 0x1000, 0x1000);
                        func_overlay_AREA21_00828E80(self, w, 0);
                        func_001B1E20(9, 0);
                    } else {
                        if (WI(0x40) != 0) {
                            WI(0x40)--;
                        }
                        func_overlay_AREA21_00829310((float *)(w + 0x34), D_00810E64[0]);
                        F(0xC4) -= W(0x34);
                        F(0xC4) = func_001B1470(F(0xC4));
                        if (F(0xC4) > 0.75049156f) {
                            F(0xC4) = 0.75049156f;
                            W(0x34) = 0.0f;
                        } else if (F(0xC4) < -0.75049156f) {
                            F(0xC4) = -0.75049156f;
                            W(0x34) = 0.0f;
                        } else if (W(0x34) && WI(0x40) == 0) {
                            func_001FB9F0(0x455, 0x1000, 0x1000, 0x1000);
                            WI(0x40) = 0x32;
                        }
                        func_overlay_AREA21_00829310((float *)(w + 0x30), D_00810E65[0]);
                        *(float *)(MDL + 0x70) += W(0x30);
                        *(float *)(MDL + 0x70) = func_001B1470(*(float *)(MDL + 0x70));
                        if (*(float *)(MDL + 0x70) > 0.6108653f) {
                            *(float *)(MDL + 0x70) = 0.6108653f;
                            W(0x30) = 0.0f;
                        } else if (*(float *)(MDL + 0x70) < -0.17453294f) {
                            *(float *)(MDL + 0x70) = -0.17453294f;
                            W(0x30) = 0.0f;
                        } else if (W(0x30) && WI(0x40) == 0) {
                            func_001FB9F0(0x455, 0x1000, 0x1000, 0x1000);
                            WI(0x40) = 0x32;
                        }
                    }
                    func_overlay_AREA21_00828410(self, w);
                    *(float *)(pl + 0xC4) = func_001B1470(3.1415927f + F(0xC4));
                    break;
                case 1:
                case 3:
                    if (--S16(0x2A) == 0) {
                        self[7]++;
                    } else if (S16(0x28) == 0) {
                        *(float *)(MDL + 0x7C) = 0.0f;
                        *(float *)(MDL + 0x80) = 0.0f;
                        *(float *)(MDL + 0x84) = 0.0f;
                        W(0x38) = 1.5707964f;
                    } else {
                        S16(0x28)--;
                        W(0x38) -= 0.31415927f;
                        SPF(0x70003600) = 0.0f;
                        SPF(0x70003604) = 0.0f;
                        SPF(0x70003608) = 2.0f * func_0011DE90(W(0x38));
                        SPF(0x7000360C) = 1.0f;
                        func_001029C0(D_70003400);
                        func_00102B08(D_70003400, D_70003400, *(float *)(MDL + 0x70));
                        func_001026A0(D_70003600, D_70003400, D_70003600);
                        *(float *)(MDL + 0x7C) = SPF(0x70003600);
                        *(float *)(MDL + 0x80) = SPF(0x70003604);
                        *(float *)(MDL + 0x84) = SPF(0x70003608);
                    }
                    break;
                case 2:
                    self[7] = 3;
                    S16(0x2A) = 20;
                    S16(0x28) = 10;
                    W(0x38) = 1.5707964f;
                    func_001FB9F0(0x454, 0x1000, 0x1000, 0x1000);
                    func_overlay_AREA21_00828E80(self, w, 1);
                    func_001B1E20(9, 0);
                    break;
                case 4:
                    self[7] = 0;
                    break;
                }
                func_001C6380(self);
                break;
            }
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 10.53f;
        D_700038A0[2] = 12.03f;
        D_700038A0[3] = 1.0f;
        func_001026A0(D_700038A0, MDL2 + 0x90, D_700038A0);
        func_001F5940(7, D_700038A0, 0);
        D_700038B0[0] = 8.5f;
        D_700038B0[1] = 11.0f;
        D_700038B0[2] = -3.0f;
        D_700038B0[3] = 1.0f;
        func_001026A0(D_700038B0, MDL2 + 0x90, D_700038B0);
        func_001F5940(7, D_700038B0, 0);
        D_700038C0[0] = -8.5f;
        D_700038C0[1] = 11.0f;
        D_700038C0[2] = -3.0f;
        D_700038C0[3] = 1.0f;
        func_001026A0(D_700038C0, MDL2 + 0x90, D_700038C0);
        func_001F5940(7, D_700038C0, 0);
        if (D_0081080E[0] == 0x10) {
            WI(0x58) = 1;
        }
        if (WI(0x58) == 1) {
            WI(0x5C)++;
            if (WI(0x5C) >= 0xFF) {
                self[4] = 3;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
