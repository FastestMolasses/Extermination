// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK GIF: returns GIF_STAT (0x10003020) bit 2 as 0/1.

int func_0011B3F8(void) {
    return (*(volatile unsigned int *)0x10003020 & 4) >> 2;
}
