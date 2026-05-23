// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001C9610(void);

asm void func_001C63D0(void) {
    lbu $a1, 0xC($a0)
    addiu $a2, $a0, 0xD0
    j func_001C9610
    addiu $a0, $a0, 0x110
}
