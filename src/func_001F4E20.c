// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001F4D40(void);

asm void func_001F4E20(void) {
    lui $v0, (0x40B00000 >> 16)
    mtc1 $v0, $f13
    nop
    j func_001F4D40
    nop
}
