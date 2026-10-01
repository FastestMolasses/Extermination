// NEARMISS func_overlay_AREA21_00825C30 (99.23%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00825C70 (splat/link name 00825C30; overlay code
//  is linked 0x40 below where it runs), 0x42C bytes.
// Role: moving object (no static reference; its +0x34 is the code pointer
//  0x825C60). State 0: direction +0xC0 normalised into +0x70, matrix
//  func_001CD390, velocity +0x200 = 7 * dir, +0x30 = 0x82B170, 180-frame life
//  (+0x20). State 1 draws func_001CEEE0, then: a func_0019A570 hit gives
//  effect 0x80000003 (kind 1: func_001B41F0(..., 0, 10); kind 2: +0x36 = 5
//  and a push); below y 10, effect 0x80000026; else it moves by the
//  velocity. It ends (+0 = 2, state 3) on a hit or the floor.
// Divergence: in the second r == 1 test the original leaves the branch
// slot empty; mwcc 2.3.3 speculates the 0x700038B0 address half into it,
// which shifts the later branch offsets by one word. Literal-address and
// volatile spellings of the store were tried.
#define F(o) (*(float *)(self + (o)))
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700031B0[4];
extern float D_00810374[];
extern char D_overlay_AREA21_0082B170[];
extern char D_overlay_AREA21_00825C60[];
extern void func_00102948(void *dst, void *src);
extern void func_00102760(void *dst, void *src);
extern void func_001CD390(void *m, void *dir);
extern void func_00103230(void *dst, void *src, float a);
extern void func_001CEEE0(void *m, void *a, void *b, int n, float f12, float f13, float f14);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001B41F0(unsigned char *o, void *v, void *dir, int a3, int t0, int t1);
extern float func_001B1470(float a);
extern void func_001EFD90(int id, void *a, void *b);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA21_00825C30(unsigned char *self) {
    int r;
    float *w = (float *)(self + 0x1F0);
    unsigned char *o;
    switch (self[4]) {
    case 0:
        func_00102948(self + 0x70, self + 0xC0);
        func_00102760(self + 0xC0, self + 0xC0);
        func_001CD390(self + 0xD0, self + 0xC0);
        func_00103230(w + 4, self + 0xC0, 7.0f);
        func_00102948(w, self + 0xB0);
        *(char **)(self + 0x30) = D_overlay_AREA21_0082B170;
        *(char **)(self + 0x34) = D_overlay_AREA21_00825C60;
        self[0xC] = 0;
        self[9] = 0;
        self[0] = 1;
        self[4] = 1;
        F(0x20) = 180.0f;
    case 1:
        F(0x100) = F(0xB0);
        F(0x104) = F(0xB4);
        F(0x108) = F(0xB8);
        D_700038A0[0] = 0.0f;
        D_700038A0[1] = 0.0f;
        D_700038A0[2] = 0.0f;
        D_700038A0[3] = 1.0f;
        D_700038B0[0] = 96.0f;
        D_700038B0[1] = 96.0f;
        D_700038B0[2] = 32.0f;
        D_700038B0[3] = 1.0f;
        func_001CEEE0(self + 0xD0, D_700038A0, D_700038B0, 0x18, 5.0f, 0.3f, 0.3f);
        if ((r = func_0019A570(w, self + 0xB0, 7, 0x20)) != 0) {
            func_00102948(D_700038A0, D_700031B0);
            D_700038A0[3] = 1.0f;
            if (r == 1) {
                o = *(unsigned char **)0x700031D4;
                if (o != 0 && o[0] == 1 && (o[2] & ~0xE0) == 2) {
                    func_001B41F0(o, D_700038A0, self + 0x70,
                                  *(int *)(*(unsigned char **)0x700031D0 + 0x1C), 0, 10);
                }
            } else if (r == 2) {
                o = *(unsigned char **)0x700031D4;
                if (o != 0 && o[0] != 0) {
                    *(short *)(o + 0x36) = 5;
                    func_00102948(o + 0x70, self + 0x70);
                }
            }
            if (r == 1) {
                D_700038B0[0] = 0.0f;
                D_700038B0[1] = func_001B1470(3.1415927f + D_00810374[0]);
                D_700038B0[2] = 0.0f;
                D_700038B0[3] = 1.0f;
            } else {
                o = *(unsigned char **)0x700031D0;
                D_700038B0[0] = *(float *)(o + 0x24);
                D_700038B0[1] = *(float *)(o + 0x28);
                D_700038B0[2] = *(float *)(o + 0x2C);
                D_700038A0[3] = 0.0f;
                D_700038B0[3] = 0.0f;
            }
            func_001EFD90(0x80000003, D_700038A0, D_700038B0);
            self[0] = 2;
            self[4] = 3;
        } else if (F(0xB4) < 10.0f) {
            D_700038A0[0] = F(0xB0);
            D_700038A0[1] = 10.0f;
            D_700038A0[2] = F(0xB8);
            D_700038A0[3] = 1.0f;
            D_700038B0[0] = 0.0f;
            D_700038B0[1] = 1.0f;
            D_700038B0[2] = 0.0f;
            D_700038B0[3] = 1.0f;
            func_001EFD90(0x80000026, D_700038A0, D_700038B0);
            self[0] = 2;
            self[4] = 3;
        } else {
            F(0x20) -= 1.0f;
            if (!F(0x20)) {
                self[4] = 3;
            }
            func_00102948(w, self + 0xB0);
            F(0xB0) += w[4];
            F(0xB4) += w[5];
            F(0xB8) += w[6];
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
