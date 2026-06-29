// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// GS/GIF register-setup sequence (same idiom as func_002125B0). Builds 64-bit
// GIF tag/register-write values and issues a fixed series of func_00207E40
// blits bracketed by func_00207D00, with one optional extra blit when arg0!=0.
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern void func_0020A7A0(unsigned long long a);

void func_00210A00(int arg0) {
    func_0020A7A0(0x20043C259D422050ULL);
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, 0x7900, 0x100, 0x100, 0x4C808080, 0x20042E8621321D00ULL);
    func_00207E40(1, 0x8000, 0x7900, 0x100, 0x100, 0x4C808080, 0x20042E8621321D40ULL);
    func_00207E40(1, 0x7000, 0x8100, 0x100, 0x40, 0x4C808080, 0x20042E85A1321F40ULL);
    func_00207E40(1, 0x8000, 0x8100, 0x100, 0x40, 0x4C808080, 0x20042E85A1321FC0ULL);
    func_00207E40(1, 0x7800, 0x8300, 0x100, 0x80, 0x40808080, 0x20043205E1321F00ULL);
    if (arg0 != 0) {
        func_00207E40(1, 0x8790, 0x8420, 0x80, 0x40, 0x80808080, 0x20043A859D422040ULL);
    }
    func_00207D00(1, 3);
    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, 0x20043A25DD422020ULL);
}
