// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Four-tile startup compositor. The original 00207E40 sprite primitive
// consumes a 64-bit TEX0 in its seventh argument, not four untyped ints.
extern void func_00207D00(int slot, int mode);
extern void func_00207E40(int slot, int x, int y, int w, int h,
                         unsigned int rgba, long long tex0);

void func_001ABF90(long long top_left, long long top_right,
                   long long bottom_left, long long bottom_right)
{
    func_00207D00(1, 0);
    func_00207E40(1, 0x7000, 0x8100, 256, 256, 0x80808080, bottom_left);
    func_00207E40(1, 0x8000, 0x8100, 256, 256, 0x80808080, bottom_right);
    func_00207E40(1, 0x7000, 0x7900, 256, 256, 0x80808080, top_left);
    func_00207E40(1, 0x8000, 0x7900, 256, 256, 0x80808080, top_right);
}
