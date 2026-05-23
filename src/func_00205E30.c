// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_00205A50(void);

asm void func_00205E30(void) {
    dsll32 $a2, $a1, 0
    addiu $a1, $zero, 0x1A
    j func_00205A50
    dsrl32 $a2, $a2, 0
}
