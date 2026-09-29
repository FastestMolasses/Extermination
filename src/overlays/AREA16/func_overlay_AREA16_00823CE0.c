// NEARMISS func_overlay_AREA16_00823CE0 (99.34%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00823D20 (splat/link name 00823CE0; overlay code is
// linked 0x40 below where it runs), 0x244 bytes.
// Role (read from the instructions): state 0: fifteen +0x1F0 floats = 0.4 +
//  0.2 * rand fraction, +0x22C = rand seed, state 1, position = 0x8290E0,
//  sound 0x928 (falls through). State 1: for each of the fifteen, a seeded
//  random into 0x70003A20; values under 1.5 draw (matrix at 0x700036A0
//  translated to 0x8290A0[i], func_001D04B0(.., 1, 0x829010, value, random))
//  and advance by 0.015; state 3 when all fifteen are past 1.5. States 2/3
//  func_001AFC10.
// Divergence: saved-register assignment in the case-1 loop (seed, done, the
//  running +0x1F0 pointer and the table pointer land in s0/s1/s2/s3 in a
//  different order); all 5040 declaration orders and separate loop variables
//  score 99.34 at best.
extern float D_70003A20;
extern char D_700036A0[];
extern char D_overlay_AREA16_008290E0[];
extern float D_overlay_AREA16_008290A0[][4];
extern char D_overlay_AREA16_00829010[];
extern int func_00122BB8(void);
extern void func_00102948(void *dst, void *src);
extern void func_001FBD50(unsigned char *self, int id, int a2, float f12);
extern void func_001029C0(void *m);
extern void func_00102918(void *dst, void *src, void *pos);
extern void func_001D04B0(void *m, int a1, void *tbl, float f12, float f13);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA16_00823CE0(unsigned char *self) {
    float *blk = (float *)(self + 0x1F0);
    int seed;
    int done;
    int i;
    float r;
    float (*pts)[4];
    float *p;
    switch (self[4]) {
    case 0:
        p = blk;
        for (i = 0; i < 15; i++) {
            *p = 0.4f + 0.2f * ((float)func_00122BB8() / 2147483648.0f);
            p++;
        }
        ((int *)blk)[15] = func_00122BB8();
        self[4] = 1;
        func_00102948(self + 0xB0, D_overlay_AREA16_008290E0);
        func_001FBD50(self, 0x928, 0, 300.0f);
    case 1:
        seed = ((int *)blk)[15];
        pts = D_overlay_AREA16_008290A0;
        done = 0;
        p = blk;
        for (i = 0; i < 15; i++) {
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            seed = seed * 37 + 11;
            D_70003A20 = r;
            if (*p < 1.5f) {
                func_001029C0(D_700036A0);
                func_00102918(D_700036A0, D_700036A0, *pts);
                func_001D04B0(D_700036A0, 1, D_overlay_AREA16_00829010, *p, D_70003A20);
                *p += 0.015f;
            } else {
                done++;
            }
            p++;
            pts++;
        }
        if (done == 15) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
