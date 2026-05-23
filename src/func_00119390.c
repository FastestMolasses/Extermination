// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001157F0(void);

asm void func_00119390(void) {
    daddu $a2, $a0, $zero
    addiu $a1, $zero, 0xA
    addiu $a0, $zero, 0x33
    j func_001157F0
    daddu $a3, $zero, $zero
}
