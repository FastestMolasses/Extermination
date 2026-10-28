// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// VU0 run state from VPU_STAT (COP2 control register vi29): 1 when bit 1 (D-bit
// stop) is set, else 2 for bit 2 (T-bit stop), else 3 for bit 3 (force break),
// else 4 when bit 0 (busy) is set, else 0 (idle). func_0011A9F0 consumes these
// codes. The COP2 move is a user-approved VU0 inline asm statement.

int func_0011AB20(void) {
    unsigned int stat;
    asm volatile("cfc2.ni %0, $vi29" : "=r"(stat));
    if (stat & 2) {
        return 1;
    }
    if (stat & 4) {
        return 2;
    }
    if (stat & 8) {
        return 3;
    }
    return (stat & 1) << 2;
}
