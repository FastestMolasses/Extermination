// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets up a hardware/scratch register block at 0x700038A0..0x700038BC and
// kicks off two interpolation/transform builds. Writes: +A0 = 0, +A4/+A8 =
// (fparg0,fparg1), +AC = 1.0f bits (0x3F800000). Branches on arg1: when 0,
// +B0 = 0xC0900000 (-4.5f), else +B0 = 0x40900000 (+4.5f); both paths then
// write +B4/+B8 = (fparg0,fparg1) and +BC = 1.0f bits. Calls func_001026A0
// twice (dst D_700038C0 / D_700038D0, src actor+0xD0, table D_700038A0 /
// D_700038B0), then func_0019AFE0(actor, D_700038C0, D_700038D0, 7).
//
// The 0x3F800000 (+BC) store is duplicated into both if/else arms so the
// shared constant register is reused, matching CW. The 0x700038xx stores use
// absolute literal addressing; the call args reference the same locations as
// relocatable externs (R_MIPS_HI16/LO16). Built with mwcc 2.3.3
// (mwcps2-2.3.3-000906): 991202 reaches only 92.86% (wall #13 delay-slot and
// branch-lowering deltas). Verified objdiff 100% vs func_0017E6E0.o.
extern int func_001026A0(int, int, int);
extern int func_0019AFE0(int, int, int, int);
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;

void func_0017E6E0(int arg0, int arg1, float fparg0, float fparg1) {
    *(int *)0x700038A0 = 0;
    *(float *)0x700038A4 = fparg0;
    *(float *)0x700038A8 = fparg1;
    *(int *)0x700038AC = 0x3F800000;
    if (arg1 == 0) {
        *(int *)0x700038B0 = 0xC0900000;
        *(float *)0x700038B4 = fparg0;
        *(float *)0x700038B8 = fparg1;
        *(int *)0x700038BC = 0x3F800000;
    } else {
        *(int *)0x700038B0 = 0x40900000;
        *(float *)0x700038B4 = fparg0;
        *(float *)0x700038B8 = fparg1;
        *(int *)0x700038BC = 0x3F800000;
    }
    func_001026A0((int)&D_700038C0, arg0 + 0xD0, (int)&D_700038A0);
    func_001026A0((int)&D_700038D0, arg0 + 0xD0, (int)&D_700038B0);
    func_0019AFE0(arg0, (int)&D_700038C0, (int)&D_700038D0, 7);
}
