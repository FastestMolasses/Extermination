// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 82.22% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the C was already right; the original was assembled by the 2.96 gas, which
//    fills the return's delay slot with the last store (// EEGCC_AS: 2.96).
//
// SDK: resets VIF0 (VIF0_FBRST, 0x10003810, = 1) and sets VIF0_ERR
// (0x10003820) = 6 (mask the DMAtag-mismatch and VIFcode errors).
void func_0011B5E0(void) {
    *(volatile int *)0x10003810 = 1;
    *(volatile int *)0x10003820 = 6;
}
