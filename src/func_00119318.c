// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001190B8(void);

asm void func_00119318(void) {
    j func_001190B8
    daddu $a0, $zero, $zero
}
