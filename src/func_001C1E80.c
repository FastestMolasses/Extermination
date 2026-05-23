// Tail-call stub: jumps to another function. mwcc has no C-level
// tail-call optimization, so we hand-write the 2-instruction `j` as
// inline assembly inside an `asm void` function (no compiler-added
// prologue/epilogue).
extern void func_001D8FD0(void);

asm void func_001C1E80(void) {
    j func_001D8FD0
    nop
}
