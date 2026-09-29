// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code is
// linked 0x40 below where it runs), 0x58C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA06; lane A06C).
// Role (read from the instructions): state 0 sets the +0x1F0 countdown to
// 240, +0x1F4 to rand%2+2 and +0x1F8 to rand%5+15, then falls into state 1.
// Sub-state +5 = 0: every rand%50+50 frames it builds a random yaw matrix at
// the runtime point 0x826A70 and spawns class 0x80000042 there
// (func_001EFEB0), and calls func_001F02C0(0x826A70, 0x41D, 200.0f); after
// the +0x1F0 countdown it sets 90 frames, sub-state 1, func_0019C6F0(0xD, 1).
// Sub-state 1: the same class-0x80000042 spawn every rand%5+15 frames; every
// rand%2+2 frames a point x = -225-30r, y = 20, z = -595-170r in the 0x700038A0
// scratch vector, its offset from 0x826A70 (func_001028D0), its length
// (func_0011E748) at 0x70003A20, a class 0x8000003B spawn given +5 = 1,
// +0x1F0 = 12, +0x1F4 = that length, +0x1F8 = 0.4f, and on every 100th value
// of D_70003B68 a random func_001FB9F0 sound 0x41D/0x41E/0x41F; func_001EA210
// (0, 2.0f) when rand is even and the countdown is a multiple of 3; after the
// countdown 240 frames, sub-state 0, func_0019C6F0(0xD, 0). States 2/3
// func_001AFC10.
#define SPF(a) (*(float *)(a))
#define SPI(a) (*(int *)(a))
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];
extern int D_70003B68;
extern char D_overlay_AREA06_00826A70[];
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102918(void *dst, void *src, void *v);
extern char *func_001EFEB0(int id, void *m);
extern void func_001F02C0(void *pos, int id, float f12);
extern void func_0019C6F0(int a0, int a1);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_0011E748(float a);
extern void func_00102760(void *dst, void *src);
extern void func_001CD390(void *m, void *v);
extern void func_001FB9F0(int id, int a1, int a2, int a3);
extern void func_001EA210(int a0, float f12);
extern void func_001AFC10(unsigned char *self);

void overlay_AREA06_func_00823540(unsigned char *self) {
    int *w = (int *)(self + 0x1F0);
    char *p;
    switch (self[4]) {
    case 0:
        w[0] = 0xF0;
        w[1] = func_00122BB8() % 2 + 2;
        w[2] = func_00122BB8() % 5 + 15;
        self[4] = 1;
        self[5] = 0;
    case 1:
        switch (self[5]) {
        case 0:
            w[2]--;
            if (w[2] < 0) {
                w[2] = func_00122BB8() % 50 + 50;
                func_001029C0(D_700036A0);
                func_00102BB0(D_700036A0, D_700036A0, 3.1415927f * ((float)func_00122BB8() / 2147483648.0f));
                func_00102918(D_700036A0, D_700036A0, D_overlay_AREA06_00826A70);
                func_001EFEB0(0x80000042, D_700036A0);
                func_001F02C0(D_overlay_AREA06_00826A70, 0x41D, 200.0f);
            }
            w[0]--;
            if (w[0] < 0) {
                w[0] = 0x5A;
                self[5] = 1;
                func_0019C6F0(0xD, 1);
            }
            break;
        case 1:
            w[2]--;
            if (w[2] < 0) {
                w[2] = func_00122BB8() % 5 + 15;
                func_001029C0(D_700036A0);
                func_00102BB0(D_700036A0, D_700036A0, 3.1415927f * ((float)func_00122BB8() / 2147483648.0f));
                func_00102918(D_700036A0, D_700036A0, D_overlay_AREA06_00826A70);
                func_001EFEB0(0x80000042, D_700036A0);
            }
            w[1]--;
            if (w[1] < 0) {
                w[1] = func_00122BB8() % 2 + 2;
                SPF(0x700038A0) = -225.0f + -30.0f * ((float)func_00122BB8() / 2147483648.0f);
                SPF(0x700038A8) = -595.0f + -170.0f * ((float)func_00122BB8() / 2147483648.0f);
                SPF(0x700038A4) = 20.0f;
                SPF(0x700038AC) = 1.0f;
                func_001028D0(D_700038B0, D_overlay_AREA06_00826A70, D_700038A0);
                SPF(0x70003A20) = func_0011E748(SPF(0x700038B0) * SPF(0x700038B0) + SPF(0x700038B4) * SPF(0x700038B4) + SPF(0x700038B8) * SPF(0x700038B8));
                func_00102760(D_700038B0, D_700038B0);
                func_001CD390(D_700036A0, D_700038B0);
                func_00102918(D_700036A0, D_700036A0, D_700038A0);
                p = func_001EFEB0(0x8000003B, D_700036A0);
                if (p != 0) {
                    p[5] = 1;
                    *(int *)(p + 0x1F0) = 12;
                    *(float *)(p + 0x1F4) = SPF(0x70003A20);
                    *(float *)(p + 0x1F8) = 0.4f;
                }
                if (D_70003B68 % 100 == 0) {
                    switch (func_00122BB8() % 4) {
                    case 0:
                        func_001FB9F0(0x41D, 0x1000, 0x1000, 0x1000);
                        break;
                    case 1:
                        func_001FB9F0(0x41E, 0x1000, 0x1000, 0x1000);
                        break;
                    case 2:
                        func_001FB9F0(0x41F, 0x1000, 0x1000, 0x1000);
                        break;
                    case 3:
                        break;
                    }
                }
            }
            if (func_00122BB8() % 2 == 0 && w[0] % 3 == 0) {
                func_001EA210(0, 2.0f);
            }
            w[0]--;
            if (w[0] < 0) {
                w[0] = 0xF0;
                self[5] = 0;
                func_0019C6F0(0xD, 0);
            }
            break;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
