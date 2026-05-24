// All-word: everything as .word except jal/j-external
extern void func_001157F0(int, int, int, int);

asm void func_0011A770(void) {
    .word 0x24040047
    .word 0x0000282d
    .word 0x0000302d
    j         func_001157F0
    .word 0x0000382d
}
