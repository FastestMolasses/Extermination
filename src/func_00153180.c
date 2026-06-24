// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-axis float clamp. arg0 = entity (floats at 0xB0/0xB4/0xB8), arg1 = state
// record. Clears the result flag byte at arg1+0x55, then (only when the latch
// byte arg1+0x54 is 0) clamps the X channel at 0xB0 into [D_00246720, D_00246724]
// and the Z channel at 0xB8 into [D_00246728, D_0024672C], OR-ing a bit into the
// 0x55 flag byte for each clamp that fires (1/2 for the X lo/hi clamp, 4/8 for Z),
// and floors the 0xB4 channel at 35.0 (0x420C0000). Note the <= vs < asymmetry:
// the lower bound uses !(v <= lo) so the clamp fires only when strictly greater,
// the upper bound uses v < hi.
//
// mwcc 2.3.3 byte-matches; the pinned 991202 build caps at 93.7% — it fills the
// clean-store delay-slot nop after the swc1 stores (wall #13) where CW/2.3.3
// leave the nop. Verified objdiff 100.0% vs build/expected.
extern float D_00246720;
extern float D_00246724;
extern float D_00246728;
extern float D_0024672C;

void func_00153180(int arg0, int arg1) {
    float f1;

    *(char *)(arg1 + 0x55) = 0;
    if (*(char *)(arg1 + 0x54) == 0) {
        f1 = *(float *)(arg0 + 0xB0);
        if (!(f1 <= D_00246720)) {
            *(char *)(arg1 + 0x55) = *(char *)(arg1 + 0x55) | 1;
            *(float *)(arg0 + 0xB0) = D_00246720;
        } else if (f1 < D_00246724) {
            *(char *)(arg1 + 0x55) = *(char *)(arg1 + 0x55) | 2;
            *(float *)(arg0 + 0xB0) = D_00246724;
        }
        f1 = *(float *)(arg0 + 0xB8);
        if (!(f1 <= D_00246728)) {
            *(char *)(arg1 + 0x55) = *(char *)(arg1 + 0x55) | 4;
            *(float *)(arg0 + 0xB8) = D_00246728;
        } else if (f1 < D_0024672C) {
            *(char *)(arg1 + 0x55) = *(char *)(arg1 + 0x55) | 8;
            *(float *)(arg0 + 0xB8) = D_0024672C;
        }
        if (*(float *)(arg0 + 0xB4) < 35.0f) {
            *(float *)(arg0 + 0xB4) = 35.0f;
        }
    }
}
