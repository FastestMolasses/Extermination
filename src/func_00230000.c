// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// AI-state handler, keyed on the global mode byte D_00810700 (level/area kind):
//   case 11: re-target via func_0022FCA0(self, other); pick a height/goal value --
//     if func_00194D10(self, other, 1) (a line-of-sight / reachability test) succeeds,
//     goal = 6 + (11 + (self->0x5C + other->0xA4)), else goal = self->0x8C +
//     (11 + (self->0x5C + other->0xA4)); func_00191D40(self, goal, 4.0) (approach with
//     rate 4); if other's state (+0x230) is 2 or 15, func_00191000(self, other);
//     then func_0018D7B0(self, 0) (set anim/state 0).
//   case 0: if other's +0xA4 (height/offset) < -83.0, do a special reposition:
//     zero self+0x98, self->0x10 = 120.0, self->0x18 = -1590.0,
//     func_00191D40(self, -67.5, 4.0), func_0018D7B0(self, 5),
//     func_0018C6A0(self+0x10, D_008105D0, 4.0) (blend vector toward global pos),
//     func_0018C4B0(D_008105D0, self->0x14, 4.0); else fall through to default.
//   default: same shape as case 11 but goal always uses self->0x8C.
// Tail: self->0x44 = func_001B1240(D_008105D0, D_008105E0, D_008105E8)
// (distance/angle query against the global position).

extern unsigned char D_00810700;
extern char D_008105D0[];
extern float D_008105E0;
extern float D_008105E8;

extern void func_0022FCA0(char *self, char *other);
extern int func_00194D10(char *self, char *other, int c);
extern void func_00191D40(char *self, float a, float b);
extern void func_00191000(char *self, char *other);
extern void func_0018D7B0(char *self, int n);
extern void func_0018C6A0(void *dst, void *src, float rate);
extern void func_0018C4B0(void *dst, float val, float rate);
extern float func_001B1240(void *p, float a, float b);

void func_00230000(char *self, char *other) {
    switch (D_00810700) {
    case 11:
        func_0022FCA0(self, other);
        {
            float f;
            if (func_00194D10(self, other, 1)) {
                f = 6.0f + (11.0f + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4)));
            } else {
                f = *(float *)(self + 0x8C) + (11.0f + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4)));
            }
            func_00191D40(self, f, 4.0f);
        }
        {
            int s = *(int *)(other + 0x230);
            if (s == 2 || s == 0xF) {
                func_00191000(self, other);
            }
        }
        func_0018D7B0(self, 0);
        break;
    case 0:
        if (*(float *)(other + 0xA4) < -83.0f) {
            *(int *)(self + 0x98) = 0;
            *(float *)(self + 0x10) = 120.0f;
            *(float *)(self + 0x18) = -1590.0f;
            func_00191D40(self, -67.5f, 4.0f);
            func_0018D7B0(self, 5);
            func_0018C6A0(self + 0x10, D_008105D0, 4.0f);
            func_0018C4B0(D_008105D0, *(float *)(self + 0x14), 4.0f);
            break;
        }
        /* FALLTHROUGH */
    default:
        func_0022FCA0(self, other);
        func_00191D40(self, *(float *)(self + 0x8C) + (11.0f + (*(float *)(self + 0x5C) + *(float *)(other + 0xA4))), 4.0f);
        {
            int s = *(int *)(other + 0x230);
            if (s == 2 || s == 0xF) {
                func_00191000(self, other);
            }
        }
        func_0018D7B0(self, 0);
        break;
    }
    *(float *)(self + 0x44) = func_001B1240(D_008105D0, D_008105E0, D_008105E8);
}
