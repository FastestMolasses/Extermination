// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003A28
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Jump-trajectory setup, long-drop variant of func_0017D940 (called from
// func_00162190 case 0 when e[0x25C] is set to 1): fixed reach 4.1 along the
// facing e+0x290/0x298, frames = distance / 0.8, at least 12; the vertical
// velocity is (e+0x258 - 4) / (frames - 8), gravity e+0x26C = 0.6 with step
// 0.6 / ((frames - 8) / 2). Same outputs as func_0017D940.
// func_0011E748 = sqrtf. Callers declare it (void); it really takes e.
extern float func_0011E748(float);
extern int float_to_int(float);
extern float D_70003A20;
extern float D_70003A24;
extern float D_70003A28;

void func_0017DAF0(unsigned char *e) {
    *(float *)(e + 0x2F4) = *(float *)(e + 0x2E0);
    *(float *)(e + 0x2F8) = *(float *)(e + 0x2E8);
    D_70003A20 = (*(float *)(e + 0x2F4) + 4.1f * *(float *)(e + 0x290)) - *(float *)(e + 0xB0);
    D_70003A24 = (*(float *)(e + 0x2F8) + 4.1f * *(float *)(e + 0x298)) - *(float *)(e + 0xB8);
    D_70003A28 = func_0011E748(D_70003A20 * D_70003A20 + D_70003A24 * D_70003A24) / 0.8f;
    if (D_70003A28 < 12.0f) {
        D_70003A28 = 12.0f;
    }
    *(short *)(e + 0x28) = float_to_int(D_70003A28 - 1.0f);
    *(float *)(e + 0x2E0) = D_70003A20 / D_70003A28;
    *(float *)(e + 0x2E8) = D_70003A24 / D_70003A28;
    *(float *)(e + 0x2E4) = (*(float *)(e + 0x258) - 4.0f) / (D_70003A28 - 8.0f);
    *(float *)(e + 0x258) += *(float *)(e + 0xB4);
    *(float *)(e + 0x26C) = 0.6f;
    *(float *)(e + 0x270) = *(float *)(e + 0x26C) / ((D_70003A28 - 8.0f) / 2.0f);
}
