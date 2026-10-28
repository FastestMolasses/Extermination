// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.93% NEARMISS).
// Fixes read from the original instructions:
// - the second table's base entry (0x70003224, the last entry of the first
//   table) is read once into a local before its loop; the C re-read it on
//   every iteration.
// - the 0x18 header field is summed into the reused header-pointer local
//   before the stride is computed, then stored to 0x70003210 (the original
//   keeps the sum in the header pointer's register).
// - all three loops share one counter; the second table's loop has its own
//   cursor and its own stride local, and the stride local is declared before
//   the base-entry local (saved/temporary register order follows declaration
//   order).
//
// EE-scratchpad (0x70003xxx) initializer for the level's slot tables: loads the
// data base pointer D_0028A598 once and stores it to 0x700031F8, then reads the
// header back from that slot and stores base-relative pointers for header
// fields +0x0/+0x8/+0x10/+0x20 to 0x700031FC..0x70003208, the per-slot count
// (+0x24, a short) to 0x7000320C, and the +0x18 table start to 0x70003210.
// It then fills the five following entries (0x70003214..0x70003224) with
// pointers that each advance by count * 2 bytes. If header word +0x1C is 12,
// a second six-entry table at 0x70003228 continues the sequence from the last
// entry; otherwise that table is cleared. Finally it stores D_0028A5A8 to
// 0x70003250 and copies the short it points to into 0x7000324C.

extern char *D_0028A598;
extern short *D_0028A5A8;
extern int D_70003214[];
extern int D_70003228[];

void func_00199C50(void) {
    char *dbase;
    char *base;
    int i;
    int *p;
    int *q;
    int delta;
    int step2;
    int last;
    int step;

    dbase = D_0028A598;
    *(char *volatile *)0x700031F8 = dbase;
    base = *(char *volatile *)0x700031F8;
    *(char *volatile *)0x700031FC = dbase + *(int *)(base + 0x0);
    *(char *volatile *)0x70003200 = dbase + *(int *)(base + 0x8);
    *(char *volatile *)0x70003204 = dbase + *(int *)(base + 0x10);
    *(char *volatile *)0x70003208 = dbase + *(int *)(base + 0x20);
    *(volatile int *)0x7000320C = *(short *)(base + 0x24);
    base = dbase + *(int *)(base + 0x18);
    step = *(int *)0x7000320C * 2;
    *(char *volatile *)0x70003210 = base;

    p = D_70003214;
    i = 1;
    do {
        i += 1;
        p[0] = p[-1] + step;
        p += 1;
    } while (i < 6);

    if (*(int *)(*(char *volatile *)0x700031F8 + 0x1C) == 0xC) {
        q = D_70003228;
        i = 0;
        step2 = *(int *)0x7000320C * 2;
        last = *(int *)0x70003224;
        do {
            delta = (i + 1) * step2;
            i += 1;
            *q = last + delta;
            q += 1;
        } while (i < 6);
    } else {
        p = D_70003228;
        i = 0;
        do {
            i += 1;
            *p = 0;
            p += 1;
        } while (i < 6);
    }

    *(short *volatile *)0x70003250 = D_0028A5A8;
    *(volatile short *)0x7000324C = **(short *volatile *)0x70003250;
}
