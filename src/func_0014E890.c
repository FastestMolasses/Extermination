// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Scale pulse. While ctx+0x5C is set, ctx+0x40 grows by 0.2 per call up to 2.0
// (clamping and clearing ctx+0x5C there); otherwise it shrinks by 0.05 down to
// 1.0. The integer part of 4096*s (modff, func_0011E0A8) goes to both halfwords
// +0x8A/+0x8C of the node at self+0x13C, and the integer part of 4096/s to
// +0x8A/+0x8C of the node at self+0x140 (fixed-point 4.12 scale pair).
extern float func_0011E0A8(float x, float *iptr);
extern int float_to_int(float f);

void func_0014E890(char *self, char *ctx) {
    float ip;

    if (*(char *)(ctx + 0x5C) != 0) {
        *(float *)(ctx + 0x40) += 0.2f;
        if (!(*(float *)(ctx + 0x40) < 2.0f)) {
            *(char *)(ctx + 0x5C) = 0;
            *(float *)(ctx + 0x40) = 2.0f;
        }
    } else {
        *(float *)(ctx + 0x40) -= 0.05f;
        if (*(float *)(ctx + 0x40) <= 1.0f) {
            *(float *)(ctx + 0x40) = 1.0f;
        }
    }
    func_0011E0A8(4096.0f * *(float *)(ctx + 0x40), &ip);
    *(short *)(*(char **)(self + 0x13C) + 0x8A) = float_to_int(ip);
    *(short *)(*(char **)(self + 0x13C) + 0x8C) = float_to_int(ip);
    func_0011E0A8(4096.0f * (1.0f / *(float *)(ctx + 0x40)), &ip);
    *(short *)(*(char **)(self + 0x140) + 0x8A) = float_to_int(ip);
    *(short *)(*(char **)(self + 0x140) + 0x8C) = float_to_int(ip);
}
