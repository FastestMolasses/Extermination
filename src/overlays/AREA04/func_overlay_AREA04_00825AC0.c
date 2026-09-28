// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay, runtime 0x00825B00 (splat/link name 00825AC0;
// overlay code is linked 0x40 below where it runs), 0x254 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA04; lane A04C).
// Role: lift. +0x1F0 moving flag, +0x1F4 direction, +0x1F8 base height,
// +0x1FC offset. State 0 stores base = +0xB4 and starts raised (offset 10,
// direction 1) when D_00810834, else lowered; model refresh, state 1. State
// 1 counts +0x28 down, at 0 sets the moving flag and plays
// func_001FBD50(self, 0x451 or 0x452, 0, 300). While moving the offset
// changes by 0.1923077 per frame up to 10 or down to 0, where it stops,
// flips direction and sets D_00810834 = 1 / 0; +0xB4 = base + offset.
// Then func_001B1B70 and the +0x4C method. Other states func_001AFC10.
typedef struct {
    int on;
    int dir;
    float base;
    float off;
} Lift;
extern unsigned char D_00810834;
extern int func_001B0FD0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *m);
extern void func_001FBD50(unsigned char *e, int id, int b, float f);
extern void func_001B1B70(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA04_00825AC0(unsigned char *self) {
    Lift *lf;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            *(short *)(self + 0x28) = 0;
            ((Lift *)(self + 0x1F0))->on = 0;
            ((Lift *)(self + 0x1F0))->base = *(float *)(self + 0xB4);
            if (D_00810834 != 0) {
                ((Lift *)(self + 0x1F0))->dir = 1;
                ((Lift *)(self + 0x1F0))->off = 10.0f;
                *(float *)(self + 0xB4) += ((Lift *)(self + 0x1F0))->off;
            } else {
                ((Lift *)(self + 0x1F0))->dir = 0;
                ((Lift *)(self + 0x1F0))->off = 0.0f;
            }
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
            self[4] = 1;
            self[0] = 1;
        }
        break;
    case 1:
        if (*(short *)(self + 0x28) != 0) {
            *(short *)(self + 0x28) -= 1;
            if (*(short *)(self + 0x28) == 0) {
                ((Lift *)(self + 0x1F0))->on = 1;
                if (((Lift *)(self + 0x1F0))->dir == 0) {
                    func_001FBD50(self, 0x451, 0, 300.0f);
                } else {
                    func_001FBD50(self, 0x452, 0, 300.0f);
                }
            }
        }
        if (*(int *)(self + 0x1F0) != 0) {
            int *w = (int *)(self + 0x1F0);
            int *dp = w + 1;
            float *op;
            switch (*(int *)(self + 0x1F4)) {
            case 0:
                op = (float *)(w + 3);
                *(float *)(self + 0x1FC) += 0.1923077f;
                if (!(*(float *)(self + 0x1FC) <= 10.0f)) {
                    *op = 10.0f;
                    *(int *)(self + 0x1F0) = 0;
                    *dp = 1;
                    D_00810834 = 1;
                }
                *(float *)(self + 0xB4) = *(float *)(self + 0x1F8) + *op;
                break;
            case 1:
                op = (float *)(w + 3);
                *(float *)(self + 0x1FC) -= 0.1923077f;
                if (*(float *)(self + 0x1FC) < 0.0f) {
                    *op = 0.0f;
                    *(int *)(self + 0x1F0) = 0;
                    *dp = 0;
                    D_00810834 = 0;
                }
                *(float *)(self + 0xB4) = *(float *)(self + 0x1F8) + *op;
                break;
            }
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
        }
        func_001B1B70(self);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
    default:
        func_001AFC10(self);
        break;
    }
}
