// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 5, 2026-10-20). The
// 99.86 NEARMISS residual (the page compare's value in $v1 and its result in
// $at) was the spelling of the two page compares: `n > 1` and
// `p[0x1C] < n - 1` give the target's compare-and-branch through $at and its
// operand order; `n >= 2` and `n - 1 > p[0x1C]` did not (MATCHING_GUIDE
// "Decomp lane round 5").

// HUD/overlay layout dispatcher. When arg1==0 it draws a fixed background set
// (selected by global event flags D_00810E70 bits 0x2000/0x8000, then a second
// pair selected by bits 0x1000/0x4000) via func_00207E40 GS register writes.
// Otherwise it formats two number strings (func_001C5FB0/func_00123168) into a
// gp-relative scratch buffer D_00275870 and blits them (func_001CBA50), then
// draws up to two arrow indicators gated by the page index in field 0x1C.
extern int func_001C5FB0(int a, int b, int c);
extern void func_00123168(void *dst, int v);
extern void func_001CBA50(int a, int b, int c, int d, int e, void *f, void *g);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern int D_002659C0[];
extern int D_00275870;
extern unsigned short D_00810E70[8];

void func_002134C0(unsigned char *p, int arg1) {
    int sp3C;
    int n;

    func_00207D00(1, 3);
    if (arg1 == 0) {
        if (D_00810E70[0] & 0x2000) {
            func_00207E40(1, 0x8A00, 0x7BC0, 0x40, 0x20, 0x80808080, 0x20044B8559322240ULL);
            func_00207E40(1, 0x7200, 0x7BC0, 0x40, 0x20, 0x80808080, 0x2004490559322210ULL);
        } else if (D_00810E70[0] & 0x8000) {
            func_00207E40(1, 0x8A00, 0x7BC0, 0x40, 0x20, 0x80808080, 0x2004498559322220ULL);
            func_00207E40(1, 0x7200, 0x7BC0, 0x40, 0x20, 0x80808080, 0x20044B0559322230ULL);
        } else {
            func_00207E40(1, 0x8A00, 0x7BC0, 0x40, 0x20, 0x80808080, 0x2004498559322220ULL);
            func_00207E40(1, 0x7200, 0x7BC0, 0x40, 0x20, 0x80808080, 0x2004490559322210ULL);
        }
        if (D_00810E70[0] & 0x1000) {
            func_00207E40(1, 0x7F80, 0x7A80, 0x20, 0x20, 0x80808080, 0x200450855542227AULL);
            func_00207E40(1, 0x7F80, 0x83C0, 0x20, 0x20, 0x80808080, 0x200450255542227CULL);
        } else if (D_00810E70[0] & 0x4000) {
            func_00207E40(1, 0x7F80, 0x7A80, 0x20, 0x20, 0x80808080, 0x2004502555422278ULL);
            func_00207E40(1, 0x7F80, 0x83C0, 0x20, 0x20, 0x80808080, 0x200450855542227EULL);
        } else {
            func_00207E40(1, 0x7F80, 0x7A80, 0x20, 0x20, 0x80808080, 0x2004502555422278ULL);
            func_00207E40(1, 0x7F80, 0x83C0, 0x20, 0x20, 0x80808080, 0x200450255542227CULL);
        }
    } else {
        func_00123168(&sp3C, func_001C5FB0(*(short *)(p + 0x1C) + 1, 2, 1));
        func_001CBA50(1, 0x870, 0x7AC, 0x10, 0x10, &sp3C, &D_00275870);
        func_00123168(&sp3C, func_001C5FB0(D_002659C0[p[0x1B]], 2, 1));
        func_001CBA50(1, 0x898, 0x7AC, 0x10, 0x10, &sp3C, &D_00275870);

        n = D_002659C0[p[0x1B]];
        if (n > 1) {
            if (*(short *)(p + 0x1C) < n - 1) {
                if (*(int *)0x70003B64 & 4) {
                    func_00207E40(1, 0x8D00, 0x7F40, 0x20, 0x20, 0x80808080, 0x2004502555422268ULL);
                } else {
                    func_00207E40(1, 0x8D00, 0x7F40, 0x20, 0x20, 0x80808080, 0x200450855542226AULL);
                }
            }
            if (*(short *)(p + 0x1C) > 0) {
                if (*(int *)0x70003B64 & 4) {
                    func_00207E40(1, 0x7100, 0x7F40, 0x20, 0x20, 0x80808080, 0x200450255542226CULL);
                } else {
                    func_00207E40(1, 0x7100, 0x7F40, 0x20, 0x20, 0x80808080, 0x200450855542226EULL);
                }
            }
        }
    }
}
