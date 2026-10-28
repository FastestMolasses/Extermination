// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled with the 2.96 gas, which puts the register read into the branch
// delay slot as the original does.
//
// SDK DMA controller: returns the current D_STADR (stall address register,
// 0x1000E060) and, unless addr is 0xFFFFFFFF ("leave unchanged"), writes addr
// to it. The old value is read before the write.

unsigned int func_00101EE0(unsigned int addr)
{
    unsigned int old = *(volatile unsigned int *)0x1000E060;

    if (addr != 0xffffffff)
        *(volatile unsigned int *)0x1000E060 = addr;
    return old;
}
