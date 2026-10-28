// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU1: sets (on == 1) or clears (anything else) FBRST bit 11 (0x800, the
// VU1 T-bit enable) in COP2 control register 28.

void func_0011AF80(int on) {
    unsigned int fbrst;
    asm volatile("cfc2.ni %0, $vi28" : "=r"(fbrst));
    if (on == 1) {
        fbrst |= 0x800;
    } else {
        fbrst &= ~0x800;
    }
    asm volatile("ctc2.ni %0, $vi28" : : "r"(fbrst));
}
