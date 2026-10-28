// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK: reads up to n quadwords from the VIF1 FIFO (0x10005000) into data while
// VIF1_STAT's FIFO count field (bits 24-28) is non-zero; returns the number read.

typedef unsigned int u128 __attribute__((mode(TI)));
#define VIF1_STAT  (*(volatile unsigned int *)0x10003C00)
#define VIF1_FIFO  (*(volatile u128 *)0x10005000)

int func_0011BC98(u128 *data, int n)
{
    int i = 0;

    while (n--) {
        if ((VIF1_STAT & 0x1F000000) == 0)
            return i;
        *data++ = VIF1_FIFO;
        i++;
    }
    return i;
}
