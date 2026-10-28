// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU: returns 1 when VPU_STAT (COP2 control register 29) shows VU0's
// state nibble == 1 (running only) or VU1's (bits 8..11) == 0x100 (running
// only), else 0. The VU1 context save/restore (func_0011B050 / func_0011B1E0)
// refuse to run while it returns 1.

int func_0011ADD0(void) {
    int stat;
    asm volatile("cfc2.ni %0, $vi29" : "=r"(stat));
    if ((stat & 0xF) == 1) {
        return 1;
    }
    if ((stat & 0xF00) == 0x100) {
        return 1;
    }
    return 0;
}
