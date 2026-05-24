// All-word: everything as .word except jal/j-external
extern void func_001157F0(int, int, int, int);

asm void func_0011A4B8(void) {
    .word 0x2404003c
    .word 0x0000282d
    .word 0x0000302d
    j         func_001157F0
    .word 0x0000382d
}
