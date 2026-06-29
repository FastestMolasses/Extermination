// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Emits a GS/GIF register-write sequence (func_00207E40) bracketed by
// func_00207D00(1,0)/(1,3), selected by arg1. Each func_00207E40 call passes
// a base color (arg f) and a packed 64-bit GIF/texture tag (arg g, built by
// CW/mwcc as dsll32/ori/or). arg1!=0 vs ==0 picks one of two 4-call blocks
// for the first group; a second arg1 test either emits a final 0x7A80 write
// or falls through to func_001FCF30(p[0x12],0x64,0x2F).
extern void func_001FCF30(int a, int b, int c);
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);

void func_002131B0(unsigned char *p, int arg1) {
    func_00207D00(1, 0);
    if (arg1 != 0) {
        func_00207E40(1, 0x7000, 0x7BA0, 0x100, 0x80, 0x40808080, 0x20044A05E1321D00ULL);
        func_00207E40(1, 0x7000, 0x7FA0, 0x100, 0x80, 0x40808080, 0x20044C05E1321E00ULL);
        func_00207E40(1, 0x8000, 0x7BA0, 0x100, 0x80, 0x40808080, 0x20044A85E1321D40ULL);
        func_00207E40(1, 0x8000, 0x7FA0, 0x100, 0x80, 0x40808080, 0x20044C85E1321E40ULL);
    } else {
        func_00207E40(1, 0x7000, 0x7BA0, 0x100, 0x80, 0x40808080, 0x20044E85E1321F00ULL);
        func_00207E40(1, 0x7000, 0x7FA0, 0x100, 0x80, 0x40808080, 0x20044385E1322000ULL);
        func_00207E40(1, 0x8000, 0x7BA0, 0x100, 0x80, 0x40808080, 0x20044305E1321F40ULL);
        func_00207E40(1, 0x8000, 0x7FA0, 0x100, 0x80, 0x40808080, 0x20044505E1322040ULL);
    }
    func_00207D00(1, 3);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, 0x20045005DD4221E0ULL);
    func_00207E40(1, 0x7100, 0x7900, 0x100, 0x40, 0x80808080, 0x20044785A1322180ULL);
    if (arg1 != 0) {
        func_00207E40(1, 0x8400, 0x7A80, 0x80, 0x40, 0x80808080, 0x20044E059D3221C0ULL);
        return;
    }
    func_001FCF30(p[0x12], 0x64, 0x2F);
}
