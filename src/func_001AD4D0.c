// Tail-call stub: jumps to another function. mwcc has no C-level
// tail-call optimization, so we hand-write the 2-instruction `j` as
// inline assembly inside an `asm void` function (no compiler-added
// prologue/epilogue).
extern void anim_frame_top_b(void);

asm void func_001AD4D0(void) {
    j anim_frame_top_b
    nop
}
