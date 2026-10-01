// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00828F40 (splat/link name 00828F00; overlay code
//  is linked 0x40 below where it runs), 0x358 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA13; lane A13C).
// Role: an effect (no static reach): +0x30 = the parent's (+0x14) +0x1F0
//  block, +0x34 = 0x828F30 (an empty function). State 1 draws eight packets
//  0x82D4E0 on a circle of radius 10 (func_001CFA60 / func_001CFBE0, random
//  phases), grows t (+0x20C) by a step easing from 0.1 toward 0.01, writes
//  the block's +0 / +4 / +8 = 0.666666 * (100, 50, 100) * t, sets +0 = 2
//  after 40 frames and ends (state 3, +0 = 2) at t >= 1.5.
extern float D_700036A0[];
extern float D_700036D0[];
extern float D_700038A0[4];
extern char D_overlay_AREA13_0082D4E0[];
extern char D_overlay_AREA13_00828F30[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern void func_00102BB0(void *dst, void *src, float angle);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001028B8(void *dst, void *a, void *b);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFA60(void *pkt, void *m, float f12, float f13);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00828F00(unsigned char *self) {
    int pkt[24];
    int i;
    int h;
    int seed;
    float r;
    float s;
    unsigned char *blk = self + 0x1F0;
    unsigned char *pb = *(unsigned char **)(self + 0x14) + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(int *)(blk + 0x18) = 0;
        *(float *)(blk + 0x1C) = 0.0f;
        *(float *)(blk + 0x20) = 0.1f;
        *(int *)(blk + 0x14) = func_00122BB8();
        self[4] = 1;
        self[0] = 1;
        *(unsigned char **)(self + 0x30) = pb;
        *(void **)(self + 0x34) = D_overlay_AREA13_00828F30;
    case 1:
        seed = *(int *)(blk + 0x14);
        i = 0;
        *(float *)0x70003A20 = -3.1415927f;
        do {
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, *(float *)0x70003A20);
            *(float *)0x700038A0 = 0.0f;
            *(float *)0x700038A4 = 0.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            func_001026A0(D_700038A0, D_700036A0, D_700038A0);
            func_001028B8(D_700038A0, D_700038A0, self + 0xB0);
            func_00102918(D_700036A0, D_700036A0, D_700038A0);
            h = func_001CCF70(D_700036D0);
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFA60(pkt, D_700036A0, *(float *)(blk + 0x1C), r);
            func_001CFBE0(h + 0x2000, 5, D_overlay_AREA13_0082D4E0, pkt, 0);
            i++;
            *(float *)0x70003A20 += 0.7853982f;
        } while (i < 8);
        s = *(float *)(blk + 0x20) + (0.01f - *(float *)(blk + 0x20)) / 8.0f;
        *(float *)(blk + 0x20) = s;
        if (s < 0.01f) {
            s = 0.01f;
        }
        *(float *)(blk + 0x20) = s;
        *(float *)(blk + 0x1C) += s;
        if (!(*(float *)(blk + 0x1C) < 1.5f)) {
            self[4] = 3;
            self[0] = 2;
            break;
        }
        *(float *)(blk + 0x0) = 0.666666f * (100.0f * *(float *)(blk + 0x1C));
        *(float *)(blk + 0x4) = 0.666666f * (50.0f * *(float *)(blk + 0x1C));
        *(float *)(blk + 0x8) = 0.666666f * (100.0f * *(float *)(blk + 0x1C));
        *(int *)(blk + 0x18) += 1;
        if (*(int *)(blk + 0x18) > 0x28) {
            self[0] = 2;
        }
        func_001B1B70(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
