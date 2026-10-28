// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU1: returns FBRST bit 10 (VU1 D-bit enable) as 0/1.

int func_0011AFB0(void) {
    unsigned int fbrst;
    asm volatile("cfc2.ni %0, $vi28" : "=r"(fbrst));
    return (fbrst >> 10) & 1;
}
