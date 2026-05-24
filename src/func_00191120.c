// All-word: everything as .word except jal/j-external
extern void func_0011DF78(int, int, int, int);
extern void func_001B1470(int, int, int, int);

asm void func_00191120(void) {
    .word 0x27bdffd0
    .word 0x7fbf0020
    .word 0xe7b80010
    .word 0x46006606
    .word 0xe7b7000c
    .word 0xe7b60008
    .word 0xe7b50004
    .word 0x460d6301
    .word 0xe7b40000
    .word 0x46007586
    .word 0x46007d46
    jal       func_001B1470
    .word 0x46006dc6
    .word 0x46000506
    jal       func_0011DF78
    .word 0x4600a306
    .word 0x46150034
    .word 0x00000000
    .word 0x45000005
    .word 0x00000000
    jal       func_001B1470
    .word 0x4600bb06
    .word 0x1000001d
    .word 0x7bbf0020
    .word 0x44800000
    .word 0x00000000
    .word 0x4600a036
    .word 0x00000000
    .word 0x4501000b
    .word 0x4600a007
    .word 0x4616a036
    .word 0x00000000
    .word 0x45010004
    .word 0x4600c006
    .word 0x1000000e
    .word 0x4616bdc0
    .word 0x4600c006
    .word 0x1000000d
    .word 0x00000000
    .word 0x4600a007
    .word 0x46160036
    .word 0x00000000
    .word 0x45010004
    .word 0x4600c006
    .word 0x10000004
    .word 0x4616bdc1
    .word 0x4600c006
    .word 0x10000003
    .word 0x00000000
    jal       func_001B1470
    .word 0x4600bb06
    .word 0x7bbf0020
    .word 0xc7b80010
    .word 0xc7b7000c
    .word 0xc7b60008
    .word 0xc7b50004
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0030
}
