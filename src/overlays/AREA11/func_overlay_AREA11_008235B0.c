// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x008235F0 (splat/link name 008235B0; overlay code is
// linked 0x40 below where it runs), 0x1D0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: flame. State 0 builds the +0xD0 matrix (func_001029C0, then
// func_00102C58 with +0xC0 and func_00102918 with +0xB0), stores the +0x14 record's
// +0x1F0 at +0x30 and the callback 0x823580 at +0x34, sets +0 = 1, the scale
// (7, 15, 7) at +0x1F0, +0x210 = 0, the sound handle +0x20C = -1, the phase
// +0x204 = 0 and the rate +0x208 = rand / 2^31, then falls into state 1.
// State 1: func_001D04B0(+0xD0, 1, table 0x828340, phase, rate); phase +=
// 0.025 and drops by 1 once it reaches 2; loop sound 0x413 through
// func_001FC3C0(self, +0x20C, 0x413, 100, 4096); +0 = 2 while +0x210 counts
// down (then 1); func_001B17A0. States 2/3: func_001FC520(+0x20C) and
// func_001AFC10.
extern char overlay_AREA11_func_00823540[];
extern char D_overlay_AREA11_00828340[];

typedef struct {
    float scale[3];
    int pad[2];
    float phase;
    float rate;
    int snd;
    int count;
} Flame;

extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102C58(void *d, void *s, void *v);
extern void func_00102918(void *d, void *s, void *v);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);
extern void func_001FC3C0(unsigned char *self, int *h, int id, float f12, float f13);
extern void func_001FC520(int *h);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA11_008235B0(unsigned char *self) {
    Flame *blk = (Flame *)(self + 0x1F0);
    unsigned char *p = *(unsigned char **)(self + 0x14) + 0x1F0;
    int n;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(unsigned char **)(self + 0x30) = p;
        *(char **)(self + 0x34) = overlay_AREA11_func_00823540 + 0x40;
        self[0] = 1;
        blk->scale[0] = 7.0f;
        blk->scale[1] = 15.0f;
        blk->scale[2] = 7.0f;
        blk->count = 0;
        blk->snd = -1;
        blk->phase = 0.0f;
        blk->rate = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
    case 1:
        func_001D04B0(self + 0xD0, 1, D_overlay_AREA11_00828340, blk->phase, blk->rate);
        blk->phase += 0.025f;
        if (blk->phase >= 2.0f) {
            blk->phase -= 1.0f;
        }
        func_001FC3C0(self, &blk->snd, 0x413, 100.0f, 4096.0f);
        n = blk->count;
        if (n == 0) {
            self[0] = 1;
        } else {
            blk->count = n - 1;
            self[0] = 2;
        }
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001FC520(&blk->snd);
        func_001AFC10(self);
        break;
    }
}
