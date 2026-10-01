// NEARMISS func_overlay_AREA19_00824BA0 (98.66%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00824BE0 (splat/link name 00824BA0; overlay code
//  is linked 0x40 below where it runs), 0x310 bytes.
// Role (read from the instructions): an effect (no static reach): +0xD 0
//  draws packets 0x82B480 / 0x82B510, +0xD 1 0x82B360 / 0x82B3F0, with random
//  phases; t += 0.03, state 3 past 1.5. The start value falls through: +0xD 0
//  stores 0.1 and then 0.3, so both models start at 0.3 (the original does
//  the same).
// Divergence: in two func_001CFA60 calls the original loads the phase
//  (+0x1F0) into f12 before the random-fraction conversion and sets a0 / a1
//  last; mwcc 2.3.3 sets a0 / a1 first and loads f12 in the call's slot
//  region. Argument temporaries and statement orders were tried.
extern char D_overlay_AREA19_0082B360[];
extern char D_overlay_AREA19_0082B3F0[];
extern char D_overlay_AREA19_0082B480[];
extern char D_overlay_AREA19_0082B510[];
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFA60(void *pkt, void *m, float f12, float f13);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00824BA0(unsigned char *self) {
    int pkt[24];
    float r;
    int h;
    int seed;
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        switch (self[0xD]) {
        case 0:
            *(float *)(blk + 0) = 0.1f;
        case 1:
            *(float *)(blk + 0) = 0.3f;
            break;
        }
        *(int *)(blk + 4) = func_00122BB8();
        self[4] = 1;
    case 1:
        seed = *(int *)(blk + 4);
        h = func_001CCF70(self + 0x100);
        switch (self[0xD]) {
        case 0:
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFA60(pkt, self + 0xD0, *(float *)(blk + 0), r);
            func_001CFBE0(h, 0, D_overlay_AREA19_0082B480, pkt, 0);
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFA60(pkt, self + 0xD0, *(float *)(blk + 0), r);
            func_001CFBE0(h, 1, D_overlay_AREA19_0082B510, pkt, 0);
            *(float *)(blk + 0) += 0.03f;
            if (!(*(float *)(blk + 0) <= 1.5f)) {
                self[4] = 3;
            }
            break;
        case 1:
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFA60(pkt, self + 0xD0, *(float *)(blk + 0), r);
            func_001CFBE0(h, 1, D_overlay_AREA19_0082B360, pkt, 0);
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFA60(pkt, self + 0xD0, *(float *)(blk + 0), r);
            func_001CFBE0(h, 1, D_overlay_AREA19_0082B3F0, pkt, 0);
            *(float *)(blk + 0) += 0.03f;
            if (!(*(float *)(blk + 0) <= 1.5f)) {
                self[4] = 3;
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
