// NEARMISS func_overlay_AREA07_00823550 (99.88%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA07 overlay, runtime 0x00823590 (splat/link name 00823550; overlay code
//  is linked 0x40 below where it runs), 0x3DC bytes.
// NEARMISS: overlay_match.py check AREA07 scores 99.88% (lane OVLC); the
//  overlay links this function from its splat .s.
// Role: effect object (no static reference in the tables area_overview.py
//  reads). State 0: the +0xD0 matrix from func_001029C0 / 00102B08 (+0xC0) /
//  00102BB0 (+0xC4) / 00102A60 (+0xC8) / 00102918 (+0xB0); +0x30 = the +0x14
//  owner's +0x1F0, +0x34 = 0x823580 (an empty function); +0x208 = the integer
//  part (func_001281C0) of 6 + 18 * rand fraction, +0x204 = a seed, +0x20C =
//  0; +0x1F0 = (2, 2, 15) through +0xD0 minus +0xB0 (func_001028D0). State 1,
//  +5 0: the packet table 0x826420 (+0x6C = (float)+0x208) is drawn twice a
//  frame by func_001CFA60 / func_001CFBE0 at seeded random phases; +0 = 1;
//  +0x20C grows by 0.05 and past +0x208 + 1 a new 6 + 9 * rand count starts
//  +5 1. +5 1: +0 = 2 until +0x20C passes +0x208, then 6 + 18 * rand and +5
//  0. func_001B17A0 each frame; states 2 / 3 func_001AFC10.
// Divergence: the three 2^31 constants (the rand fraction divisor) are built
//  in a2 by the original and in a0 by mwcc 2.3.3; instructions, order and
//  every other register are equal. Declaration orders (420), a (int) cast in
//  place of the func_001281C0 call, prototype spellings and placements of the
//  owner pointer were tried.
extern float D_700038A0[4];
extern float D_overlay_AREA07_0082648C;
extern char D_overlay_AREA07_00826420[];
extern char D_overlay_AREA07_00823580[];
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102BB0(void *dst, void *src, float a);
extern void func_00102A60(void *dst, void *src, float a);
extern void func_00102918(void *dst, void *src, void *v);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001028D0(void *dst, void *a, void *b);
extern int func_00122BB8(void);
extern int func_001281C0(float f);
extern int func_001CCF70(void *pos);
extern void func_001CFA60(void *pkt, void *m, float f12, float f13);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA07_00823550(unsigned char *self) {
    int pkt[24];
    unsigned char *parent = *(unsigned char **)(self + 0x14) + 0x1F0;
    int seed;
    int i;
    int h;
    unsigned char *blk = self + 0x1F0;

    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102B08(self + 0xD0, self + 0xD0, *(float *)(self + 0xC0));
        func_00102BB0(self + 0xD0, self + 0xD0, *(float *)(self + 0xC4));
        func_00102A60(self + 0xD0, self + 0xD0, *(float *)(self + 0xC8));
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        *(unsigned char **)(self + 0x30) = parent;
        *(char **)(self + 0x34) = D_overlay_AREA07_00823580;
        *(int *)(blk + 0x18) = func_001281C0(6.0f + 18.0f * ((float)func_00122BB8() / 2147483648.0f));
        *(int *)(blk + 0x14) = func_00122BB8();
        *(float *)(blk + 0x1C) = 0.0f;
        self[4] = 1;
        self[5] = 0;
        D_700038A0[0] = 2.0f;
        D_700038A0[1] = 2.0f;
        D_700038A0[2] = 15.0f;
        D_700038A0[3] = 1.0f;
        func_001026A0(D_700038A0, self + 0xD0, D_700038A0);
        func_001028D0(blk, D_700038A0, self + 0xB0);
        break;
    case 1:
        switch (self[5]) {
        case 0:
            h = func_001CCF70(self + 0x100);
            D_overlay_AREA07_0082648C = (float)*(int *)(blk + 0x18);
            seed = *(int *)(blk + 0x14);
            for (i = 0; i < 2; i++) {
                float f = (float)((seed >> 16) & 0xFFFF);
                float g;
                f = f / 65535.0f;
                f += 0.0001f;
                f = *(float *)(blk + 0x1C) + f;
                seed = seed * 37 + 11;
                g = (float)((seed >> 16) & 0xFFFF);
                seed = seed * 37 + 11;
                g = g / 65535.0f;
                g += 0.0001f;
                *(float *)0x70003A20 = f;
                func_001CFA60(pkt, self + 0xD0, *(float *)0x70003A20, g);
                func_001CFBE0(h, 1, D_overlay_AREA07_00826420, pkt, 1);
            }
            self[0] = 1;
            ((float *)blk)[7] += 0.05f;
            if (!(((float *)blk)[7] <= 1.0f + (float)*(int *)(blk + 0x18))) {
                *(int *)(blk + 0x18) = func_001281C0(6.0f + 9.0f * ((float)func_00122BB8() / 2147483648.0f));
                *(float *)(blk + 0x1C) = 0.0f;
                self[5] = 1;
            }
            break;
        case 1:
            self[0] = 2;
            *(float *)(blk + 0x1C) += 0.05f;
            if (!(*(float *)(blk + 0x1C) <= (float)*(int *)(blk + 0x18))) {
                *(int *)(blk + 0x18) = func_001281C0(6.0f + 18.0f * ((float)func_00122BB8() / 2147483648.0f));
                *(float *)(blk + 0x1C) = 0.0f;
                self[5] = 0;
            }
            break;
        }
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
