// All-word: everything as .word except jal/j-external
extern void func_0010E338(int, int, int, int);
extern void func_0010E5F8(int, int, int, int);
extern void func_0010DE78(int, int, int, int);

asm void func_0010E648(void) {
    .word 0x27bdffc0
    .word 0xffb10010
    .word 0xffb00000
    .word 0x00a0882d
    .word 0x0080802d
    .word 0xffb20020
    .word 0xffbf0030
    jal       func_0010E338
    .word 0x0220202d
    .word 0x0040902d
    .word 0x8e04001c
    .word 0x8e030014
    .word 0x3c028000
    .word 0x34420009
    .word 0xae44001c
    .word 0xae430014
    .word 0x0220282d
    .word 0xae420020
    jal       func_0010E5F8
    .word 0x8e040020
    .word 0x0040182d
    .word 0x54600005
    .word 0xae430024
    .word 0xae400024
    .word 0xae400028
    .word 0x10000005
    .word 0xae40002c
    .word 0x8c620008
    .word 0xae420028
    .word 0x8c630014
    .word 0xae43002c
    .word 0x0240282d
    .word 0x3c048000
    .word 0xdfbf0030
    .word 0x34840008
    .word 0xdfb20020
    .word 0x24060040
    .word 0xdfb10010
    .word 0x0000382d
    .word 0xdfb00000
    .word 0x0000402d
    .word 0x0000482d
    j         func_0010DE78
    .word 0x27bd0040
}
