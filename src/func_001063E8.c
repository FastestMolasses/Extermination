// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 78.83% NEARMISS).
// Fixes read from the original instructions:
// - The hardware register 0x10002010 is read through a constant-address
//   expression at each use (no pointer local), so ee-gcc hoists its address into
//   a saved register for the poll loop instead of rematerialising it.
// - The guard compares the masked status against a local holding 0x80000000 and
//   returns that same local, so the failing guard returns the compare register.
// - The watchdog counter is post-incremented in the test (`i++ > 5000`): the
//   compare reads the old value and the increment runs on both paths before the
//   reset to 0 after the func_00109B70 call (the old if/else form had the same
//   behaviour).
//
// Waits while the status at 0x10002010 masked with 0x80004000 reads 0x80000000
// (returns 0x80000000 at once if it does not on entry). When the count passes 5000 it calls
// func_00109B70(D_002412F4) and restarts the count. Returns the poll count.

extern int func_00109B70(int);
extern int D_002412F4;

#define REG (*(volatile int *)0x10002010)

int func_001063E8(void) {
    int ret;
    int i;

    i = 0;
    ret = 0x80000000;
    if ((REG & 0x80004000) != ret) {
        return ret;
    }
    do {
        if (i++ > 5000) {
            func_00109B70(D_002412F4);
            i = 0;
        }
    } while ((REG & 0x80004000) == 0x80000000);
    return i;
}
