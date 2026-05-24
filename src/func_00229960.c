// All-word: everything as .word except jal/j-external
extern void func_00228320(int, int, int, int);

asm void func_00229960(void) {
    .word 0x27bdffe0
    .word 0x7fbf0010
    .word 0x7fb00000
    .word 0x90830006
    .word 0x24020001
    .word 0x1062000f
    .word 0x70808628
    .word 0x10600003
    .word 0x00000000
    .word 0x10000017
    .word 0x7bbf0010
    .word 0x8e020038
    jal       func_00228320
    .word 0xae02005c
    .word 0x10400004
    .word 0x24020001
    .word 0x24020001
    .word 0xa2020005
    .word 0x24020001
    .word 0x1000000c
    .word 0xa2020006
    .word 0x8e02003c
    jal       func_00228320
    .word 0xae02005c
    .word 0x10400005
    .word 0x24020002
    .word 0x24020001
    .word 0x10000003
    .word 0xa2020005
    .word 0x24020002
    .word 0xa2020005
    .word 0xa2000006
    .word 0x7bbf0010
    .word 0x7bb00000
    .word 0x70001628
    .word 0x03e00008
    .word 0x27bd0020
}
