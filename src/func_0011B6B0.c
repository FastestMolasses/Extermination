// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled by the 2.96 gas (// EEGCC_AS: 2.96), which fills the delay slot as
// the original does.
//
// SDK VIF0: sets VIF0_ERR (0x10003820) = mii | me0 << 1 | me1 << 2 (the
// interrupt-bit, DMAtag-mismatch and VIFcode error masks) and returns that value
// (the VIF0 twin of func_0011B9E0).

int func_0011B6B0(int mii, int me0, int me1) {
    int v = mii | (me0 << 1) | (me1 << 2);
    *(volatile int *)0x10003820 = v;
    return v;
}
