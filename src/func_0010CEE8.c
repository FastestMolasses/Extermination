// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// SDK serial-port console output (EE SIO putc): spins while the SIO status
// register 0x1000F130 reports the transmit FIFO full (bit 15), then writes the
// byte to the transmit FIFO register 0x1000F180 and returns the character.
// The 2.96 assembler fills the return's delay slot with the FIFO store.

int func_0010CEE8(int c) {
    while (*(volatile unsigned int *)0x1000F130 & 0x8000) {
    }
    *(volatile unsigned char *)0x1000F180 = c;
    return c;
}
