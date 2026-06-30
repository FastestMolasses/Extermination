// NEARMISS func_00117BA0  (vram 0x00117BA0, 0x88 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.56% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation permutation. Size-matched (34/34 instrs); semantics fully decoded and correct. Expected computes sll $t0,$a1,4 FIRST and keeps the struct base in a single persistent register ($a1) reused for all D_00281AC0 accesses; our ee-gcc 2.9-991111-01 -O2 emits lui $t0 first, uses $t1 for the shift, and a ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_00242230[];

struct g {
    void *p0;          /* 0x00 */
    unsigned char *p4; /* 0x04 */
    int pad8;          /* 0x08 */
    unsigned char *pC; /* 0x0C */
};
extern struct g D_00281AC0;

unsigned char func_00117BA0(int a0, int a1) {
    int t0 = a1 << 4;
    unsigned char *p;
    unsigned char v0;

    p = D_00281AC0.p4 + t0;
    D_00281AC0.p4 = p;
    if (a0 == 4) {
        v0 = p[0xC];
    } else {
        unsigned char *a = (unsigned char *)D_00281AC0.p0;
        unsigned char *e = D_00281AC0.pC;
        unsigned int v1 = p[0xC] >> 2;
        unsigned int t = (a[2] >> 2) << 5;
        v1 = D_00242230[v1 + t];
        {
            unsigned int q = (e[4] >> 2) << 5;
            v0 = D_00242230[(v1 >> 2) + q];
        }
    }
    D_00281AC0.p4 = D_00281AC0.p4 - t0;
    return v0;
}
