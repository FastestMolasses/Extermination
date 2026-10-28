// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK: pushes up to n quadwords from data into the VIF1 FIFO (0x10005000),
// stopping early when VIF1_STAT bit 28 (FIFO full) is set; returns the number
// written. The FIFO store is a plain (non-volatile) 128-bit store: that is what
// lets the compiler put it in the loop branch's delay slot as the original does.

typedef unsigned int u128 __attribute__((mode(TI)));
#define VIF1_STAT  (*(volatile unsigned int *)0x10003C00)
#define VIF1_FIFO  (*(u128 *)0x10005000)

int func_0011BC38(u128 *data, int n)
{
    int i = 0;

    while (n--) {
        if (VIF1_STAT & 0x10000000)
            return i;
        VIF1_FIFO = *data++;
        i++;
    }
    return i;
}
