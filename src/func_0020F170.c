// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Issues a fixed sequence of GS/GIF register setups: one 64-bit value to
// func_0020A7A0, then four func_00207E40 calls (each: rect args + a color
// constant + a packed 64-bit GS register value) bracketing a func_00207D00.
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_0020A7A0(unsigned long long v);

void func_0020F170(void) {
    func_0020A7A0(0x2003C8A59D421DB0ULL);
    func_00207E40(1, 0x7BC0, 0x7CF0, 0x80, 0x80, 0x40808080, 0x2003C405DD321D00ULL);
    func_00207E40(1, 0x7800, 0x8300, 0x100, 0x80, 0x40808080, 0x2003CA85E1421D40ULL);
    func_00207D00(1, 3);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, 0x2003CA25DD421D20ULL);
    func_00207E40(1, 0x7100, 0x7900, 0x80, 0x40, 0x80808080, 0x2003C8859D421DA0ULL);
}
