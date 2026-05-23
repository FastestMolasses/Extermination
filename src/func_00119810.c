// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001157F0(void);

asm void func_00119810(void) {
    daddu $a2, $a1, $zero
    daddu $a3, $zero, $zero
    daddu $a1, $a0, $zero
    j func_001157F0
    addiu $a0, $zero, 0x15
}
