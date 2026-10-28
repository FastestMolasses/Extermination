// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU0: returns VU0's TPC (COP2 control register 26), low 16 bits.

unsigned short func_0011AB70(void) {
    unsigned int v;
    asm volatile("cfc2.ni %0, $vi26" : "=r"(v));
    return v;
}
