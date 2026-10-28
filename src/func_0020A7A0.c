// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.67% NEARMISS).
// Fixes read from the original instructions:
//  - D_002655A0 is an array of 0x20-byte records (Layer); walking it as Layer *
//    with e++ and declaring the locals in the order i, inner, outer, color, e
//    gives the original saved-register assignment (record pointer last).
//  - The phase step is a compound `+=` on the field (operand order of the add).
//  - Cases 0/1 store the decremented coordinate before the wrap test and again
//    after it (two stores, as in the original).
//  - Case 2 loads y only after the first conversion call, takes the
//    1792 - x difference into a temporary before scaling by 16, and the first
//    converted value lives in the "inner" register.
//  - func_00128250 returns unsigned int (its src/ definition); the colour is
//    accumulated as unsigned int; func_00207E40's last parameter is the 64-bit
//    tex0 of its src/ definition, so this function's argument is 64-bit too.
//
// Three overlay layers (0x20-byte records at D_002655A0): layers 1 and 2 run a
// countdown timer, and when it is negative advance a phase angle by 0.25 deg until
// 180, then re-arm the timer to a random 60..237 frames and reset phase/position.
// Layers 0 and 1 scroll (x or y wraps from below 0 to 255) and tile a 8x32 grid of
// 256x128 sprites; layer 2 drifts both coordinates by 0.3 and draws one sprite
// whose size grows with the position. Alpha is 64 * func_0011E2A8(phase in radians).

typedef struct {
    float x;      /* 0x00 */
    float y;      /* 0x04 */
    float phase;  /* 0x08 */
    int timer;    /* 0x0C */
    float r;      /* 0x10 */
    float g;      /* 0x14 */
    float b;      /* 0x18 */
    float a;      /* 0x1C */
} Layer;

extern void func_00207D00(int slot, int mode);
extern void func_00207E40(int slot, int x0, int y0, int width, int height,
                          unsigned int rgba, unsigned long long tex0);
extern int func_00122BB8(void);
extern unsigned int func_00128250(float a);
extern int float_to_int(float a);
extern float func_0011E2A8(float a);

extern Layer D_002655A0[];

void func_0020A7A0(unsigned long long arg0) {
    int i;
    int inner;
    int outer;
    unsigned int color;
    Layer *e;
    float f1;
    float f20;
    float f21;

    func_00207D00(1, 0);

    e = D_002655A0;
    for (i = 0; i < 3; i++, e++) {
        switch (i) {
        case 0:
            break;
        case 1:
        case 2:
            if (e->timer < 0) {
                e->phase += 0.25f;
                if (!(e->phase < 180.0f)) {
                    e->timer = ((func_00122BB8() % 60) * 3) + 0x3C;
                    e->phase = 0.0f;
                    e->x = 0.0f;
                    e->y = 0.0f;
                }
            } else {
                e->timer = e->timer - 1;
            }
            break;
        }

        switch (i) {
        case 0:
            f1 = e->x - 0.5f;
            e->x = f1;
            if (f1 < 0.0f) {
                f1 = 255.0f;
            }
            e->x = f1;
            e->r = 96.0f;
            e->g = 96.0f;
            e->b = 96.0f;
            e->a = 64.0f;
            e->a = e->a * func_0011E2A8((3.1415927f * e->phase) / 180.0f);
            break;
        case 1:
            f1 = e->y - 0.5f;
            e->y = f1;
            if (f1 < 0.0f) {
                f1 = 255.0f;
            }
            e->y = f1;
            e->r = 96.0f;
            e->g = 96.0f;
            e->b = 96.0f;
            e->a = 64.0f;
            e->a = e->a * func_0011E2A8((3.1415927f * e->phase) / 180.0f);
            break;
        case 2:
            if (e->timer < 0) {
                e->x += 0.3f;
                e->y += 0.3f;
                e->r = 96.0f;
                e->g = 96.0f;
                e->b = 96.0f;
                e->a = 64.0f;
                e->a = e->a * func_0011E2A8((3.1415927f * e->phase) / 180.0f);
            }
            break;
        }

        switch (i) {
        case 0:
        case 1:
            outer = 0x400;
            do {
                inner = 0x400;
                do {
                    color = func_00128250(e->r);
                    color |= func_00128250(e->g) << 8;
                    color |= func_00128250(e->b) << 0x10;
                    color |= func_00128250(e->a) << 0x18;
                    func_00207E40(1, (inner + float_to_int(e->x)) << 4,
                                  (outer + float_to_int(e->y)) << 4,
                                  0x100, 0x80, color, arg0);
                    inner += 0x100;
                } while (inner < 0xC00);
                outer += 0x40;
            } while (outer < 0xC00);
            break;
        case 2:
            if (e->timer < 0) {
                f20 = e->x;
                f1 = 1792.0f - f20;
                inner = float_to_int(16.0f * f1);
                f21 = e->y;
                outer = float_to_int(16.0f * (1936.0f + (-f21 / 2.0f)));
                color = func_00128250(e->r);
                color |= func_00128250(e->g) << 8;
                color |= func_00128250(e->b) << 0x10;
                color |= func_00128250(e->a) << 0x18;
                func_00207E40(1, inner, outer,
                              float_to_int(2.0f * f20) + 0x200,
                              float_to_int(2.0f * f21) + 0x1C0,
                              color, arg0);
            }
            break;
        }
    }
}
