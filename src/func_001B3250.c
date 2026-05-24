// All-word: everything as .word except jal/j-external
extern void func_001B2B80(int, int, int, int);

asm void func_001B3250(void) {
    .word 0x27bdffc0
    .word 0x7fbf0020
    .word 0x7fb00010
    .word 0xe7b40000
    .word 0x27a60030
    .word 0x46006506
    jal       func_001B2B80
    .word 0x70808628
    .word 0x30430001
    .word 0x54600004
    .word 0x30420004
    .word 0x10000016
    .word 0x24020002
    .word 0x30420004
    .word 0x10400009
    .word 0x00000000
    .word 0x3c017000
    .word 0x8c2231d0
    .word 0x8442001a
    .word 0x30422000
    .word 0x10400003
    .word 0x00000000
    .word 0x1000000b
    .word 0x24020001
    .word 0xc60100b4
    .word 0xc7a00030
    .word 0x46140841
    .word 0x46000834
    .word 0x00000000
    .word 0x45010004
    .word 0x70001628
    .word 0x10000002
    .word 0x24020002
    .word 0x70001628
    .word 0x7bbf0020
    .word 0x7bb00010
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0040
}
