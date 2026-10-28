// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Tilt toggle driven by self+7. Step 0 waits for the short timer ctx+0x56 to
// reach 0; then it advances the step, latches ctx+0x58 = ctx+0x59, reloads the
// timer with a random 0..127 ((rand >> 17) & 0x7F, func_00122BB8), and sets the
// turn speed ctx+0x3C to 0.5 (tilted) or 2.5 (level) degrees per tick, falling
// into step 1. Step 1 eases the angle at (self+0x174)+0x78 toward 35 degrees
// (0.6108653 rad) when ctx+0x59 is set, else toward 0, by speed*PI/180 per call
// via func_001B12B0(target, current, step); on arrival it returns to step 0 and
// flips ctx+0x59.
extern int func_00122BB8(void);
extern float func_001B12B0(float cur, float target, float step);

void func_0014E670(char *self, char *ctx) {
    float target;

    switch (*(unsigned char *)(self + 7)) {
    case 0:
        if (*(short *)(ctx + 0x56) != 0) {
            break;
        }
        *(unsigned char *)(self + 7) = *(unsigned char *)(self + 7) + 1;
        *(char *)(ctx + 0x58) = *(char *)(ctx + 0x59);
        *(short *)(ctx + 0x56) = (func_00122BB8() >> 17) & 0x7F;
        if (*(char *)(ctx + 0x59) != 0) {
            *(float *)(ctx + 0x3C) = 0.5f;
        } else {
            *(float *)(ctx + 0x3C) = 2.5f;
        }
        /* fallthrough */
    case 1:
        if (*(char *)(ctx + 0x59) != 0) {
            target = 0.6108653f;
        } else {
            target = 0.0f;
        }
        *(float *)(*(char **)(self + 0x174) + 0x78) =
            func_001B12B0(target, *(float *)(*(char **)(self + 0x174) + 0x78),
                          (3.1415927f * *(float *)(ctx + 0x3C)) / 180.0f);
        if (target == *(float *)(*(char **)(self + 0x174) + 0x78)) {
            *(unsigned char *)(self + 7) = 0;
            *(char *)(ctx + 0x59) = 1 - *(char *)(ctx + 0x59);
        }
        break;
    }
}
