// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 68.00% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the C was already right; the original was assembled by the 2.96 gas, which
//    fills the return's delay slot with the store (// EEGCC_AS: 2.96), as for
//    func_0011B910.

// SDK: resets the GIF (GIF_CTRL, 0x10003000, = 1).
void func_0011B328(void) {
    *(volatile int *)0x10003000 = 1;
}
