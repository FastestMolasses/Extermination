// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 66.39% NEARMISS).
// Fixes read from the original instructions:
//  - Structural twin of the byte-matched func_001021B0: the channel is a volatile
//    DmaChan register block (CHCR at +0, QWC +0x20, TADR +0x30), the wait loop has
//    no separate pre-check, the countdown is `int count = 0x1000000;` tested as
//    `--count < 0`, and STR is cleared through the temporary-copy do/while on the
//    ChcrBits bitfield (the folded "never repeats" loop in the original).
//  - The final CHCR write is a bitfield record update (MOD = 1, DIR = 1, STR = 1,
//    in that order) after QWC is zeroed, not a whole-word mask-and-or.
//  - The old "degenerate dead-backedge wall" was the missing bitfield shape.
//
// SDK DMA helper (libdma chain-mode start, sceDmaSend-shaped): wait for the
// channel's CHCR.STR to clear with a 0x1000000 countdown; once it runs out it prints
// a diagnostic (func_00122B58, printf-style, on the string at D_0026B220) each
// iteration and, if the channel is still running, clears STR. Then, unless the
// channel's TADR currently holds -1, stores `tag` into TADR; zeroes QWC and starts
// the channel in source-chain mode (DIR = 1, MOD = 1, STR = 1).

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
    unsigned int p2[3];
    void *tadr;
} DmaChan;

extern void func_00122B58(int *p);
extern int D_0026B220;

void func_00101F08(volatile DmaChan *d, void *tag)
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
    if (d->tadr != (void *)-1)
        d->tadr = tag;
    {
        ChcrBits c = d->chcr;
        d->qwc = 0;
        c.MOD = 1;
        c.DIR = 1;
        c.STR = 1;
        d->chcr = c;
    }
}
