// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.31% NEARMISS).
// Fixes read from the original instructions:
//  - the large-transfer branch passes dev->0xD8 (not D_002412F4) to func_00108300;
//    both calls take the same first argument (loaded once in the branch delay slot).
//  - func_00109B20 returns int (as its byte-matched callers in src/ declare it);
//    the void prototype changed register allocation after the first call.
//  - the masked DMA source address is computed into its own local before the
//    channel-register writes.
//
// DMA to IPU: bracketed by func_00109B20(D_002412F4, &buf) with buf[0] = 2, then 3.
// Clears an IPU error (IPU_CTRL 0x10002010 bit 0x4000 -> write reset 0x40000000),
// waits on the IPU_CTRL busy (sign) bit, calls func_001063B8(0), waits again, then
// programs DMA channel 4 (MADR 0x1000B410, QWC 0x1000B420 = n * 24, CHCR 0x1000B400
// = 0x101) and dispatches func_00108160 for n < 0x400, else func_00108300.

extern int D_002412F4;

extern int func_00109B20(int, int *);
extern void func_001063B8(int);
extern void func_00108160(int, int);
extern void func_00108300(int, int);

void func_001084B0(int *a0) {
    int buf[8];
    int n;
    int small;
    int dev;
    unsigned int madr;

    dev = *(int *)(D_002412F4 + 0x40);
    n = a0[3] * a0[4];
    buf[0] = 2;
    func_00109B20(D_002412F4, buf);

    if (*(volatile unsigned int *)0x10002010 & 0x4000) {
        *(volatile unsigned int *)0x10002010 = 0x40000000;
    }

    small = n < 0x400;

    while (*(volatile int *)0x10002010 < 0)
        ;
    func_001063B8(0);

    while (*(volatile int *)0x10002010 < 0)
        ;

    madr = (unsigned int)a0[0] & 0x0FFFFFFF;
    *(volatile unsigned int *)0x1000B410 = madr;
    *(volatile unsigned int *)0x1000B420 = n * 0x18;
    *(volatile unsigned int *)0x1000B400 = 0x101;

    if (small) {
        func_00108160(*(int *)(dev + 0xD8), n);
    } else {
        func_00108300(*(int *)(dev + 0xD8), n);
    }

    buf[0] = 3;
    func_00109B20(D_002412F4, buf);
}
