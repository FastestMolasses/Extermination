// Tail-call stub: jumps to another function. mwcc has no C-level
// tail-call optimization, so we hand-write the 2-instruction `j` as
// inline assembly inside an `asm void` function (no compiler-added
// prologue/epilogue).
extern void func_001F4BF0(void);

asm void func_001F4A00(void) {
    j func_001F4BF0
    nop
}
