// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 69.98% NEARMISS).
// Fixes read from the original instructions:
//  - Same shape as the byte-matched sibling func_00102530: the channel is a volatile
//    register block (CHCR at +0, QWC at +0x20), the countdown starts at 0x1000000 and is
//    decremented before its test (`--count < 0`), and clearing STR goes through a
//    temporary bitfield copy in a loop the compiler folds to "never repeats" (this is
//    the inner spin loop the old C could not produce).
//  - The STR wait test is a whole-word CHCR read; the final CHCR update is a bitfield
//    record read before QWC is cleared, then written back once.
//
// SDK DMA helper: wait for a DMA channel to stop (CHCR.STR clear), with a countdown of
// 0x1000000 polls; once it runs out, each further poll prints a diagnostic
// (func_00122B58, printf, on the string at D_0026B220) and, if the channel is still
// running, clears STR. Then clears QWC and restarts the channel with MOD = 1 (chain),
// DIR = 0 and STR = 1.

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

void func_001021B0(volatile DmaChan *d)
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

    {
        ChcrBits c = d->chcr;
        d->qwc = 0;
        c.MOD = 1;
        c.DIR = 0;
        c.STR = 1;
        d->chcr = c;
    }
}
