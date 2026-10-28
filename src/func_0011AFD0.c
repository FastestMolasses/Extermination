// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU1: run state decoded from VPU_STAT (COP2 control register 29), first
// match wins: bit 9 -> 1 (stopped on D-bit), bit 10 -> 2 (stopped on T-bit),
// bit 11 -> 3 (stopped by force break), bit 8 -> 4 (running), else 0 (idle).

int func_0011AFD0(void) {
    int stat;
    int state;
    asm volatile("cfc2.ni %0, $vi29" : "=r"(stat));
    if (stat & 0x200) {
        state = 1;
    } else if (stat & 0x400) {
        state = 2;
    } else if (stat & 0x800) {
        state = 3;
    } else if (stat & 0x100) {
        state = 4;
    } else {
        state = 0;
    }
    return state;
}
