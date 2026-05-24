// All-word: everything as .word except jal/j-external
extern void func_001028B8(int, int, int, int);
extern void func_0015AC00(int, int, int, int);
extern void func_0015AE20(int, int, int, int);
extern void func_001AFC10(int, int, int, int);
extern void func_001B1190(int, int, int, int);
extern void func_001C6380(int, int, int, int);

asm void func_0015B030(void) {
    .word 0x27bdffc0
    .word 0x7fbf0030
    .word 0x7fb20020
    .word 0x7fb10010
    .word 0x7fb00000
    .word 0x90860004
    .word 0x8c910020
    .word 0x70809628
    .word 0x24050003
    .word 0x10c50029
    .word 0x265001f0
    .word 0x24030002
    .word 0x10c30026
    .word 0x00000000
    .word 0x24030001
    .word 0x10c30010
    .word 0x00000000
    .word 0x10c00003
    .word 0x00000000
    .word 0x10000020
    .word 0x9244009a
    jal       func_0015AC00
    .word 0x00000000
    .word 0x14400020
    .word 0x00000000
    .word 0x92230004
    .word 0x28630002
    .word 0x1460001c
    .word 0x00000000
    .word 0x24030003
    .word 0x10000019
    .word 0xa2430004
    .word 0x92230004
    .word 0x28630002
    .word 0x54600004
    .word 0x262500b0
    .word 0x10000013
    .word 0xa2450004
    .word 0x262500b0
    .word 0x264400b0
    jal       func_001028B8
    .word 0x264600a0
    .word 0x3c023f80
    .word 0xae4200bc
    jal       func_001C6380
    .word 0x72402628
    .word 0x72402628
    jal       func_0015AE20
    .word 0x72002e28
    .word 0x10000007
    .word 0x7bbf0030
    .word 0x9244009a
    jal       func_001B1190
    .word 0x00000000
    jal       func_001AFC10
    .word 0x72402628
    .word 0x7bbf0030
    .word 0x7bb20020
    .word 0x7bb10010
    .word 0x7bb00000
    .word 0x03e00008
    .word 0x27bd0040
}
