// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.57% NEARMISS).
// Fixes read from the original instructions:
// - `started = 1` is set at the end of each digit arm (if / else-if / else
//   chain), not once after a nested if; each arm then branches to the shared
//   tail on its own.
// - The loop is a plain `for (i = 0; i < arg1; i++)`; its entry test is the
//   `0 < arg1` form the original uses.
// - No separate `rem` copy: the value parameter itself is reduced each place.
// - Local declaration order (i, p, divisor, digit, idx, neg, started) and
//   computing `idx` before `p` give the original's saved-register colouring
//   and prologue order.
//
// Fixed-point integer-to-decimal-digits formatter. arg0 = value, arg1 = field
// width (1..7 digits), arg2 = leading-blank flag. Walks a per-width divisor
// table D_0026E2F0 from index (7-arg1) upward; for each place computes
// digit = rem/divisor. When arg2 set, suppresses leading zeros as ' '(0x20)
// until the first significant digit (except the final place). Negative digits
// emit a '-' (0x2D) once, then func_0011E860 (abs/neg helper) + '0'. Writes the
// resulting NUL-terminated ASCII string into the global buffer D_008111D0 and
// returns it.
extern int func_0011E860(int);
extern int D_0026E2F0[];
extern char D_008111D0[];

char *func_001C5FB0(int arg0, int arg1, int arg2) {
    int i;
    char *p;
    int divisor;
    int digit;
    int idx;
    int neg;
    int started;

    idx = 7 - arg1;
    p = D_008111D0;
    started = 0;
    neg = 0;
    for (i = 0; i < arg1; i++) {
        divisor = D_0026E2F0[idx];
        digit = arg0 / divisor;
        idx += 1;
        if ((arg2 != 0) && (started == 0) && (digit == 0) && (i != (arg1 - 1))) {
            *p = 0x20;
            p += 1;
        } else if (digit < 0) {
            if (neg == 0) {
                *p = 0x2D;
                p += 1;
                neg = 1;
            }
            *p = func_0011E860(digit) + 0x30;
            p += 1;
            started = 1;
        } else {
            *p = digit + 0x30;
            p += 1;
            started = 1;
        }
        arg0 -= digit * divisor;
    }
    *p = 0;
    return D_008111D0;
}
