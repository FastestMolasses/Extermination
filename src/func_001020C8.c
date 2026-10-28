// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 62.98% NEARMISS).
// Fixes read from the original instructions:
//  - Same shape as the byte-matched siblings func_001021B0 / func_00102530: the
//    channel is a volatile register block (CHCR at +0, MADR at +0x10, QWC at +0x20),
//    the countdown starts at 0x1000000 and is decremented before its test, and clearing
//    STR goes through a temporary bitfield copy in a loop the compiler folds to "never
//    repeats" (the one-iteration inner loop the old C could not produce).
//  - The MADR test compares against (void *)-1; the final CHCR update is a bitfield
//    record read before QWC is written, fields set MOD, DIR, STR, then written back once
//    (the old C had the right mask/OR values but a plain word update).
//
// SDK DMA helper (interleave send, sceDmaSendI-shaped): wait for the channel to stop
// (CHCR.STR clear) with a countdown of 0x1000000 polls; once it runs out each further
// poll prints a diagnostic (func_00122B58, printf, on the string at D_0026B220) and,
// if the channel is still running, clears STR. Then, unless MADR reads as -1, sets
// MADR to `addr`; writes QWC = `qwc`; and starts the channel with MOD = 2
// (interleave), DIR = 1 (from memory) and STR = 1.

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

void func_001020C8(volatile DmaChan *d, void *addr, int qwc)
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
        d->qwc = qwc;
        c.MOD = 2;
        c.DIR = 1;
        c.STR = 1;
        d->chcr = c;
    }
}
