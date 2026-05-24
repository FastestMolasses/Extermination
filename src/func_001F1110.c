// All-word: everything as .word except jal/j-external
extern void func_00122BB8(int, int, int, int);

asm void func_001F1110(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x249002d0
    jal       func_00122BB8
    .word 0x70a08e28
    .word 0x24030078
    .word 0x0043001a
    .word 0x3c034220
    .word 0x44830000
    .word 0x00001810
    .word 0x44830800
    .word 0x00000000
    .word 0x46800860
    .word 0x46010000
    .word 0xe6000004
    .word 0xae000000
    .word 0xa6110008
    .word 0xa600000a
    .word 0xae00000c
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
