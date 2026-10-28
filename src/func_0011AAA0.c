// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// VU0 D-bit enable: sets bit 2 of FBRST (COP2 control register vi28) when
// on == 1, clears it otherwise. COP2 moves are user-approved VU0 inline asm.

void func_0011AAA0(int on) {
    unsigned int fbrst;
    asm volatile("cfc2.ni %0, $vi28" : "=r"(fbrst));
    if (on == 1) {
        fbrst |= 4;
    } else {
        fbrst &= ~4;
    }
    asm volatile("ctc2.ni %0, $vi28" : : "r"(fbrst));
}
