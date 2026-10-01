// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008297C0 (splat/link name 00829780; overlay code
//  is linked 0x40 below where it runs), 0x74C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: sub 0 placement [62]; the 0x828700 pattern for D_008102B0 +0x1F0
//  == 0x38: the D_00810E64 / E65 inputs move +0xC4 and the model's +0x70
//  (+-0.69813174); a set input bit adds 1 to +0x22C (else it decays by 0.5)
//  and, when +7 is 0, calls func_001B1E20(0xA, 0), sound 0x451, effect 0xA,
//  a func_001F4F40(2) child and func_001F4010(3); above 900: +0 = 2,
//  D_008102B0 +0x302 = 1, +0x22C = 1800, +5 2 and 0x829F10; +5 2 counts
//  +0x22C down with 0x829F40 each frame; otherwise 0x8260A0(+0x22C / 900).
//  Each frame the +0x20 child takes +0xC4 and a func_001F5940(7) marker is
//  placed.
typedef void (*ActorFn)(unsigned char *);
#define F(o) (*(float *)(self + (o)))
#define W(o) (*(float *)(w + (o)))
#define MDL (*(unsigned char **)(self + 0x118))
#define SPF(a) (*(float *)(a))
extern unsigned char D_008102B0[];
extern unsigned char D_00810E64[];
extern unsigned char D_00810E65[];
extern unsigned short D_00810E70[];
extern unsigned short D_70003B74[];
extern unsigned short D_70003B78[];
extern float D_700038A0[4];
extern float D_700036A0[4];
extern float D_700036D0[4];
extern float D_70003400[16];
extern float D_70003600[4];
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern unsigned char *func_001C5570(unsigned char *self, void *v, int a2, int a3);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_overlay_AREA21_00829310(float *v, int key);
extern float func_001B1470(float a);
extern void func_overlay_AREA21_00829590(unsigned char *self, unsigned char *w);
extern void func_001B1E20(int a, int b);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001EFD90(int id, void *a, void *b);
extern unsigned char *func_001F4F40(int a);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_001026D0(void *dst, void *a, void *b);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001F4010(int a0, void *a1);
extern float func_0011DE90(float a);
extern void func_overlay_AREA21_008294C0(unsigned char *self, unsigned char *o);
extern void func_overlay_AREA21_00829F10(unsigned char *self);
extern void func_overlay_AREA21_008260A0(float t);
extern void func_overlay_AREA21_00829F40(unsigned char *self);
extern void func_001B17A0(unsigned char *self);
extern void func_001F5940(int a0, void *v, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00829780(unsigned char *self) {
    unsigned char *pl = D_008102B0;
    unsigned char *w = self + 0x1F0;
    unsigned char *o;
    unsigned char *c;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) != 0) {
            break;
        }
        func_001C6380(self);
        self[0] = 1;
        W(0x3C) = 0.0f;
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 1.0f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 1.0f;
        *(unsigned char **)(self + 0x20) = func_001C5570(self, D_700038A0, 0x15, 2);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (*(int *)(pl + 0x230) == 0x28 && pl[0x1F0] == 0x38) {
                self[5]++;
                self[6] = 0;
            }
            if (W(0x3C) != 0.0f) {
                W(0x3C) -= 0.5f;
            }
            break;
        case 1:
            if (*(int *)(pl + 0x230) != 0x28) {
                self[5] = 0;
                W(0x30) = 0.0f;
                W(0x34) = 0.0f;
                W(0x38) = 0.0f;
                *(float *)(MDL + 0x70) = 0.0f;
                *(float *)(MDL + 0x7C) = 0.0f;
                *(float *)(MDL + 0x80) = 0.0f;
                *(float *)(MDL + 0x84) = 0.0f;
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
                break;
            }
            switch (self[6]) {
            case 0:
                if (pl[0x1F1] == 1) {
                    self[6]++;
                    self[7] = 0;
                    W(0x30) = 0.0f;
                    W(0x34) = 0.0f;
                    func_overlay_AREA21_00829590(self, w);
                }
                break;
            case 1:
                func_overlay_AREA21_00829310((float *)(w + 0x34), D_00810E64[0]);
                F(0xC4) -= W(0x34);
                F(0xC4) = func_001B1470(F(0xC4));
                func_overlay_AREA21_00829310((float *)(w + 0x30), D_00810E65[0]);
                *(float *)(MDL + 0x70) += W(0x30);
                *(float *)(MDL + 0x70) = func_001B1470(*(float *)(MDL + 0x70));
                if (*(float *)(MDL + 0x70) > 0.69813174f) {
                    *(float *)(MDL + 0x70) = 0.69813174f;
                    W(0x30) = 0.0f;
                } else if (*(float *)(MDL + 0x70) < -0.69813174f) {
                    *(float *)(MDL + 0x70) = -0.69813174f;
                    W(0x30) = 0.0f;
                }
                func_overlay_AREA21_00829590(self, w);
                if (D_00810E70[0] & (D_70003B74[0] | D_70003B78[0])) {
                    W(0x3C) += 1.0f;
                    if (self[7] == 0) {
                        func_001B1E20(0xA, 0);
                        self[7] = 1;
                        *(short *)(self + 0x28) = 6;
                        W(0x38) = 1.5707964f;
                        func_001FB9F0(0x451, 0x1000, 0x1000, 0x1000);
                        func_001EFD90(0xA, w, self + 0x70);
                        o = func_001F4F40(2);
                        if (o != 0) {
                            func_00102948(o + 0xB0, w);
                            func_00102958(o + 0xD0, MDL + 0x90);
                            func_00102948(o + 0x100, w);
                            *(float *)(o + 0x10C) = 1.0f;
                            func_00102958(D_700036A0, MDL + 0x90);
                            func_001029C0(D_70003400);
                            func_00102BB0(D_70003400, D_70003400, 1.5707964f);
                            func_001026D0(D_700036A0, D_700036A0, D_70003400);
                            SPF(0x70003600) = 3.0f;
                            SPF(0x70003604) = 4.0f;
                            SPF(0x70003608) = 0.5f;
                            SPF(0x7000360C) = 1.0f;
                            func_001026A0(D_700036D0, D_700036A0, D_70003600);
                            func_001F4010(3, D_700036A0);
                        }
                    }
                } else if (W(0x3C) != 0.0f) {
                    W(0x3C) -= 0.5f;
                }
                if (self[7] != 0) {
                    if (--*(short *)(self + 0x28) == 0) {
                        self[7] = 0;
                        *(float *)(MDL + 0x7C) = 0.0f;
                        *(float *)(MDL + 0x80) = 0.0f;
                        *(float *)(MDL + 0x84) = 0.0f;
                        W(0x38) = 1.5707964f;
                    } else {
                        W(0x38) -= 0.5235988f;
                        SPF(0x70003600) = 0.0f;
                        SPF(0x70003604) = 0.0f;
                        SPF(0x70003608) = 0.15f * func_0011DE90(W(0x38));
                        SPF(0x7000360C) = 1.0f;
                        func_001029C0(D_70003400);
                        func_00102B08(D_70003400, D_70003400, *(float *)(MDL + 0x70));
                        func_001026A0(D_70003600, D_70003400, D_70003600);
                        *(float *)(MDL + 0x7C) = SPF(0x70003600);
                        *(float *)(MDL + 0x80) = SPF(0x70003604);
                        *(float *)(MDL + 0x84) = SPF(0x70003608);
                    }
                }
                func_001C6380(self);
                func_001A2370(self, self + 0xD0);
                func_overlay_AREA21_008294C0(self, pl);
                if (W(0x3C) > 900.0f) {
                    self[0] = 2;
                    pl[0x302] = 1;
                    W(0x3C) = 1800.0f;
                    self[5] = 2;
                    W(0x30) = 0.0f;
                    W(0x34) = 0.0f;
                    W(0x38) = 0.0f;
                    *(float *)(MDL + 0x70) = 0.0f;
                    *(float *)(MDL + 0x7C) = 0.0f;
                    *(float *)(MDL + 0x80) = 0.0f;
                    *(float *)(MDL + 0x84) = 0.0f;
                    func_overlay_AREA21_00829F10(self);
                } else {
                    func_overlay_AREA21_008260A0(W(0x3C) / 900.0f);
                }
                break;
            }
            break;
        case 2:
            W(0x3C) -= 1.0f;
            if (!W(0x3C)) {
                self[0] = 1;
                self[5] = 0;
            }
            func_overlay_AREA21_00829F40(self);
            break;
        }
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        c = *(unsigned char **)(self + 0x20);
        if (c != 0) {
            *(float *)(c + 0xC4) = F(0xC4);
        }
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 0.0f;
        D_700038A0[2] = 2.323f;
        D_700038A0[3] = 1.0f;
        func_001026A0(D_700038A0, MDL + 0x90, D_700038A0);
        func_001F5940(7, D_700038A0, 0);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
