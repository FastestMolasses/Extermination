// NEARMISS func_overlay_AREA19_00823CD0 (93.11%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00823D10 (splat/link name 00823CD0; overlay code
//  is linked 0x40 below where it runs), 0x97C bytes.
// Role (read from the instructions): sub 1 placement [39]: the fire at (848,
//  377, 920). State 0: position and matrix, +0x30 = the parent's (+0x14)
//  +0x1F0 block, +0x34 = 0x823CA0, random seed, size (+0x214) 1; state 3 at
//  once when D_008107FB == 0xFF or D_008107F8 != 0. States 1/2 (none while
//  D_00810702 is 2, 4 or 5): state 2 once D_008107F8 != 0; three flame
//  packets 0x82B120 at z 940 / 920 / 900 and, while the size is non-zero and
//  D_008107F9 bit 7 is clear, three smoke columns (0x82B000 x3 and 0x82B090,
//  scaled by the size), between func_0021B9A0 calls; in state 2, after 60
//  frames, the size shrinks by 1/240 a frame and +0x218 grows by 0.01, ending
//  in state 3 past 5; func_001FC3C0(self, blk + 0x20, 0x413, 300, 4096) in
//  state 1; the block's +0 / +4 / +8 = (18, 45 * size, 35). State 3
//  func_001FC520 and func_001AFC10.
// Divergence: list scheduling in the flame and smoke loops: the float
//  constants of the func_001CFB50 calls and of the size products are
//  materialised in different registers and order, one scratch store uses a
//  different base form, and the original pads the end of the flame loop with
//  one nop. Calls, stores and control flow are the same. The switch-form
//  early return, an unsigned-char staged model index, extern-array scratch
//  updates, the order of the +0x204 if / else arms (92.35 -> 93.11) and
//  bounded declaration permutations were tried.
extern unsigned char D_00810702;
extern float D_700036A0[];
extern float D_700036E0[];
extern float D_700038A0[];
extern float D_overlay_AREA19_0082B000[];
extern float D_overlay_AREA19_0082B040[];
extern float D_overlay_AREA19_0082B050[];
extern float D_overlay_AREA19_0082B090[];
extern float D_overlay_AREA19_0082B0D0[];
extern float D_overlay_AREA19_0082B0E0[];
extern char D_overlay_AREA19_0082B120[];
extern char func_overlay_AREA19_00823C60[];
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102918(void *a, void *b, void *c);
extern int func_001CCF70(void *p);
extern void func_001CFB50(void *pkt, int a1, void *m, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);
extern void func_0021B9A0(int a, float f12, float f13);
extern void func_001FC3C0(unsigned char *self, void *h, int id, float vol, float f13);
extern void func_001FC520(void *h);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA19_00823CD0(unsigned char *self) {
    int pkt[24];
    int i;
    int h;
    int seed;
    int k;
    unsigned char *blk = self + 0x1F0;
    int j;
    unsigned char *ob = *(unsigned char **)(self + 0x14) + 0x1F0;
    int t;
    float r;
    float f;
    switch (self[4]) {
    case 0:
        *(float *)(self + 0xB0) = 848.0f;
        *(float *)(self + 0xB4) = 377.0f;
        *(float *)(self + 0xB8) = 920.0f;
        *(float *)(self + 0xBC) = 1.0f;
        *(unsigned char **)(self + 0x30) = ob;
        *(void **)(self + 0x34) = func_overlay_AREA19_00823C60 + 0x40;
        self[0] = 1;
        *(int *)(blk + 0x14) = 0;
        *(int *)(blk + 0x18) = 0;
        *(int *)(blk + 0x1C) = func_00122BB8();
        *(float *)(blk + 0x24) = 1.0f;
        *(float *)(blk + 0x28) = 0.0f;
        *(float *)(blk + 0x2C) = 0.0f;
        *(float *)(blk + 0x30) = 0.0f;
        *(int *)(blk + 0x20) = -1;
        func_001029C0(self + 0xD0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        self[0xC] = 0;
        self[9] = 0;
        self[4] = 1;
        if (*(unsigned char *)0x8107FB == 0xFF) {
            self[4] = 3;
            break;
        }
        if (*(unsigned char *)0x8107F8 != 0) {
            self[4] = 3;
            break;
        }
    case 1:
    case 2:
        switch (D_00810702) {
        case 2:
        case 4:
        case 5:
            return;
        }
        if (*(unsigned char *)0x8107F8 != 0) {
            self[4] = 2;
        }
        seed = *(int *)(blk + 0x1C);
        h = func_001CCF70(self + 0xB0);
        k = (unsigned char)(h > 1000000 ? 6 : 1);
        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0:
                *(float *)0x700038A0 = 848.0f;
                *(float *)0x700038A4 = 370.0f;
                *(float *)0x700038A8 = 940.0f;
                *(float *)0x700038AC = 1.0f;
                break;
            case 1:
                *(float *)0x700038A0 = 848.0f;
                *(float *)0x700038A4 = 370.0f;
                *(float *)0x700038A8 = 920.0f;
                *(float *)0x700038AC = 1.0f;
                break;
            case 2:
                *(float *)0x700038A0 = 848.0f;
                *(float *)0x700038A4 = 370.0f;
                *(float *)0x700038A8 = 900.0f;
                *(float *)0x700038AC = 1.0f;
                break;
            }
            func_001029C0(D_700036A0);
            func_001029C0(D_700036E0);
            func_00102918(D_700036A0, D_700036A0, D_700038A0);
            func_00102918(D_700036E0, D_700036E0, D_700038A0);
            D_700036A0[13] -= 4.0f;
            D_700036E0[13] -= 6.0f;
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            func_001CFB50(pkt, 0, D_700036E0, *(float *)(blk + 0x28), r, 1.0f, 0.2f, 20.0f);
            func_001CFBE0(h, 1, D_overlay_AREA19_0082B120, pkt, 0);
        }
        func_0021B9A0(2, 1.0f, 150.0f);
        func_0021B9A0(3, 1.0f, 150.0f);
        if (*(float *)(blk + 0x24) != 0.0f && !(*(unsigned char *)0x8107F9 & 0x80)) {
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    *(float *)0x700038A0 = 848.0f;
                    *(float *)0x700038A4 = 377.0f;
                    *(float *)0x700038A8 = 940.0f;
                    *(float *)0x700038AC = 1.0f;
                    break;
                case 1:
                    *(float *)0x700038A0 = 848.0f;
                    *(float *)0x700038A4 = 377.0f;
                    *(float *)0x700038A8 = 920.0f;
                    *(float *)0x700038AC = 1.0f;
                    break;
                case 2:
                    *(float *)0x700038A0 = 848.0f;
                    *(float *)0x700038A4 = 377.0f;
                    *(float *)0x700038A8 = 900.0f;
                    *(float *)0x700038AC = 1.0f;
                    break;
                }
                func_001029C0(D_700036A0);
                func_001029C0(D_700036E0);
                func_00102918(D_700036A0, D_700036A0, D_700038A0);
                func_00102918(D_700036E0, D_700036E0, D_700038A0);
                D_700036A0[13] += 2.0f;
                D_700036E0[13] -= 6.0f;
                for (j = 1; j < 4; j++) {
                    f = 4.0f * (float)j;
                    D_overlay_AREA19_0082B000[4] = f;
                    D_overlay_AREA19_0082B000[6] = f;
                    r = (float)((seed >> 16) & 0xFFFF);
                    r = r / 65535.0f;
                    r += 0.0001f;
                    seed = seed * 37 + 11;
                    D_overlay_AREA19_0082B000[5] = 10.0f * *(float *)(blk + 0x24);
                    D_overlay_AREA19_0082B000[1] = 15.0f * *(float *)(blk + 0x24);
                    D_overlay_AREA19_0082B040[0] = 8.0f * *(float *)(blk + 0x24);
                    D_overlay_AREA19_0082B040[1] = 8.0f * *(float *)(blk + 0x24);
                    D_overlay_AREA19_0082B050[0] = 8.0f * *(float *)(blk + 0x24);
                    D_overlay_AREA19_0082B050[1] = 8.0f * *(float *)(blk + 0x24);
                    func_001CFB50(pkt, 0, D_700036A0, *(float *)(blk + 0x2C), r, 1.0f, 0.2f, 20.0f);
                    func_001CFBE0(h, k, D_overlay_AREA19_0082B000, pkt, 1);
                }
                *(int *)&D_overlay_AREA19_0082B090[5] = 0;
                D_overlay_AREA19_0082B090[1] = 40.0f * *(float *)(blk + 0x24);
                r = (float)((seed >> 16) & 0xFFFF);
                r = r / 65535.0f;
                r += 0.0001f;
                seed = seed * 37 + 11;
                D_overlay_AREA19_0082B0D0[0] = 20.0f * *(float *)(blk + 0x24);
                D_overlay_AREA19_0082B0D0[1] = 20.0f * *(float *)(blk + 0x24);
                D_overlay_AREA19_0082B0E0[0] = 20.0f * *(float *)(blk + 0x24);
                D_overlay_AREA19_0082B0E0[1] = 30.0f * *(float *)(blk + 0x24);
                func_001CFB50(pkt, 0, D_700036E0, *(float *)(blk + 0x30), r, 1.0f, 0.2f, 20.0f);
                func_001CFBE0(h, k, D_overlay_AREA19_0082B090, pkt, 1);
            }
        }
        func_0021B9A0(1, 0.0f, 0.0f);
        if (*(int *)(blk + 0x14) == 0) {
            self[0] = 1;
        } else {
            *(int *)(blk + 0x14) -= 1;
            self[0] = 2;
        }
        if (self[4] == 2) {
            self[0] = 2;
        }
        *(float *)(blk + 0x2C) += 0.015f;
        *(float *)(blk + 0x30) += 0.0075f;
        if (!(*(float *)(blk + 0x2C) <= 2.0f)) {
            *(float *)(blk + 0x2C) -= 1.0f;
        }
        if (!(*(float *)(blk + 0x30) <= 2.0f)) {
            *(float *)(blk + 0x30) -= 1.0f;
        }
        if (self[4] == 2) {
            *(int *)(blk + 0x18) += 1;
            if (*(int *)(blk + 0x18) > 0x3C) {
                *(float *)(blk + 0x24) -= 0.004166667f;
                if (*(float *)(blk + 0x24) < 0.0f) {
                    *(float *)(blk + 0x24) = 0.0f;
                }
                *(float *)(blk + 0x28) += 0.01f;
                if (!(*(float *)(blk + 0x28) <= 5.0f)) {
                    self[4] = 3;
                    break;
                }
            }
        }
        if (self[4] == 1) {
            func_001FC3C0(self, blk + 0x20, 0x413, 300.0f, 4096.0f);
        }
        *(float *)(blk + 0) = 18.0f;
        *(float *)(blk + 4) = 45.0f * *(float *)(blk + 0x24);
        *(float *)(blk + 8) = 35.0f;
        func_001B1B70(self);
        break;
    case 3:
        func_001FC520(blk + 0x20);
        func_001AFC10(self);
        break;
    }
}
