// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// actor_anim: if the float at +0x224 is nonzero, arm an anim/state record
// (byte +0x4 = 2, byte +0x5 = 0x19, byte +0x6 = 0) and return 1; if the float
// is exactly 0.0 the record is left untouched and 0 is returned.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 was the idiom-13 clean-store delay-slot nop (991202
// fills the bc1t delay slot; CW + 2.3.3 leave the return-0 paddub there).
// The float compare operand order (c.eq.s value,zero) is recovered via a
// `float zero` local; the not-equal arm is written as the if-TRUE block so the
// return-0 lands in the bc1t delay slot (CW two-exit form). objdiff 100.0.
int func_001823E0(char *p) {
    float zero = 0.0f;
    if (*(float *)(p + 0x224) != zero) {
        *(char *)(p + 4) = 2;
        *(char *)(p + 5) = 0x19;
        *(char *)(p + 6) = 0;
        return 1;
    }
    return 0;
}
