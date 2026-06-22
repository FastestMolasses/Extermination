// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Calls func_001FCBD0(0x3C, 0x9B, 7, 0x1E, 0x70808080) (a 5-arg call; the 5th
// arg in $t0 is the packed color/flags constant 0x70808080 that m2c missed),
// then dispatches on flag bits in the u16 D_00810E74: bit 0x40 -> func_0020CD40
// and return 1; else bit 0x20 -> func_0020CD60 and return 1; else bit 0x10 ->
// func_0020CD60 and return 2; else return 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906). Verified objdiff 100% vs
// build/expected/func_00227270.o. (991202 = 66.7%, hitting the branch-likely /
// delay-slot wall.) The if/else-if/else chain with a single trailing return v0
// is load-bearing: it lets mwcc compute each next andi in the prior branch's
// delay slot exactly as the target does.
extern void func_001FCBD0(int, int, int, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern unsigned short D_00810E74;

int func_00227270(void) {
    int v0;

    func_001FCBD0(0x3C, 0x9B, 7, 0x1E, 0x70808080);
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        v0 = 1;
    } else if (D_00810E74 & 0x20) {
        func_0020CD60();
        v0 = 1;
    } else if (D_00810E74 & 0x10) {
        func_0020CD60();
        v0 = 2;
    } else {
        v0 = 0;
    }
    return v0;
}
