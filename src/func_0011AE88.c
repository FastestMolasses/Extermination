// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU1: FBRST (COP2 control register 28) |= 0x200, i.e. reset VU1.

void func_0011AE88(void) {
    int fbrst;
    asm volatile("cfc2.ni %0, $vi28" : "=r"(fbrst));
    fbrst |= 0x200;
    asm volatile("ctc2.ni %0, $vi28" : : "r"(fbrst));
}
