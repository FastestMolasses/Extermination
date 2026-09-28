// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00824BB0 (splat/link name 00824B70; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 sets block +0x1F0 scale 0.2 and a random seed; state 1
//  submits 21 packets (func_001CFB50 / func_001CFBE0, table 0x828CD0) along an
//  arc (angle 180 - 180 i/21 into D_70003A20, positions from 0x828B80), the
//  seed stepping seed*37+11; scale += 0.012 until it passes 2.0, then state 3.
typedef struct {
    float scale;
    int zero;
    int seed;
} Fx;
extern char D_700036A0[];
extern char D_700036D0[];
extern float D_70003A20;
extern char D_overlay_AREA00_00828B80[];
extern char D_overlay_AREA00_00828CD0[];
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_00102918(void *a, void *b, void *c);
extern int func_001CCF70(void *p);
extern void func_001CFB50(int *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, int *, int);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00824B70(unsigned char *self) {
    int i;
    int h;
    int seed;
    Fx *fx = (Fx *)(self + 0x1F0);
    char *pos;
    int pkt[24];
    float r;
    switch (self[4]) {
    case 0:
        fx->scale = 0.2f;
        fx->zero = 0;
        fx->seed = func_00122BB8();
        self[4] = 1;
    case 1:
        seed = fx->seed;
        pos = D_overlay_AREA00_00828B80;
        for (i = 0; i < 21; i++) {
            D_70003A20 = 180.0f - 180.0f * ((float)i / 21.0f);
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, 3.1415927f * D_70003A20 / 180.0f);
            func_00102918(D_700036A0, D_700036A0, pos);
            h = func_001CCF70(D_700036D0);
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFB50(pkt, 0, D_700036A0, fx->scale, r, 1.0f, 1e-6f, 12.0f);
            func_001CFBE0(h, 1, D_overlay_AREA00_00828CD0, pkt, 0);
            pos += 0x10;
        }
        fx->scale += 0.012f;
        if (!(fx->scale <= 2.0f)) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
