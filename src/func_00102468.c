// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.76% NEARMISS).
// Fixes read from the original instructions:
//  - Structural twin of the byte-matched func_00102530: the channel is a volatile
//    register block (CHCR at +0), there is no separate pre-check before the wait
//    loop, and the countdown is decremented before its test (`--count < 0`).
//  - CHCR is read as a bitfield record when clearing STR (through a temporary
//    copy in a loop the compiler folds to "never repeats"); the mode-1 poll and
//    the loop test read CHCR as a whole word and test bit 8 (STR).
//  - With that shape the old frame-size difference (0x50 vs 0x40) is gone; it
//    was not a compiler wall.
//
// SDK DMA helper: wait for a DMA channel to finish (CHCR.STR clear). Mode 1 only
// polls once and returns STR. Otherwise it spins while STR is set with a
// countdown (default 0x1000000 when `count` is 0); once the countdown runs out
// it prints a diagnostic (func_00122B58, printf-style, on the string at
// D_0026B220) each iteration and, if the channel is still running, clears STR.
// Always returns 0 on that path.

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
} DmaChan;

extern void func_00122B58(int *p);
extern int D_0026B220;

int func_00102468(volatile DmaChan *d, int mode, int count)
{
    ChcrBits chcr;

    if (mode == 1)
        return (*(volatile unsigned int *)&d->chcr >> 8) & 1;

    if (count == 0)
        count = 0x1000000;

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
    return 0;
}
