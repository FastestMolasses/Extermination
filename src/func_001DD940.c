// Tail-call stub: jumps to another function. mwcc has no C-level
// tail-call optimization, so we hand-write the 2-instruction `j` as
// inline assembly inside an `asm void` function (no compiler-added
// prologue/epilogue).
extern void func_001DD7B0(void);

asm void func_001DD940(void) {
    j func_001DD7B0
    nop
}
