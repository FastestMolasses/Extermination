// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 54.73% NEARMISS).
// Fixes read from the original instructions:
//  - Same shape as the byte-matched sibling func_00102368 (identical code apart from the
//    MOD value): the channel is a volatile register block (CHCR at +0, MADR +0x10,
//    QWC +0x20), the countdown starts at 0x1000000 and is decremented before its test
//    (`--count < 0`), and clearing STR goes through a temporary bitfield copy in a loop
//    the compiler folds to "never repeats" (the "dead one-iteration inner loop" the old
//    note called a compiler wall).
//  - MADR is replaced only when it does NOT read as -1 (the old C had the test inverted);
//    the final CHCR update is a bitfield record read before QWC is written and stored
//    back once, fields set in the original's order MOD = 0, DIR = 0, STR = 1.
//  - The function returns nothing (the old C returned CHCR).
//
// SDK DMA helper (sceDmaSend-like normal-mode start): wait for a DMA channel to stop
// (CHCR.STR clear), with a countdown of 0x1000000 polls; once it runs out, each further
// poll prints a diagnostic (func_00122B58, printf, on the string at D_0026B220) and, if
// the channel is still running, clears STR. Then sets MADR to `addr` unless MADR reads
// as -1, sets QWC to `size`, and starts the channel with MOD = 0 (normal), DIR = 0 and
// STR = 1.

typedef struct {
    unsigned DIR : 1;
    unsigned p0 : 1;
    unsigned MOD : 2;
    unsigned ASP : 2;
    unsigned TTE : 1;
    unsigned TIE : 1;
    unsigned STR : 1;
    unsigned p1 : 7;
    unsigned TAG : 16;
} ChcrBits;

typedef struct {
    ChcrBits chcr;
    unsigned int p0[3];
    void *madr;
    unsigned int p1[3];
    unsigned int qwc;
} DmaChan;

extern void func_00122B58(int *p);
extern int D_0026B220;

void func_00102278(volatile DmaChan *d, void *addr, int size)
{
    int count = 0x1000000;
    ChcrBits chcr;

    while (*(volatile unsigned int *)&d->chcr & 0x100) {
        if (--count < 0) {
            func_00122B58(&D_0026B220);
            chcr = d->chcr;
            if (chcr.STR) {
                ChcrBits t;
                do {
                    t = chcr;
                    t.STR = 0;
                    chcr = t;
                } while (chcr.STR);
                d->chcr = t;
            }
        }
    }

    if (d->madr != (void *)-1)
        d->madr = addr;
    {
        ChcrBits c = d->chcr;
        d->qwc = size;
        c.MOD = 0;
        c.DIR = 0;
        c.STR = 1;
        d->chcr = c;
    }
}
