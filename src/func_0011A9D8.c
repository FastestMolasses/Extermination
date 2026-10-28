// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// VU0 reset: sets bit 1 (VU0 reset) of FBRST (COP2 control register vi28).
// The COP2 control-register moves are user-approved VU0 inline asm statements.

void func_0011A9D8(void) {
    unsigned int fbrst;
    asm volatile("cfc2.ni %0, $vi28" : "=r"(fbrst));
    fbrst |= 2;
    asm volatile("ctc2.ni %0, $vi28" : : "r"(fbrst));
}
