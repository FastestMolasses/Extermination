// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Direction from point `from` to point `to` (x, y, z floats): stores the
// delta into the scratchpad vector 0x70003A20 (dx at +0, dy at +4, dz at +8),
// the horizontal length sqrt(dx*dx + dz*dz) (func_0011E748) at +0xC, then the
// angle func_0011E620(horizontal length, dy) back into +0, and passes it to
// func_001B1510. Scratchpad words are per-address relocated externs
// (idiom-32).
extern float func_0011E620(float y, float x);
extern float func_0011E748(float x);
extern float func_001B1510(float x);
extern float D_70003A20;
extern float D_70003A24;
extern float D_70003A28;
extern float D_70003A2C;

void func_0017A800(float *from, float *to) {
    D_70003A20 = to[0] - from[0];
    D_70003A28 = to[2] - from[2];
    D_70003A2C = func_0011E748(D_70003A20 * D_70003A20 +
                                         D_70003A28 * D_70003A28);
    D_70003A24 = to[1] - from[1];
    D_70003A20 = func_0011E620(D_70003A2C, D_70003A24);
    func_001B1510(D_70003A20);
}
