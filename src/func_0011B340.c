// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled by the 2.96 gas (// EEGCC_AS: 2.96), which fills the delay slot as
// the original does.
//
// SDK GIF: GIF_CTRL (0x10003000) = 8 (PSE: pause the transfer); returns 1.

int func_0011B340(void) {
    *(volatile int *)0x10003000 = 8;
    return 1;
}
