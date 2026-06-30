// NEARMISS func_00127E40  (vram 0x00127E40, 0x160 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 50.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Compiler-version / stack-codegen wall: target uses a 0x50 frame with 8-byte `sd` $ra/$s0 saves and pins &b in callee-saved $s0 with the two scratch structs at sp+0x00/sp+0x10; both available mwcc builds (2.3.3, 991202) emit a 0x40 frame with 16-byte `sq` saves, no callee-saved reg, and structs at...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS ~51% (mwcc 991202 = 51.43%, 2.3.3 = 50.84%). The instruction-level
// LOGIC is fully recovered, but this walls on a COMPILER-VERSION codegen
// difference the available builds cannot reproduce: the target frame is 0x50
// and saves $ra/$s0 with 8-byte `sd` while keeping &b (sp+0x10) live in the
// callee-saved $s0 across the whole function; both mwcc 2.3.3 and 991202 emit
// a 0x40 frame, save with 16-byte `sq`, use NO callee-saved register, and lay
// the two 4-int scratch structs at sp+0x10/sp+0x20 (target: sp+0x00/sp+0x10)
// with the float spills after. This `sd`-save + s0-pin + struct-first layout is
// characteristic of the ORIGINAL CW 2.3.1 build; declaration reorder and an
// explicit `int *pb=b;` pin both LOWER the score. Parked: compiler-version /
// stack-layout+save-format wall, not the clean-store nop.
//
// Two-input fixed-point combiner. Spills the two float args, decodes each via
// func_001278C0(&f, slot) into a 4-int record (a = first, b = second). Early
// outs when a record's tag word [0] < 2 (selects a or b as the result record).
// XORs the two records' [1] words. Branches on the tag combinations (tag^4,
// tag^2, a.tag==b.tag -> the constant record D_00281B40) to pick/zero fields,
// else runs a restoring binary long-division bit loop (dividend a[2], divisor
// b[3]) accumulating a 7-bit quotient in `r`, with a round-to-even style
// adjustment when (r&0x7F)==0x40, storing the quotient into a[3]. Finally calls
// func_001277B0(result_record, a).
extern void func_001277B0(int *, int *);
extern void func_001278C0(float *, int *);
extern int D_00281B40;

void func_00127E40(float fparg0, float fparg1) {
    int a[4];
    int b[4];
    float f0;
    float f1;
    int *out;
    int m;
    int n;
    int r;
    int v;

    f0 = fparg0;
    f1 = fparg1;
    func_001278C0(&f0, a);
    func_001278C0(&f1, b);
    if ((unsigned int)a[0] < 2U) {
        out = a;
    } else {
        out = b;
        if ((unsigned int)b[0] >= 2U) {
            a[1] ^= b[1];
            if (((a[0] ^ 4) == 0) || ((a[0] ^ 2) == 0)) {
                out = a;
                if (a[0] == b[0]) {
                    out = &D_00281B40;
                }
            } else if ((b[0] ^ 4) == 0) {
                a[3] = 0;
                out = a;
                a[2] = 0;
            } else if ((b[0] ^ 2) == 0) {
                out = a;
                a[0] = 4;
            } else {
                m = a[3];
                r = a[2] - b[2];
                n = m < b[3];
                a[2] = r;
                if (n != 0) {
                    m *= 2;
                    a[2] = r - 1;
                    n = m < b[3];
                }
                v = 0x40000000;
                r = 0;
loop_15:
                if (n == 0) {
                    r |= v;
                    m -= b[3];
                }
                v = (unsigned int)v >> 1;
                m *= 2;
                if (v != 0) {
                    n = m < b[3];
                    goto loop_15;
                }
                if ((r & 0x7F) == 0x40) {
                    if (r & 0x80) {
                        r += 0x40;
                    } else {
                        r = (m != 0) ? (r + 0x40) : r;
                    }
                }
                a[3] = r;
                out = a;
            }
        }
    }
    func_001277B0(out, a);
}
