// Tail-call stub: jumps to another function. mwcc has no C-level
// tail-call optimization, so we hand-write the 2-instruction `j` as
// inline assembly inside an `asm void` function (no compiler-added
// prologue/epilogue).
extern void func_001D0720(void);

asm void func_001D0C70(void) {
    j func_001D0720
    nop
}
