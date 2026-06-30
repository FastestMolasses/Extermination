// NEARMISS func_001C5FB0  (vram 0x001C5FB0, 0x164 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/control-flow byte-identical; every residual is a saved-register coloring permutation (target s5/s6/s7/s4/s3/s2/s1/s0/s8 + `started` in a0 vs mwcc's different assignment) plus prologue scheduling of the `7-arg1` computation. Declaration-order and guard-sense reshaping only lower the score. Re...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 86.57% (mwcc 2.3.3; 991202 = 74.26%). Body/control-flow fully
// recovered and instruction-stream identical; ALL residuals are register-
// coloring permutation: the target colors arg1(count)->s5, arg0(rem)->s6,
// idx->s7, table->s4, divisor->s2, digit->s3, p->s1, i->s0, neg->s8, and keeps
// `started` in the caller-saved a0; mwcc assigns a different saved-register
// permutation (and schedules the `7-arg1` prologue computation differently).
// Declaration reorder / guard-sense (`0 < arg1` vs `arg1 > 0`) only shifts the
// score; this is regalloc-ORDER, permuter territory.
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
    char *p;
    int rem;
    int idx;
    int i;
    int started;
    int neg;
    int divisor;
    int digit;

    rem = arg0;
    p = D_008111D0;
    idx = 7 - arg1;
    started = 0;
    neg = 0;
    i = 0;
    if (arg1 > 0) {
        do {
            divisor = D_0026E2F0[idx];
            digit = rem / divisor;
            idx += 1;
            if ((arg2 != 0) && (started == 0) && (digit == 0) && (i != (arg1 - 1))) {
                *p = 0x20;
                p += 1;
            } else {
                if (digit < 0) {
                    if (neg == 0) {
                        *p = 0x2D;
                        p += 1;
                        neg = 1;
                    }
                    *p = func_0011E860(digit) + 0x30;
                    p += 1;
                } else {
                    *p = digit + 0x30;
                    p += 1;
                }
                started = 1;
            }
            i += 1;
            rem -= digit * divisor;
        } while (i < arg1);
    }
    *p = 0;
    return D_008111D0;
}
