// All-word: everything as .word except jal/j-external
extern void func_001157F0(int, int, int, int);
extern void func_001157F0(int, int, int, int);

asm void func_00119320(void) {
    .word 0x27bdffe0
    .word 0xffb00000
    .word 0xffbf0010
    .word 0x0000802d
    .word 0x3c0600ff
    .word 0x00000000
    .word 0x0200282d
    .word 0x2404000b
    .word 0x34c6ffff
    jal       func_001157F0
    .word 0x0000382d
    .word 0x0200282d
    .word 0x24040028
    .word 0x0000302d
    jal       func_001157F0
    .word 0x0000382d
    .word 0x26100001
    .word 0x2a020002
    .word 0x1440fff3
    .word 0x3c0600ff
    .word 0xdfbf0010
    .word 0x2404001f
    .word 0xdfb00000
    .word 0x0000282d
    .word 0x0000302d
    .word 0x0000382d
    j         func_001157F0
    .word 0x27bd0020
}
