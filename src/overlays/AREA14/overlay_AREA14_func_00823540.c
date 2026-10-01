// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA14 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x1B4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA14; lane A03C).
// Role: sub 0 placements [35], [36]. State 0: func_001029C0(+0xD0),
//  func_00102C58(+0xD0, +0xD0, +0xC0), func_00102918(+0xD0, +0xD0, +0xB0),
//  +0x1F4 = 0.8, +0x1F0 = func_00122BB8() / 2147483648.0, state 1. State 1:
//  func_0021B9A0(2, 0, 0) and (3, 0, 700), a func_001CFB50 packet (+0xD0,
//  +0x1F4, +0x1F0, 1.0, 0.6, 15.0) queued by func_001CFBE0(func_001CCF70(+0x100),
//  5, 0x8266E0, ...); +0x1F4 += 0.001, minus 1.0 once it reaches 2.0;
//  func_0021B9A0(1, 0, 0). States 2 / 3: func_001AFC10. The 700 is an
//  int-staged float (idiom-24 family: it puts f13 before f12).
extern char D_overlay_AREA14_008266E0[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern int func_00122BB8(void);
extern void func_0021B9A0(int id, float a, float b);
extern int func_001CCF70(void *p);
extern void func_001CFB50(void *p, int a1, void *a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void overlay_AREA14_func_00823540(unsigned char *self) {
    float *w = (float *)(self + 0x1F0);
    int pkt[24];
    int h;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        w[1] = 0.8f;
        w[0] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        func_0021B9A0(2, 0.0f, 0.0f);
        { int k = 700; float y = (float)k; func_0021B9A0(3, 0.0f, y); }
        h = func_001CCF70(self + 0x100);
        func_001CFB50(pkt, 0, self + 0xD0, w[1], w[0], 1.0f, 0.6f, 15.0f);
        func_001CFBE0(h, 5, D_overlay_AREA14_008266E0, pkt, 0);
        w[1] += 0.001f;
        if (!(w[1] < 2.0f)) {
            w[1] -= 1.0f;
        }
        func_0021B9A0(1, 0.0f, 0.0f);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
