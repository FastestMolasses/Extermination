// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// FPU quad/vertex setup. Args: func(int $a0, float $f12, int $a1) — the float
// param does not consume a GPR slot, so the trailing int flag lands in $a1 and
// $a0 (arg0) is forwarded unchanged as the first arg to func_0019AFE0. Two
// scratchpad structs at 0x700038A0 / 0x700038B0 are real symbols (address-of via
// %hi/%lo) while the per-field stores use raw 0x7000xxxx addressing. Builds the
// two 4-float records (x = src.x + dir.x*scale, plus a constant 1.0 in the 4th
// slot) from base vectors at 0x70003050.. and a scale chosen by arg2 (0 -> 1.0/
// 0.5 ; else 5.0 / 5.0, bias 4.01), then returns func_0019AFE0(arg0,&A,&B,7)!=0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was wall #13 (it fills the clean-store delay slot where
// CW leaves a nop). 2.3.3 leaves the nop -> objdiff 100% vs build/expected.
extern int func_0019AFE0(int, float *, float *, int);
extern float D_700038A0;
extern float D_700038B0;

int func_001775E0(int arg0, float arg1, int arg2) {
    float f0, f1, f2;
    float f3, f4, f5, f6, f7;
    int v0;

    if (arg2 == 0) {
        f1 = 1.0f;
        f2 = 0.5f;
        f0 = 1.0f;
    } else {
        f1 = 5.0f;
        f0 = 4.01f;
        f2 = 5.0f;
    }
    f4 = *(float *)0x70003050;
    f7 = arg1 + f0;
    f3 = *(float *)0x70003060;
    f6 = *(float *)0x70003058;
    f5 = *(float *)0x70003068;
    *(float *)0x700038A0 = f4 + (f3 * f1);
    *(float *)0x700038A8 = f6 + (f5 * f1);
    *(float *)0x700038A4 = f7;
    *(float *)0x700038AC = 1.0f;
    *(float *)0x700038B0 = f4 - (f3 * f2);
    *(float *)0x700038B8 = f6 - (f5 * f2);
    *(float *)0x700038B4 = f7;
    *(float *)0x700038BC = 1.0f;
    v0 = func_0019AFE0(arg0, &D_700038A0, &D_700038B0, 7);
    if (v0 != 0) {
        v0 = 1;
        return v0;
    }
    v0 = 0;
    return v0;
}
