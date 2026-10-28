// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU1: writes the start address (16 bits) to CMSAR1 (COP2 control register
// 31), which starts VU1 at that microprogram address. func_0011AF00 uses it with
// func_0011B028() (VU1 TPC) to resume VU1.

void func_0011B018(unsigned short addr) {
    asm volatile("ctc2.ni %0, $vi31" : : "r"(addr));
}
