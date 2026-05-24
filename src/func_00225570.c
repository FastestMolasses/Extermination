// All-word: everything as .word except jal/j-external
extern void func_0021D2E0(int, int, int, int);

asm void func_00225570(void) {
    .word 0x27bdfff0
    .word 0x7fbf0000
    .word 0x90850006
    .word 0x24030001
    .word 0x50a30009
    .word 0x24050078
    .word 0x50a00004
    .word 0x24a20001
    .word 0x10000008
    .word 0x7bbf0000
    .word 0x24a20001
    .word 0xa0820006
    .word 0xa0800007
    .word 0x24050078
    jal       func_0021D2E0
    .word 0x70003628
    .word 0x7bbf0000
    .word 0x03e00008
    .word 0x27bd0010
}
