// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Returns FBRST bit 2 (VU0 D-bit enable, COP2 control register vi28).
// The COP2 move is a user-approved VU0 inline asm statement.

int func_0011AB00(void) {
    unsigned int fbrst;
    asm volatile("cfc2.ni %0, $vi28" : "=r"(fbrst));
    return (fbrst >> 2) & 1;
}
