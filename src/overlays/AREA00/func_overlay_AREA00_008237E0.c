// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00823820 (splat/link name 008237E0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: eight puffs in block +0x1F0 (timers at +8, four floats each at +0x28):
//  state 0 randomises them; state 1 counts each timer down, draws two packets
//  per live puff (tables 0x8287E0 / 0x828870), a func_001CD520 sprite while its
//  first value is under 0.3, re-arms it (timer rand % 60 + 40) past 1.5, then
//  calls func_001FC3C0(self, block, 0x41C, 200, 4096).
// Matching: func_001CD520 is declared with the colour word before the three
//  floats (the order func_001F1F60 uses); the integer argument still goes in
//  t0 and the floats in f12-f14, so the call is the same, but the declared
//  order sets mwcc's argument scheduling.
typedef struct {
    float a;
    float b;
    float c;
    float d;
} Puff;
typedef struct {
    int mode;           /* 0x00 */
    int count;          /* 0x04 */
    int timer[8];       /* 0x08 */
    Puff p[8];          /* 0x28 */
} Fx;
extern char D_700036A0[];
extern char D_700036D0[];
extern int D_70003600;
extern int D_70003604;
extern int D_70003608;
extern char D_overlay_AREA00_008287E0[];
extern char D_overlay_AREA00_00828870[];
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102948(void *dst, void *src);
extern int func_001CCF70(void *p);
extern int func_001281C0(float f);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         unsigned int rgba, float w, float h, float zbias);
extern void func_001CFA60(void *sp, void *pos, float f12, float f13);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern void func_001FC3C0(void *a0, void *a1, int a2, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_008237E0(unsigned char *self) {
    int pkt[24];
    int i;
    int h;
    Fx *fx = (Fx *)(self + 0x1F0);
    float t;
    float w;
    switch (self[4]) {
    case 0:
        for (i = 0; i < 8; i++) {
            fx->timer[i] = func_00122BB8() % 120;
            fx->p[i].a = 0.0f;
            fx->p[i].b = 0.0f;
            fx->p[i].c = (float)func_00122BB8() / 2147483648.0f;
            fx->p[i].d = (float)func_00122BB8() / 2147483648.0f;
        }
        fx->mode = -1;
        fx->count = 0;
        self[0xC] = 0;
        self[9] = 0;
        self[4] = 1;
        self[5] = 0;
    case 1:
        func_001029C0(D_700036A0);
        func_00102948(D_700036D0, self + 0xB0);
        h = func_001CCF70(D_700036D0);
        for (i = 0; i < 8; i++) {
            fx->timer[i]--;
            if (fx->timer[i] >= 0) {
                continue;
            }
            if (fx->p[i].a < 0.3f) {
                t = (0.3f - fx->p[i].a) / 0.3f;
                D_70003600 = func_001281C0(255.0f * t);
                t = 192.0f * t;
                D_70003604 = func_001281C0(t) << 8;
                D_70003608 = func_001281C0(t) << 16;
                w = 1.0f + 3.0f * fx->p[i].a / 0.3f;
                func_001CD520(0, 2, D_700036D0, 0x20045B2599421E98ULL,
                              0x80000000 | D_70003608 | D_70003604 | D_70003600, w, w, 2.0f);
            }
            func_001CFA60(pkt, D_700036A0, fx->p[i].a, fx->p[i].c);
            func_001CFBE0(h, 2, D_overlay_AREA00_008287E0, pkt, 0);
            fx->p[i].a += 0.09f;
            if (!(fx->p[i].a <= 2.0f)) {
                fx->p[i].a = 2.0f;
            }
            func_001CFA60(pkt, D_700036A0, fx->p[i].b, fx->p[i].d);
            func_001CFBE0(h, 2, D_overlay_AREA00_00828870, pkt, 0);
            fx->p[i].b += 0.01f;
            if (!(fx->p[i].b <= 1.5f)) {
                fx->timer[i] = func_00122BB8() % 60 + 40;
                fx->p[i].a = 0.0f;
                fx->p[i].b = 0.0f;
                fx->p[i].c = (float)func_00122BB8() / 2147483648.0f;
                fx->p[i].d = (float)func_00122BB8() / 2147483648.0f;
            }
        }
        func_001FC3C0(self, fx, 0x41C, 200.0f, 4096.0f);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
