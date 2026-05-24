// All-word: everything as .word except jal/j-external
extern void func_00128250(int, int, int, int);
extern void func_001C64F0(int, int, int, int);
extern void func_001C8480(int, int, int, int);
extern void func_001C8D50(int, int, int, int);

asm void func_001C67E0(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb00010
    .word 0xe7b50004
    .word 0xe7b40000
    .word 0x34a28000
    .word 0x70808628
    .word 0xa482002c
    .word 0x8c840040
    .word 0x8605002c
    .word 0x46006546
    jal       func_001C8480
    .word 0x46006d06
    .word 0x44800000
    .word 0x00000000
    .word 0x46150032
    .word 0x00000000
    .word 0x45000014
    .word 0x4600a306
    .word 0x3c023f80
    .word 0xae02003c
    jal       func_00128250
    .word 0x4600a306
    .word 0x8e050110
    .word 0x3c033f80
    .word 0x44836800
    .word 0x26040110
    .word 0xa4a2008e
    .word 0x9205000c
    jal       func_001C8D50
    .word 0x4600a306
    .word 0x3c023f80
    .word 0x44826000
    jal       func_001C64F0
    .word 0x72002628
    .word 0x1000000c
    .word 0x7bbf0020
    .word 0x4600a306
    jal       func_00128250
    .word 0xe615003c
    .word 0x8e030110
    .word 0x26040110
    .word 0x4600a306
    .word 0xa462008e
    .word 0x9205000c
    jal       func_001C8D50
    .word 0x4600ab46
    .word 0x7bbf0020
    .word 0x7bb00010
    .word 0xc7b50004
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0030
}
