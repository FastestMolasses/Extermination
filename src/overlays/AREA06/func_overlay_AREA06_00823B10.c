// NEARMISS func_overlay_AREA06_00823B10 (97.12%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA06 overlay, runtime 0x00823B50 (splat/link name 00823B10; overlay code is
// linked 0x40 below where it runs), 0x58C bytes.
// Role (read from the instructions): state 0 sets +0x30 = 0x826AB0 (runtime
// {5, 15, 7.5}), +0x34 = 0x823B10 (the runtime address of
// func_overlay_AREA06_00823AD0; the link name of that address is this
// function), +0x1F0 = 0, +0x1F8 = rand%3, +0x1F4 = A[+0x1F8], +0x200 = 0,
// +0x1FC = a random fraction. State 1 returns at once while D_00810702 == 1;
// otherwise a three-phase cycle on +5 with the local copies of the period
// tables A {150, 240, 120}, B {90, 90, 60}, C {90, 120, 180} (runtime data at
// 0x826AC8/AD8/AE8): phase 0 (+0 = 2) spawns class 2, phase 1 (+0 = 2) class
// 3, phase 2 class 4 plus a class 0x8000003B object (+5 = 1, +0x1F0 = 6,
// +0x1F4 = 14.0, +0x1F8 = 0.5), whenever the +0x1F4 phase countdown is a
// multiple of 48/40/12, at one of the three
// runtime points of 0x826A80 under a 90-degree rotation (func_001029C0,
// func_00102B08, func_00102918). Phase 2 sets +0 = 2 while its +0x1F0 timer
// (set to 60 by the 0x823B10 callback) runs, else 1. After a spawn a 1-in-6
// chance of func_001F02C0(point, 0x41D/0x41E/0x41F, 100.0f). Then
// func_001B17A0. States 2/3 func_001AFC10.
// Divergence (97.12%, same size): the three 12-byte period tables are copied
// to the stack with a 64-bit load/store plus one 32-bit word; mwcc 2.3.3
// (and 2.4 / 991202) move that last word through an FPR (every struct-copy,
// initializer, pack and field spelling tried), the original through a GPR,
// which also shifts the copy's register choice. No other 12-byte table copy
// exists in the game to compare against. Everything after the copies
// matches (the case-1 and case-2/3 exits are `return`, which restores the
// original delay-slot speculation; `fired = 1` follows the spawn check).
typedef struct { int v[3]; } I3;
extern unsigned char D_00810702;
extern I3 D_overlay_AREA06_00826AC8;
extern I3 D_overlay_AREA06_00826AD8;
extern I3 D_overlay_AREA06_00826AE8;
extern float D_overlay_AREA06_00826AB0[4];
extern float D_overlay_AREA06_00826A80[3][4];
extern float D_700036A0[16];
extern void func_overlay_AREA06_00823B10(unsigned char *self);
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102918(void *dst, void *src, void *v);
extern char *func_001EFEB0(int id, void *m);
extern void func_001F02C0(void *pos, int id, float f12);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA06_00823B10(unsigned char *self) {
    I3 a;
    I3 b;
    I3 c;
    int fired;
    int *w;
    char *p;
    w = (int *)(self + 0x1F0);
    a = D_overlay_AREA06_00826AC8;
    b = D_overlay_AREA06_00826AD8;
    c = D_overlay_AREA06_00826AE8;
    switch (self[4]) {
    case 0:
        *(float **)(self + 0x30) = D_overlay_AREA06_00826AB0;
        *(void (**)(unsigned char *))(self + 0x34) = func_overlay_AREA06_00823B10;
        w[0] = 0;
        w[2] = func_00122BB8() % 3;
        w[1] = a.v[w[2]];
        w[4] = 0;
        ((float *)w)[3] = (float)func_00122BB8() / 2147483648.0f;
        self[4] = 1;
        self[5] = 0;
    case 1:
        if (D_00810702 == 1) {
            break;
        }
        fired = 0;
        switch (self[5]) {
        case 0:
            self[0] = 2;
            w[1]--;
            if (w[1] < 0) {
                w[1] = b.v[w[2]];
                self[5] = 1;
            }
            if (w[1] % 48 == 0) {
                func_001029C0(D_700036A0);
                func_00102B08(D_700036A0, D_700036A0, 1.5707964f);
                func_00102918(D_700036A0, D_700036A0, D_overlay_AREA06_00826A80[func_00122BB8() % 3]);
                func_001EFEB0(2, D_700036A0);
                fired = 1;
            }
            break;
        case 1:
            self[0] = 2;
            w[1]--;
            if (w[1] < 0) {
                w[1] = c.v[w[2]];
                self[5] = 2;
            }
            if (w[1] % 40 == 0) {
                func_001029C0(D_700036A0);
                func_00102B08(D_700036A0, D_700036A0, 1.5707964f);
                func_00102918(D_700036A0, D_700036A0, D_overlay_AREA06_00826A80[func_00122BB8() % 3]);
                func_001EFEB0(3, D_700036A0);
                fired = 1;
            }
            break;
        case 2:
            self[0] = 1;
            w[1]--;
            if (w[1] < 0) {
                w[2] = func_00122BB8() % 3;
                w[1] = a.v[w[2]];
                self[5] = 0;
            }
            if (w[1] % 12 == 0) {
                func_001029C0(D_700036A0);
                func_00102B08(D_700036A0, D_700036A0, 1.5707964f);
                func_00102918(D_700036A0, D_700036A0, D_overlay_AREA06_00826A80[func_00122BB8() % 3]);
                func_001EFEB0(4, D_700036A0);
                p = func_001EFEB0(0x8000003B, D_700036A0);
                if (p != 0) {
                    p[5] = 1;
                    *(int *)(p + 0x1F0) = 6;
                    *(float *)(p + 0x1F4) = 14.0f;
                    *(float *)(p + 0x1F8) = 0.5f;
                }
                fired = 1;
            }
            if (w[0] == 0) {
                self[0] = 1;
            } else {
                w[0]--;
                self[0] = 2;
            }
            break;
        }
        if (fired) {
            if (func_00122BB8() % 6 == 0) {
                switch (func_00122BB8() % 4) {
                case 0:
                    func_001F02C0(D_overlay_AREA06_00826A80[func_00122BB8() % 3], 0x41D, 100.0f);
                    break;
                case 1:
                    func_001F02C0(D_overlay_AREA06_00826A80[func_00122BB8() % 3], 0x41E, 100.0f);
                    break;
                case 2:
                    func_001F02C0(D_overlay_AREA06_00826A80[func_00122BB8() % 3], 0x41F, 100.0f);
                    break;
                case 3:
                    break;
                }
            }
        }
        func_001B17A0(self);
        return;
    case 2:
    case 3:
        func_001AFC10(self);
        return;
    }
}
