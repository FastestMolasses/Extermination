// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003A28
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Jump-trajectory setup (called from func_00162190 case 0, short drop).
// Saves the current horizontal velocity e+0x2E0/0x2E8 into e+0x2F4/0x2F8, then
// aims at a landing point ahead of the facing direction (e+0x290/0x298) by a
// reach of 6.3 (e[0x25C] == 3) or 4.1, relative to the position e+0xB0/0xB8.
// The scratchpad floats D_70003A20/24 hold the delta and D_70003A28 the
// frame count = distance / speed (0.9 or 0.8), at least 8. Writes the frame
// counter e+0x28 (frames - 1), the per-frame velocity e+0x2E0/0x2E4/0x2E8
// (vertical from the height e+0x258), raises e+0x258 by e+0xB4, sets the
// gravity accumulator e+0x26C to 0.6 and its step e+0x270 = 0.6 / (frames/2).
// func_0011E748 = sqrtf. Callers declare it (void); it really takes e.
extern float func_0011E748(float);
extern int float_to_int(float);
extern float D_70003A20;
extern float D_70003A24;
extern float D_70003A28;

void func_0017D940(unsigned char *e) {
    float reach;
    float speed;

    *(float *)(e + 0x2F4) = *(float *)(e + 0x2E0);
    *(float *)(e + 0x2F8) = *(float *)(e + 0x2E8);
    if (e[0x25C] == 3) {
        reach = 6.3f;
        speed = 0.9f;
    } else {
        reach = 4.1f;
        speed = 0.8f;
    }
    D_70003A20 = (*(float *)(e + 0x2F4) + *(float *)(e + 0x290) * reach) - *(float *)(e + 0xB0);
    D_70003A24 = (*(float *)(e + 0x2F8) + *(float *)(e + 0x298) * reach) - *(float *)(e + 0xB8);
    D_70003A28 = func_0011E748(D_70003A20 * D_70003A20 + D_70003A24 * D_70003A24) / speed;
    if (D_70003A28 < 8.0f) {
        D_70003A28 = 8.0f;
    }
    *(short *)(e + 0x28) = float_to_int(D_70003A28 - 1.0f);
    *(float *)(e + 0x2E0) = D_70003A20 / D_70003A28;
    *(float *)(e + 0x2E8) = D_70003A24 / D_70003A28;
    *(float *)(e + 0x2E4) = *(float *)(e + 0x258) / D_70003A28;
    *(float *)(e + 0x258) += *(float *)(e + 0xB4);
    *(float *)(e + 0x26C) = 0.6f;
    *(float *)(e + 0x270) = *(float *)(e + 0x26C) / (D_70003A28 / 2.0f);
}
