// All-word: everything as .word except jal/j-external
extern void func_001281C0(int, int, int, int);
extern void func_001C8F10(int, int, int, int);
extern void func_001C90D0(int, int, int, int);
extern void func_001C92C0(int, int, int, int);

asm void func_001C8710(void) {
    .word 0x27bdffb0
    .word 0x7fbf0040
    .word 0x7fb20030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0x70a09628
    .word 0xe7b40000
    .word 0x0012082a
    .word 0x46006506
    .word 0x10200018
    .word 0x70008628
    .word 0x70808e28
    jal       func_001281C0
    .word 0x4600a306
    .word 0x8e240000
    .word 0x70403628
    jal       func_001C8F10
    .word 0x72002e28
    jal       func_001281C0
    .word 0x4600a306
    .word 0x8e240000
    .word 0x70403628
    jal       func_001C90D0
    .word 0x72002e28
    jal       func_001281C0
    .word 0x4600a306
    .word 0x8e240000
    .word 0x72002e28
    jal       func_001C92C0
    .word 0x70403628
    .word 0x26100001
    .word 0x0212182a
    .word 0x1460ffeb
    .word 0x26310004
    .word 0x7bbf0040
    .word 0x7bb20030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0050
}
