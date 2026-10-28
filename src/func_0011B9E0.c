// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 76.25% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - assembled by the 2.96 gas (// EEGCC_AS: 2.96), which puts the register
//    store in the return's delay slot (as for func_0011B910 / func_0011B5E0);
//  - the function returns the value it wrote: the original builds the value in
//    the return-value register and leaves it there. A void function built it in
//    the first argument register instead. Callers may ignore the result.

// SDK: sets VIF1_ERR (0x10003C20) = mii | me0 << 1 | me1 << 2 (the
// interrupt-bit, DMAtag-mismatch and VIFcode error masks) and returns that value.
int func_0011B9E0(int mii, int me0, int me1) {
    int v = mii | (me0 << 1) | (me1 << 2);
    *(volatile int *)0x10003C20 = v;
    return v;
}
