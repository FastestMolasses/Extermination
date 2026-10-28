// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK printf helper: converts the 64-bit IEEE double bit pattern `m` (passed in
// one GPR, sign ignored) to an int. Unbiased exponent e = exponent - 1075:
// below -53 the result is 0; 13 or more saturates to 9999. Otherwise the 53-bit
// mantissa (hidden bit restored) is shifted left by e, or for e < 0 shifted
// right keeping two extra bits and rounded up only when both of them are set
// (fraction >= 0.75), else truncated. The 64-bit result is truncated to int.
// The exponent and mantissa are updated in place (one local each), as the
// original's register use shows.

int func_0010D008(unsigned long m) {
    long e;

    e = (m << 1) >> 53;
    e -= 1075;
    if (e < -53) {
        return 0;
    }
    if (e >= 13) {
        return 9999;
    }
    m = (m << 12) >> 12;
    m |= (unsigned long)1 << 52;
    if (e < 0) {
        e = -e;
        m >>= (int)(e - 2);
        if ((m & 3) == 3) {
            m = (m >> 2) + 1;
        } else {
            m >>= 2;
        }
    } else {
        m <<= (int)e;
    }
    return (int)m;
}
