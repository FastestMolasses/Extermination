// Tail-call wrapper: sets up arguments and tail-jumps to another
// function. mwcc lacks C-level tail-call optimization, so we write
// these wrappers as `asm void` to control the final `j`.
extern void func_001CABA0(void);

asm void func_001CACB0(void) {
    j func_001CABA0
    lw $a1, 0x44($a0)
}
