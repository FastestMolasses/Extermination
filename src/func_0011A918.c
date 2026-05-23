// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001157F0(void);

asm void func_0011A918(void) {
    daddu $a3, $a2, $zero
    dsll32 $a0, $a0, 0
    dsra32 $a0, $a0, 0
    daddu $a2, $a1, $zero
    daddu $a1, $a0, $zero
    j func_001157F0
    addiu $a0, $zero, 0x4A
}
