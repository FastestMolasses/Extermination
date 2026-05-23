// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_0010C020(void);

asm void func_0010C140(void) {
    lui $v0, (0xFFFFFFC0 >> 16)
    ori $v0, $v0, (0xFFFFFFC0 & 0xFFFF)
    and $a1, $a1, $v0
    j func_0010C020
    and $a0, $a0, $v0
}
