// All-word: everything as .word except jal/j-external
extern void func_0011C7B0(int, int, int, int);
extern void func_0011CCC8(int, int, int, int);
extern void func_0011D770(int, int, int, int);

asm void func_0011DE90(void) {
    .word 0x44026000
    .word 0x27bdffe0
    .word 0x0040202d
    .word 0x3c037fff
    .word 0x3463ffff
    .word 0x3c023f49
    .word 0x00832024
    .word 0x34420fd8
    .word 0x0044102a
    .word 0x14400006
    .word 0xffbf0010
    .word 0x44806800
    jal       func_0011CCC8
    .word 0x00000000
    .word 0x10000029
    .word 0xdfbf0010
    .word 0x3c027f7f
    .word 0x3442ffff
    .word 0x0044102a
    .word 0x10400003
    .word 0x00000000
    .word 0x10000021
    .word 0x460c6001
    jal       func_0011C7B0
    .word 0x03a0202d
    .word 0x30430003
    .word 0x24020001
    .word 0x1062000f
    .word 0x28620002
    .word 0x10400005
    .word 0x24020002
    .word 0x10600007
    .word 0xc7ac0000
    .word 0x10000013
    .word 0x24040001
    .word 0x1062000d
    .word 0xc7ac0000
    .word 0x1000000f
    .word 0x24040001
    jal       func_0011CCC8
    .word 0xc7ad0004
    .word 0x1000000e
    .word 0xdfbf0010
    .word 0xc7ac0000
    .word 0x24040001
    jal       func_0011D770
    .word 0xc7ad0004
    .word 0x10000007
    .word 0x46000007
    jal       func_0011CCC8
    .word 0xc7ad0004
    .word 0x10000003
    .word 0x46000007
    jal       func_0011D770
    .word 0xc7ad0004
    .word 0xdfbf0010
    .word 0x03e00008
    .word 0x27bd0020
}
