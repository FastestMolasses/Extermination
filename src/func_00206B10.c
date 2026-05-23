// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_00204490(void);

asm void func_00206B10(void) {
    j func_00204490
    addiu $a0, $a0, 0x48
}
