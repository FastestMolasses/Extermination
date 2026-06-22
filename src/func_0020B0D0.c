// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sprite/quad-draw dispatcher keyed on game-state flags D_00810E70. Calls
// func_00207D00(1, 3) to set up a draw context, then issues two func_00207E40
// quad draws (color modulate 0x80808080) selecting which 64-bit field of the
// actor (arg1) supplies the texture/UV operand: bit 0x1000 set -> +0x58,+0x50;
// else bit 0x4000 set -> +0x48,+0x60; else (default) -> +0x48,+0x50. arg0 is
// taken in a0 but immediately clobbered by the func_00207D00(1,...) call.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// 991202 residual is wall #13 (it fills the trailing `b` delay slot where CW
// leaves a nop). 2.3.3 leaves the nop -> byte-identical. Verified objdiff 100%
// vs build/expected/func_0020B0D0.o.
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, int, long long);
extern unsigned short D_00810E70;

void func_0020B0D0(int arg0, char *arg1) {
    func_00207D00(1, 3);
    if (D_00810E70 & 0x1000) {
        func_00207E40(1, 0x7800, 0x7B30, 0x20, 0x20, 0x80808080, *(long long *)(arg1 + 0x58));
        func_00207E40(1, 0x7800, 0x8240, 0x20, 0x20, 0x80808080, *(long long *)(arg1 + 0x50));
    } else if (D_00810E70 & 0x4000) {
        func_00207E40(1, 0x7800, 0x7B30, 0x20, 0x20, 0x80808080, *(long long *)(arg1 + 0x48));
        func_00207E40(1, 0x7800, 0x8240, 0x20, 0x20, 0x80808080, *(long long *)(arg1 + 0x60));
    } else {
        func_00207E40(1, 0x7800, 0x7B30, 0x20, 0x20, 0x80808080, *(long long *)(arg1 + 0x48));
        func_00207E40(1, 0x7800, 0x8240, 0x20, 0x20, 0x80808080, *(long long *)(arg1 + 0x50));
    }
}
