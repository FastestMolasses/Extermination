// All-word: everything as .word except jal/j-external
extern void func_0011DE90(int, int, int, int);
extern void func_0011E2A8(int, int, int, int);
extern void func_001916C0(int, int, int, int);
extern void func_001B12B0(int, int, int, int);

asm void func_00193D90(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x24060001
    .word 0x70808628
    jal       func_001916C0
    .word 0x70a08e28
    .word 0xc60d0044
    .word 0x3c023b64
    .word 0x3442c389
    .word 0x44827000
    jal       func_001B12B0
    .word 0xc60c0048
    .word 0xe6000044
    jal       func_0011E2A8
    .word 0x46000306
    .word 0xc602004c
    .word 0xc6010020
    .word 0x46001002
    .word 0x46000801
    .word 0xe6000010
    jal       func_0011DE90
    .word 0xc60c0044
    .word 0xc602004c
    .word 0xc6010028
    .word 0x46001002
    .word 0x46000801
    .word 0xe6000018
    .word 0xc6010044
    .word 0xc6000048
    .word 0x46000832
    .word 0x00000000
    .word 0x45000004
    .word 0x00000000
    .word 0x24030001
    .word 0xa2030001
    .word 0xa2000003
    .word 0x8e250230
    .word 0x24040001
    .word 0x10a40006
    .word 0x00000000
    .word 0x24030002
    .word 0x10a30003
    .word 0x00000000
    .word 0xa2040001
    .word 0xa2000003
    .word 0x92030003
    .word 0x14600009
    .word 0x00000000
    .word 0x92030007
    .word 0x3063000d
    .word 0x1060000c
    .word 0x00000000
    .word 0x24030001
    .word 0xa2030001
    .word 0x10000008
    .word 0xa2000003
    .word 0x92030007
    .word 0x3063000b
    .word 0x10600004
    .word 0x00000000
    .word 0x24030001
    .word 0xa2030001
    .word 0xa2000003
    .word 0x7bbf0020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0030
}
