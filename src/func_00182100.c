// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Horizontal distance from the entity (e+0xB0/0xB8) to its linked target:
// e[0x2F1] == 1 uses the object at e+0x160, 2 the object at e+0x15C (their
// position at +0xC0/+0xC8); any other value gives 0. The deltas are left in
// the scratchpad floats D_70003A20/24. func_0011E748 = sqrtf.
extern float func_0011E748(float);
extern float D_70003A20;
extern float D_70003A24;

float func_00182100(unsigned char *e) {
    float d;
    if (e[0x2F1] == 1) {
        D_70003A20 = *(float *)(*(unsigned char **)(e + 0x160) + 0xC0) - *(float *)(e + 0xB0);
        D_70003A24 = *(float *)(*(unsigned char **)(e + 0x160) + 0xC8) - *(float *)(e + 0xB8);
        d = func_0011E748(D_70003A20 * D_70003A20 + D_70003A24 * D_70003A24);
    } else if (e[0x2F1] == 2) {
        D_70003A20 = *(float *)(*(unsigned char **)(e + 0x15C) + 0xC0) - *(float *)(e + 0xB0);
        D_70003A24 = *(float *)(*(unsigned char **)(e + 0x15C) + 0xC8) - *(float *)(e + 0xB8);
        d = func_0011E748(D_70003A20 * D_70003A20 + D_70003A24 * D_70003A24);
    } else {
        d = 0.0f;
    }
    return d;
}
