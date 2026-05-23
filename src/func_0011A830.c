// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001157F0(void);

asm void func_0011A830(void) {
    daddu $a1, $a0, $zero
    daddu $a2, $zero, $zero
    addiu $a0, $zero, 0x4E
    j func_001157F0
    daddu $a3, $zero, $zero
}
