// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 73.30% NEARMISS).
// Fixes read from the original instructions (same fixes as its byte-matched twin
// func_001EBD20):
//  - the float at 0x70003434 is element 13 of the scratchpad matrix D_70003400
//    (a 4x4 float matrix, row 3 = translation, .y), bumped with a compound
//    `+= 2.5f` on that element; the literal-address `x = x + 2.5f` spelling
//    scheduled the FP constants and the gp load in a different order (the
//    "emit-order wall" was the spelling).
//  - the flag test is one if/else over the two func_001CFBE0 calls.
//
// func_001EBC30 (vram 0x001EBC30, 0xE4 bytes).
// Same template as func_001EBD20: copy the source matrix (arg0) into the
// scratchpad matrix D_70003400, move its translation y up by 2.5, emit via
// func_001CFB50 (radius/params from D_00275C34 +0x54/+0x5C, 1.0f, 1e-6f, 3.0f),
// then func_001CFBE0 with table D_00256310 when D_00275C30->+0x38 is zero, else
// D_00256280. D_00275C34 / D_00275C30 are gp-relative.
extern char D_0081F8F0[0x100];
extern char D_00256310[0x100];
extern char D_00256280[0x100];
extern char *D_00275C30;
extern char *D_00275C34;
extern float D_70003400[16];

extern void copy_qw4(float *dst, int src);
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001EBC30(int arg0, int arg1) {
    copy_qw4(D_70003400, arg0);
    D_70003400[13] += 2.5f;
    func_001CFB50(D_0081F8F0, 0, D_70003400,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 3.0f);
    if (*(int *)(D_00275C30 + 0x38) == 0) {
        func_001CFBE0(arg1, 1, D_00256310, D_0081F8F0, 0);
    } else {
        func_001CFBE0(arg1, 1, D_00256280, D_0081F8F0, 0);
    }
}
