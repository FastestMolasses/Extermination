// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled by the 2.96 gas (// EEGCC_AS: 2.96), which fills the delay slot as
// the original does.
//
// SDK VIF0: pushes up to n quadwords from src into the VIF0 FIFO (0x10004000),
// stopping early when VIF0_STAT (0x10003800) bit 28 is set; returns the number of
// quadwords written (the VIF0 twin of func_0011B580).

typedef unsigned int u128 __attribute__((mode(TI)));
int func_0011B8B0(u128 *src, int n) {
    int i = 0;
    while (n-- != 0) {
        if (*(volatile unsigned int *)0x10003800 & 0x10000000) {
            return i;
        }
        *(volatile u128 *)0x10004000 = *src++;
        i++;
    }
    return i;
}
