// All-word: everything as .word except jal/j-external
extern void func_001278C0(int, int, int, int);

asm void func_00128250(void) {
    .word 0x27bdffd0
    .word 0xffbf0020
    .word 0x27a40010
    .word 0xe7ac0010
    jal       func_001278C0
    .word 0x03a0282d
    .word 0x8fa30000
    .word 0x38620002
    .word 0x10400003
    .word 0x2c620002
    .word 0x10400003
    .word 0x8fa20004
    .word 0x10000016
    .word 0x0000102d
    .word 0x14400014
    .word 0x0000102d
    .word 0x38620004
    .word 0x10400005
    .word 0x8fa40008
    .word 0x0480fff8
    .word 0x28820020
    .word 0x54400004
    .word 0x2882001f
    .word 0x3c02ffff
    .word 0x1000000a
    .word 0x3442ffff
    .word 0x54400005
    .word 0x2402001e
    .word 0x8fa3000c
    .word 0x2482ffe2
    .word 0x10000004
    .word 0x00431004
    .word 0x8fa3000c
    .word 0x00441023
    .word 0x00431006
    .word 0xdfbf0020
    .word 0x03e00008
    .word 0x27bd0030
}
