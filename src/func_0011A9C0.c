// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// VU0 status: returns 1 when the VU0 bits of VPU_STAT (COP2 control register
// vi29, low four bits) are exactly 1 (busy, no stop flag), else 0. The COP2
// control-register move is a user-approved VU0 inline asm statement.

int func_0011A9C0(void) {
    unsigned int stat;
    asm volatile("cfc2.ni %0, $vi29" : "=r"(stat));
    return (stat & 0xF) == 1;
}
