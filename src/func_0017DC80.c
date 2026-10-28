// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003A28
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Jump-trajectory setup, third variant of func_0017D940 (func_00162190 case 0,
// state 0x1E path): reach 6.3 (e[0x25C] == 3) or 4.1, frames = distance / 0.8,
// at least 12; the vertical velocity is e+0x258 / frames, gravity e+0x26C =
// 1.0 with step 1.0 / (frames / 2). Same outputs as func_0017D940.
// func_0011E748 = sqrtf. Callers declare it (void); it really takes e.
extern float func_0011E748(float);
extern int float_to_int(float);
extern float D_70003A20;
extern float D_70003A24;
extern float D_70003A28;

void func_0017DC80(unsigned char *e) {
    float reach;

    *(float *)(e + 0x2F4) = *(float *)(e + 0x2E0);
    *(float *)(e + 0x2F8) = *(float *)(e + 0x2E8);
    if (e[0x25C] == 3) {
        reach = 6.3f;
    } else {
        reach = 4.1f;
    }
    D_70003A20 = (*(float *)(e + 0x2F4) + *(float *)(e + 0x290) * reach) - *(float *)(e + 0xB0);
    D_70003A24 = (*(float *)(e + 0x2F8) + *(float *)(e + 0x298) * reach) - *(float *)(e + 0xB8);
    D_70003A28 = func_0011E748(D_70003A20 * D_70003A20 + D_70003A24 * D_70003A24);
    D_70003A28 /= 0.8f;
    if (D_70003A28 < 12.0f) {
        D_70003A28 = 12.0f;
    }
    *(short *)(e + 0x28) = float_to_int(D_70003A28 - 1.0f);
    *(float *)(e + 0x2E0) = D_70003A20 / D_70003A28;
    *(float *)(e + 0x2E8) = D_70003A24 / D_70003A28;
    *(float *)(e + 0x2E4) = *(float *)(e + 0x258) / D_70003A28;
    *(float *)(e + 0x258) += *(float *)(e + 0xB4);
    *(float *)(e + 0x26C) = 1.0f;
    *(float *)(e + 0x270) = *(float *)(e + 0x26C) / (D_70003A28 / 2.0f);
}
