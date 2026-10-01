// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00828F30 (splat/link name 00828EF0; overlay code
//  is linked 0x40 below where it runs), 0x3D8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: the object spawned by 0x828E80 (behaviour 0x828F30). State 0: speed
//  +0xC0 scaled to 8 (func_00103230), +0x20 = func_001EFE00(0x80000037,
//  self). State 1 moves by +0xC0 for 32 frames, then falls (+0x38 += 0.05 up
//  to 7); func_0019A570 hits stop it (state 2, +5 0): a hit object of kind
//  2 with +0 == 1 gets func_001B41F0(..., 0x2000, 0x50), another gets +0x36
//  = 5 and the direction, with sound 0x456; below y 10 it stops with +5 1.
//  State 2: effect 0x80000069 / sound 0x456 (+5 0) or 0x8000005F / 0xDB
//  (+5 1), the +0x20 object's state 2.
#define F(o) (*(float *)(self + (o)))
#define S16(o) (*(short *)(self + (o)))
#define SPF(a) (*(float *)(a))
extern float D_700038A0[4];
extern float D_700031B0[4];
extern void func_00103230(void *dst, void *src, float a);
extern unsigned char *func_001EFE00(int id, unsigned char *self);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001031E0(void *dst, void *src);
extern void func_00102948(void *dst, void *src);
extern void func_001B41F0(unsigned char *o, void *v, void *dir, int a3, int t0, int t1);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001C94B0(void *m, void *pos, void *a, void *b);
extern void func_001EFD20(int id, void *pos);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00828EF0(unsigned char *self) {
    int r;
    unsigned char *o;
    switch (self[4]) {
    case 0:
        self[4]++;
        S16(0x28) = 32;
        func_00103230(self + 0xC0, self + 0xC0, 8.0f);
        *(unsigned char **)(self + 0x20) = func_001EFE00(0x80000037, self);
        F(0xA0) = F(0xB0);
        F(0xA4) = F(0xB4);
        F(0xA8) = F(0xB8);
        F(0xAC) = F(0xBC);
        F(0x38) = 0.0f;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (--S16(0x28) == 0) {
                self[5]++;
            }
            F(0xB0) += F(0xC0);
            F(0xB4) += F(0xC4);
            F(0xB8) += F(0xC8);
            break;
        case 1:
            F(0xB0) += F(0xC0) / 7.0f;
            F(0xB8) += F(0xC8) / 7.0f;
            F(0x38) += 0.05f;
            if (!(F(0x38) <= 7.0f)) {
                F(0x38) = 7.0f;
            }
            F(0xB4) -= F(0x38);
            break;
        }
        if ((r = func_0019A570(self + 0xA0, self + 0xB0, 7, 0x20)) != 0) {
            func_001031E0(D_700038A0, D_700031B0);
            SPF(0x700038AC) = 1.0f;
            func_00102948(self + 0xB0, D_700038A0);
            if ((unsigned int)(r - 1) < 2) {
                o = *(unsigned char **)0x700031D4;
                if (o != 0 && o[0] != 0) {
                    if ((o[2] & ~0xE0) == 2) {
                        if (o[0] == 1) {
                            func_001B41F0(o, D_700038A0, self + 0xC0,
                                          *(int *)(*(unsigned char **)0x700031D0 + 0x1C), 0x2000, 0x50);
                        }
                    } else {
                        *(short *)(o + 0x36) = 5;
                        func_00102948(o + 0x70, self + 0xC0);
                    }
                    func_001FBD50(self, 0x456, 0, 300.0f);
                }
            }
            self[4] = 2;
            self[5] = 0;
        } else if (F(0xB4) < 10.0f) {
            F(0xB4) = 10.0f;
            self[4] = 2;
            self[5] = 1;
        }
        F(0xA0) = F(0xB0);
        F(0xA4) = F(0xB4);
        F(0xA8) = F(0xB8);
        F(0xAC) = F(0xBC);
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 0.0f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 1.0f;
        func_001C94B0(self + 0xD0, self + 0xB0, D_700038A0, D_700038A0);
        break;
    case 2:
        switch (self[5]) {
        case 0:
            func_001EFD20(0x80000069, self + 0xB0);
            func_001FBD50(self, 0x456, 0, 1000.0f);
            break;
        case 1:
            func_001EFD20(0x8000005F, self + 0xB0);
            func_001FBD50(self, 0xDB, 0, 800.0f);
            break;
        }
        (*(unsigned char **)(self + 0x20))[4] = 2;
        self[4]++;
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
