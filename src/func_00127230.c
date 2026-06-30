// NEARMISS func_00127230  (vram 0x00127230, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.56% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc register-allocation coloring. NOTE: this func is Sony PS2 SDK softfloat-library code built with ee-gcc 2.9-991111-01 (tell-tales: sd/ld 64-bit ra saves, daddu reg moves, unfilled jal;nop) -- it is NOT mwcc-class, so mwcc 2.3.3 cannot help and pct_991202/pct_233 are N/A; matched via the eeg...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// NEARMISS 94.6% via ee-gcc 2.9-991111-01 -O2 (Sony SDK softfloat lane; this func
// is ee-gcc-class, NOT mwcc -- sd/ld 64-bit saves, daddu moves). Body/logic op-for-op
// faithful; sole residual is ee-gcc register coloring (target b.mant->t0, quotient->a3,
// result-ptr->a1; mine differs), which also shifts the cls<2 early-out branch layout.
// Permuter territory.
//
// Software-float binary op (mantissa restoring-division) on the unpacked form.
// Each operand is decoded by func_00126BE8 into a 0x18-byte record:
//   +0x00 cls  (class/tag: <2 = NaN/zero special, 2 and 4 = inf/special)
//   +0x04 sign
//   +0x08 exp
//   +0x10 mant (64-bit)
// The result record is chosen among operand a, operand b, a shared special
// constant D_00281B28, or computed in-place into a's record by a restoring
// long-division of the 64-bit mantissas, then repacked by func_00126AB8.
struct fp {
    unsigned int cls;
    int sign;
    int exp;
    int pad;
    unsigned long mant;
};

extern void func_00126BE8(long *src, struct fp *dst);
extern void func_00126AB8(struct fp *res, struct fp *a);
extern struct fp D_00281B28;

void func_00127230(long arg0, long arg1)
{
    struct fp a;
    struct fp b;
    long ia;
    long ib;
    struct fp *res;

    ia = arg0;
    ib = arg1;
    func_00126BE8(&ia, &a);
    func_00126BE8(&ib, &b);

    if (a.cls < 2U) {
        res = &a;
    } else {
        res = &b;
        if (b.cls >= 2U) {
            a.sign ^= b.sign;
            if ((a.cls ^ 4) == 0 || (a.cls ^ 2) == 0) {
                res = &a;
                if (a.cls == b.cls) {
                    res = &D_00281B28;
                }
            } else if ((b.cls ^ 4) == 0) {
                a.mant = 0;
                res = &a;
                a.exp = 0;
            } else if ((b.cls ^ 2) == 0) {
                res = &a;
                a.cls = 4;
            } else {
                unsigned long rem;
                unsigned long den;
                unsigned long bit;
                long quo;

                rem = a.mant;
                den = b.mant;
                a.exp = a.exp - b.exp;
                if (rem < den) {
                    rem *= 2;
                    a.exp = a.exp - 1;
                }
                bit = 0x8000UL << 0x2D;
                quo = 0;
                do {
                    if (rem >= den) {
                        quo |= bit;
                        rem -= den;
                    }
                    bit >>= 1;
                    rem *= 2;
                } while (bit != 0);

                if ((quo & 0xFF) == 0x80) {
                    if (quo & 0x100) {
                        quo += 0x80;
                    } else if (rem != 0) {
                        quo += 0x80;
                    }
                }
                a.mant = quo;
                res = &a;
            }
        }
    }
    func_00126AB8(res, &a);
}
