// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK soft-float helper: builds a double-precision bit pattern from the signed
// 64-bit integer x (sign bit, exponent n + 1023 where n is the index of the
// highest set bit of |x|, mantissa from x shifted left by 64 - n then
// arithmetically right by 12 when n < 53) and then converts that assembled
// 64-bit VALUE (not a reinterpretation) to double with func_00125848 (signed
// 64-bit to double); a negative pattern is converted as unsigned: halve with
// the low bit kept sticky, convert, then double it with func_00126EC8 (add).
// x == 0 returns 0. Note the mantissa is taken from x, not |x|, and |LONG_MIN|
// never reaches 1 in the bit-count loop. Shift counts are 6-bit on the EE.

extern long func_00125848(long a);
extern long func_00126EC8(long a, long b);

long func_0010D098(long x) {
    long n;
    unsigned long sign;
    long a;
    long d;
    long e;

    n = 0;
    sign = 0;
    if (x == 0) {
        return 0;
    }
    a = x;
    if (x < 0) {
        a = -x;
        sign = (unsigned long)0x8000 << 48;
    }
    while (a != 1) {
        n++;
        a >>= 1;
    }
    e = n + 0x3FF;
    if (n < 53) {
        x <<= (int)(64 - n);
        x >>= 12;
    }
    d = sign | (e << 52) | x;
    if (d >= 0) {
        return func_00125848(d);
    }
    d = func_00125848((d & 1) | ((unsigned long)d >> 1));
    return func_00126EC8(d, d);
}
