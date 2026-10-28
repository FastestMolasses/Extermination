// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 73.15% NEARMISS).
// Fixes read from the original instructions:
//  - The channel is a volatile register block (CHCR at +0, MADR at +0x10): every
//    MADR test reloads, and there is no separate pre-check before the wait loop.
//  - The countdown is decremented before its test (`--count < 0`) instead of being
//    pre-decremented and decremented at the loop bottom.
//  - CHCR is read as a bitfield record; clearing STR goes through a temporary copy
//    in a loop whose re-test the compiler folds to "never repeats", and the
//    temporary is what is written back.
//
// SDK DMA helper: wait for a DMA channel's MADR to reach `addr`. Mode 1 only
// polls once and returns whether MADR is still below `addr`. Otherwise it spins
// while MADR < addr with a countdown (default 0x1000000 when `count` is 0); once
// the countdown runs out it prints a diagnostic (func_00122B58, printf-style, on
// the string at D_0026B220) each iteration and, if the channel is still
// running (CHCR.STR), clears STR. Always returns 0 on that path.

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

int func_00102530(volatile DmaChan *d, void *addr, int mode, int count)
{
    ChcrBits chcr;

    if (mode == 1)
        return d->madr < addr;

    if (count == 0)
        count = 0x1000000;

    while (d->madr < addr) {
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
