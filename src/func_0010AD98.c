// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_0010AF28(void);

asm void func_0010AD98(void) {
    lw $a0, 0x40($a0)
    j func_0010AF28
    addiu $a0, $a0, 0x4C
}
