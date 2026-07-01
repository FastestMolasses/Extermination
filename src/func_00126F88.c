// NEARMISS func_00126F88  (vram 0x00126F88, 0x2A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 59.59% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// This function is Sony PS2 SDK softfloat-library code (module tag 'lowmem' in FUNCTIONS.csv; sd/ld 64-bit ra saves, daddu reg moves -- the neighboring already-decompiled func_00126EC8/func_00126F20/func_00127230 in the same cluster are all `// COMPILER: eegcc`), so it is NOT mwcc-class and pct_991...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Software-float binary op (mantissa multiply) on the unpacked form -- the
// multiplication twin of func_00127230 (division). Each operand is decoded
// by func_00126BE8 into a 0x18-byte record:
//   +0x00 cls  (class/tag: <2 = NaN/zero special, 2 and 4 = inf/special)
//   +0x04 sign
//   +0x08 exp
//   +0x10 mant (64-bit)
// The result record is chosen among operand a, operand b, a shared special
// constant D_00281B28, or computed into a fresh 0x18-byte record (res) by a
// 64x64->128 mantissa multiply (four 32x32->64 partial products via
// func_00125F48) followed by normalize + round-to-nearest-even on the 128-bit
// product, then repacked by func_00126AB8.
typedef unsigned long long u64;

struct fp {
    unsigned int cls;
    int sign;
    int exp;
    int pad;
    u64 mant;
};

extern void func_00126BE8(long *src, struct fp *dst);
extern void func_00126AB8(struct fp *res);
extern u64 func_00125F48(u64 a, u64 b);
extern struct fp D_00281B28;

void func_00126F88(long arg0, long arg1)
{
    struct fp a;
    struct fp b;
    long ia;
    long ib;
    struct fp res;
    struct fp *out;

    ia = arg0;
    ib = arg1;
    func_00126BE8(&ia, &a);
    func_00126BE8(&ib, &b);

    if (a.cls < 2U) {
        out = &a;
        a.sign = (a.sign != b.sign);
    } else if (b.cls < 2U) {
        out = &b;
        b.sign = (a.sign != b.sign);
    } else if (a.cls == 4) {
        if (b.cls != 2) {
            out = &a;
            a.sign = (a.sign != b.sign);
        } else {
            out = &D_00281B28;
        }
    } else if (b.cls == 4) {
        if (a.cls == 2) {
            out = &D_00281B28;
        } else {
            out = &b;
            b.sign = (a.sign != b.sign);
        }
    } else if (a.cls == 2) {
        out = &a;
        a.sign = (a.sign != b.sign);
    } else if (b.cls == 2) {
        out = &b;
        b.sign = (a.sign != b.sign);
    } else {
        u64 mask = 0xFFFFFFFFUL;
        u64 alo = a.mant & mask;
        u64 ahi = a.mant >> 32;
        u64 blo = b.mant & mask;
        u64 bhi = b.mant >> 32;
        u64 p0 = func_00125F48(blo, alo);
        u64 p1 = func_00125F48(blo, ahi);
        u64 p2 = func_00125F48(bhi, alo);
        u64 p3 = func_00125F48(bhi, ahi);
        u64 mid = p1 + p2;
        u64 lo = p0 + (mid << 32);
        u64 carry = (mid < p1) ? 1 : 0;
        u64 hi = ((carry << 32) | (lo < p0)) + ((mid >> 32) + p3);
        int exp = a.exp + b.exp + 4;

        if (hi > (0xFFFFFFFFUL >> 3)) {
            do {
                exp += 1;
                if ((int)(hi & 1) != 0) {
                    lo = (lo >> 1) | (0x8000ULL << 48);
                }
                hi = hi >> 1;
            } while (hi > (0xFFFFFFFFUL >> 3));
        }
        if (hi > (0xFFFFFFFFUL >> 4)) {
            do {
                u64 h2 = hi * 2;
                exp -= 1;
                hi = (lo & (0x8000ULL << 48)) ? (h2 | 1) : h2;
                lo *= 2;
            } while (hi > (0xFFFFFFFFUL >> 4));
        }
        if ((hi & 0xFF) == 0x80) {
            if (hi & 0x100) {
                hi += 0x80;
            } else if (lo != 0) {
                hi += 0x80;
            }
        }
        res.mant = hi;
        res.cls = 3;
        res.sign = (a.sign != b.sign);
        res.exp = exp;
        out = &res;
    }
    func_00126AB8(out);
}
