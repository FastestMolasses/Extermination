// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.89% NEARMISS).
// Fixes read from the original instructions:
//  - The spin counter is tested with a post-increment (`count++ >= 5001`): the
//    original compares the counter's old value (a copy) and increments in the
//    branch delay slot; the earlier if/else-increment spelling compared the
//    counter directly, which looked like a "redundant copy" compiler wall.
//
// Polls the 64-bit hardware register at 0x10002000 until it reads non-negative
// or bit 0x4000 of 0x10002010 is set; every 5001 polls it calls func_00109B70
// with D_002412F4 and restarts the count. Returns the last value read.

extern int func_00109B70(int);
extern int D_002412F4;

long func_00106490(void) {
    long val;
    int count = 0;

    val = *(volatile long *)0x10002000;
    if (val < 0) {
        if ((*(volatile int *)0x10002010 & 0x4000) == 0) {
            do {
                if (count++ >= 0x1389) {
                    func_00109B70(D_002412F4);
                    count = 0;
                }
                val = *(volatile long *)0x10002000;
                if (val >= 0)
                    break;
            } while ((*(volatile int *)0x10002010 & 0x4000) == 0);
        }
    }
    return val;
}
