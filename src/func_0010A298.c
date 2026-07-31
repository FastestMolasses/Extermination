// COMPILER: eegcc
// CFLAGS: -O2

/* SDK/libkernel DMA shutdown + reset path.
 *
 * Clears the "DMA busy" flag, marks the reset-in-progress flag, disables
 * interrupts, suspends the DMA controller (D_ENABLEW bit 16), stops the
 * three channels that can still be running, resumes the controller,
 * re-enables interrupts, clears those channels' D_MADR registers, kicks
 * the VIF0 FBRST reset bit, and tail-calls the low-level reset routine. */

extern void func_0010B0F8(int, int);

extern int D_0024153C;  /* DMA-busy / in-flight flag */
extern int D_00241324;  /* reset-in-progress flag    */

void func_0010A298(void)
{
    unsigned int stat;

    D_0024153C = 0;
    D_00241324 = 1;

    /* Disable interrupts and spin until COP0 Status.EIE (bit 16) is clear. */
    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(stat));
    } while (stat & 0x10000);

    /* D_ENABLEW = D_ENABLER | CPND  -- suspend all DMA transfers. */
    *(volatile unsigned int *)0x1000F590 = *(volatile unsigned int *)0x1000F520 | 0x10000;

    *(volatile unsigned int *)0x1000B000 = 0; /* D2_CHCR  */
    *(volatile unsigned int *)0x1000B400 = 0; /* D3_CHCR  */
    *(volatile unsigned int *)0x1000D400 = 0; /* D8_CHCR  */

    /* D_ENABLEW = D_ENABLER & ~CPND -- let DMA run again. */
    *(volatile unsigned int *)0x1000F590 = *(volatile unsigned int *)0x1000F520 & 0xFFFEFFFF;

    __asm__ __volatile__("ei");

    *(volatile unsigned int *)0x1000B020 = 0; /* D2_MADR */
    *(volatile unsigned int *)0x1000B420 = 0; /* D3_MADR */
    *(volatile unsigned int *)0x1000D420 = 0; /* D8_MADR */

    *(volatile unsigned int *)0x10002010 = 0x40000000; /* VIF0_FBRST */

    func_0010B0F8(0, 0);
}
