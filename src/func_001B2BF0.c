// All-word: everything as .word except jal/j-external
extern void func_0019A310(int, int, int, int);
extern void func_0019AD00(int, int, int, int);
extern void func_0019BC40(int, int, int, int);
extern void func_001B2E50(int, int, int, int);
extern void func_001B30E0(int, int, int, int);

asm void func_001B2BF0(void) {
    .word 0x27bdffa0
    .word 0x7fbf0040
    .word 0x7fb20030
    .word 0x7fb10020
    .word 0x7fb00010
    .word 0xe7b40000
    .word 0x3c023f80
    .word 0x70809628
    .word 0x70a08e28
    .word 0x70c08628
    .word 0x46006506
    .word 0xaca2000c
    jal       func_0019BC40
    .word 0x72202628
    .word 0x24060007
    .word 0x72402628
    jal       func_0019AD00
    .word 0x72202e28
    .word 0x10400025
    .word 0x72202628
    .word 0x30420002
    .word 0x10400007
    .word 0x00000000
    .word 0x3c017000
    .word 0x8c2231d4
    .word 0x14400003
    .word 0x00000000
    .word 0x1000001e
    .word 0x24020004
    .word 0x3c017000
    .word 0x8c2231d0
    .word 0x8452001a
    .word 0x32421800
    .word 0x10400004
    .word 0x27a4005c
    .word 0x10000016
    .word 0x24020004
    .word 0x27a4005c
    jal       func_0019A310
    .word 0x00000000
    .word 0x72202628
    jal       func_001B2E50
    .word 0x72002e28
    .word 0x32432000
    .word 0x10600003
    .word 0x00000000
    .word 0x1000000b
    .word 0x34420004
    .word 0xc7a0005c
    .word 0x46140036
    .word 0x00000000
    .word 0x45010003
    .word 0x00000000
    .word 0x10000004
    .word 0x34420004
    .word 0x72202628
    jal       func_001B30E0
    .word 0x72002e28
    .word 0x7bbf0040
    .word 0x7bb20030
    .word 0x7bb10020
    .word 0x7bb00010
    .word 0xc7b40000
    .word 0x03e00008
    .word 0x27bd0060
}
