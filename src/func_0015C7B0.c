// Tail-call stub: jumps to another function. mwcc has no C-level
// tail-call optimization, so we hand-write the 2-instruction `j` as
// inline assembly inside an `asm void` function (no compiler-added
// prologue/epilogue).
extern void func_0015C1F0(void);

asm void func_0015C7B0(void) {
    j func_0015C1F0
    nop
}
