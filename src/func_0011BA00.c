// COMPILER: eegcc
// CFLAGS: -O2
// EEGCC_AS: 2.96
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Assembled by the 2.96 gas (// EEGCC_AS: 2.96), which fills the delay slot as
// the original does.
//
// SDK VIF1: returns VIF1_ERR (0x10003C20).

int func_0011BA00(void) {
    return *(volatile int *)0x10003C20;
}
