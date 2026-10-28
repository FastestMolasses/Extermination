// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled by the 2.96 gas (// EEGCC_AS: 2.96), which fills the delay slot as
// the original does.
// The VU0 macro / COP2 transfers are GCC-style asm statements, one instruction
// each (user-approved 2026-10-28 for the VU0 class); everything else is C.
//
// SDK VU0: start a VU0 microprogram. Writes the start address (16 bits) to
// CMSAR0 (COP2 control register 27) and issues the macro-mode call through it.
// func_0011AA50 uses it with func_0011AB70() to resume VU0 at its saved TPC.

void func_0011AB60(unsigned short addr) {
    asm volatile("ctc2.ni %0, $vi27" : : "r"(addr));
    asm volatile("vcallmsr $vi27");
}
