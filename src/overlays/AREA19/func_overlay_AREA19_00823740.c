// NEARMISS func_overlay_AREA19_00823740 (99.39%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00823780 (splat/link name 00823740; overlay code
//  is linked 0x40 below where it runs), 0x520 bytes.
// Role (read from the instructions): sub 1 placement [40]: two flame columns.
//  State 0: two random phases / seeds, position (870, 370, 910), +2 = 0xD, +3
//  = 3, +0x30 = 0x82AFE0, +0x34 = 0x823770 (an empty function); state 3 and
//  func_001B6660(0x82AB00) when D_0081077B and D_00810778 are both 0xFF; +5 =
//  1 when D_008107F8 != 0; falls into state 1. State 1: returns after
//  func_001FC520(blk + 0x18) while D_00810702 is 2, 4, 5 or 8; +5 0 waits for
//  D_008107F8 (then func_001F02C0(.., 0x8E8, 300)); +5 1 draws four packets
//  0x82AF50 per column at (870, 448, 938) and (870, 448, 880) and keeps
//  func_001FC3C0(self, blk + 0x18, 0x8E9, 300, 4096). States 2/3
//  func_001FC520 and func_001AFC10.
// Divergence: registers and instruction order match; two alignment-like nops
//  sit in different places (the original has one after the outer-loop phase
//  store, mwcc 2.3.3 one after the inner-loop wrap), which shifts two branch
//  offsets. Loop forms, the wrap compare spelling and declaration orders were
//  tried.
extern unsigned char D_00810702;
extern unsigned char D_008107F8;
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_overlay_AREA19_0082AF50[];
extern char D_overlay_AREA19_0082AB00[];
extern char D_overlay_AREA19_0082AFE0[];
extern char D_overlay_AREA19_00823770[];
extern int func_00122BB8(void);
extern void *func_001B6660(void *p);
extern void func_001FC520(void *h);
extern void func_001F02C0(void *pos, int id, float vol);
extern void func_001029C0(void *m);
extern void func_00102918(void *a, void *b, void *c);
extern void func_00103230(void *dst, void *src, float k);
extern void func_001CFA60(void *pkt, void *m, float f12, float f13);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_001FC3C0(unsigned char *self, void *h, int id, float vol, float f13);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00823740(unsigned char *self) {
    int pkt[24];
    int j;
    int seed;
    int i;
    unsigned char *blk = self + 0x1F0;
    unsigned char *p;
    unsigned char *q;
    int k;
    float f;
    float r;
    switch (self[4]) {
    case 0:
        for (i = 0, p = blk; i < 2; i++) {
            *(float *)(p + 8) = (float)func_00122BB8() / 2147483648.0f;
            *(int *)(p + 0x10) = func_00122BB8();
            p += 4;
        }
        *(float *)(self + 0xB0) = 870.0f;
        *(float *)(self + 0xB4) = 370.0f;
        *(float *)(self + 0xB8) = 910.0f;
        *(float *)(self + 0xBC) = 1.0f;
        self[2] = 0xD;
        self[3] = 3;
        self[0xD] = 0;
        *(short *)(self + 0x56) = 1;
        self[0] = 2;
        *(void **)(self + 0x30) = D_overlay_AREA19_0082AFE0;
        *(void **)(self + 0x34) = D_overlay_AREA19_00823770;
        *(int *)(blk + 0x18) = -1;
        *(float *)(blk + 0) = 0.0f;
        *(float *)(blk + 4) = 0.0f;
        self[4] = 1;
        self[5] = 0;
        if (*(unsigned char *)0x81077B == 0xFF && *(unsigned char *)0x810778 == 0xFF) {
            func_001B6660(D_overlay_AREA19_0082AB00);
            self[4] = 3;
            return;
        }
        if (D_008107F8 != 0) {
            self[5] = 1;
        }
    case 1:
        switch (D_00810702) {
        case 2:
        case 4:
        case 5:
        case 8:
            func_001FC520(blk + 0x18);
            return;
        }
        switch (self[5]) {
        case 0:
            if (D_008107F8 != 0) {
                self[5] = 1;
                func_001F02C0(self + 0xB0, 0x8E8, 300.0f);
            }
            self[0] = 2;
            break;
        case 1:
            for (k = 0, q = blk; k < 2; k++) {
                switch (k & 1) {
                case 0:
                    *(float *)0x700038A0 = 870.0f;
                    *(float *)0x700038A4 = 448.0f;
                    *(float *)0x700038A8 = 938.0f;
                    *(float *)0x700038AC = 1.0f;
                    break;
                case 1:
                    *(float *)0x700038A0 = 870.0f;
                    *(float *)0x700038A4 = 448.0f;
                    *(float *)0x700038A8 = 880.0f;
                    *(float *)0x700038AC = 1.0f;
                    break;
                }
                seed = *(int *)(q + 0x10);
                func_001029C0(D_700036A0);
                func_00102918(D_700036A0, D_700036A0, D_700038A0);
                D_overlay_AREA19_0082AF50[0] = 30.0f;
                D_overlay_AREA19_0082AF50[1] = 0.0f;
                D_overlay_AREA19_0082AF50[2] = 30.0f;
                D_overlay_AREA19_0082AF50[3] = 1.0f;
                func_00103230(D_overlay_AREA19_0082AF50, D_overlay_AREA19_0082AF50, *(float *)(blk + 4));
                for (j = 0; j < 4; j++) {
                    f = *(float *)(q + 8) + (float)k / 4.0f;
                    *(float *)0x70003A20 = f;
                    if (!(f <= 2.0f)) {
                        f -= 1.0f;
                    }
                    r = (float)((seed >> 16) & 0xFFFF);
                    r = r / 65535.0f;
                    r += 0.0001f;
                    seed = seed * 37 + 11;
                    *(float *)0x70003A20 = f;
                    func_001CFA60(pkt, D_700036A0, f, r);
                    func_001CFBE0(0, 6, D_overlay_AREA19_0082AF50, pkt, 1);
                }
                *(float *)(q + 8) += *(float *)(blk + 0);
                if (!(*(float *)(q + 8) <= 2.0f)) {
                    *(float *)(q + 8) -= 1.0f;
                }
                q += 4;
            }
            *(float *)(blk + 0) = 0.008f;
            *(float *)(blk + 4) = 1.0f;
            if (!(*(float *)(blk + 0) < 0.008f)) {
                *(float *)(blk + 0) = 0.008f;
            }
            if (!(*(float *)(blk + 4) <= 1.0f)) {
                *(float *)(blk + 4) = 1.0f;
            }
            func_001FC3C0(self, blk + 0x18, 0x8E9, 300.0f, 4096.0f);
            self[0] = 1;
            func_001B17A0(self);
            break;
        }
        break;
    case 2:
    case 3:
        func_001FC520(blk + 0x18);
        func_001AFC10(self);
        break;
    }
}
