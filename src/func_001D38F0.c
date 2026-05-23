// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001D38A0(void);

asm void func_001D38F0(void) {
    paddub $a1, $a0, $zero
    j func_001D38A0
    paddub $a0, $zero, $zero
}
