// All-word: everything as .word except jal/j-external
extern void func_001B1470(int, int, int, int);

asm void func_001B12B0(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0xe7b60008
    .word 0x46006586
    .word 0x460d6301
    .word 0xe7b50004
    .word 0xe7b40000
    .word 0x46007506
    jal       func_001B1470
    .word 0x46006d46
    .word 0x44800800
    .word 0x00000000
    .word 0x46000832
    .word 0x00000000
    .word 0x45000005
    .word 0x00000000
    jal       func_001B1470
    .word 0x4600ab06
    .word 0x1000001b
    .word 0x7bbf0010
    .word 0x46010036
    .word 0x00000000
    .word 0x4503000b
    .word 0x46000007
    .word 0x46140036
    .word 0x00000000
    .word 0x45010004
    .word 0x4600b006
    .word 0x1000000e
    .word 0x4614ad40
    .word 0x4600b006
    .word 0x1000000d
    .word 0x00000000
    .word 0x46000007
    .word 0x46140036
    .word 0x00000000
    .word 0x45010004
    .word 0x4600b006
    .word 0x10000004
    .word 0x4614ad41
    .word 0x4600b006
    .word 0x10000003
    .word 0x00000000
    jal       func_001B1470
    .word 0x4600ab06
    .word 0x7bbf0010
    .word 0xc7b60008
    .word 0xc7b50004
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0020
}
