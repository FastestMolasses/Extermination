// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_00205A50(void);

asm void func_00205C60(void) {
    dsll32 $v0, $a2, 0
    dsll32 $v1, $a1, 0
    dsrl32 $v0, $v0, 0
    dsrl32 $v1, $v1, 0
    dsll $v0, $v0, 16
    addiu $a1, $zero, 0x3
    j func_00205A50
    or $a2, $v1, $v0
}
