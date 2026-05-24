// All-word: everything as .word except jal/j-external
extern void func_00126BE8(int, int, int, int);

asm void func_00127650(void) {
    .word 0x27bdffc0
    .word 0xffa40020
    .word 0x03a0282d
    .word 0xffbf0030
    jal       func_00126BE8
    .word 0x27a40020
    .word 0x8fa30000
    .word 0x38620002
    .word 0x10400003
    .word 0x2c620002
    .word 0x10400003
    .word 0x8fa20004
    .word 0x10000018
    .word 0x0000102d
    .word 0x14400016
    .word 0x0000102d
    .word 0x38620004
    .word 0x10400005
    .word 0x8fa40008
    .word 0x0480fff8
    .word 0x28820020
    .word 0x54400004
    .word 0x2882003d
    .word 0x3c02ffff
    .word 0x1000000c
    .word 0x3442ffff
    .word 0x14400005
    .word 0x2403003c
    .word 0xdfa20010
    .word 0x2483ffc4
    .word 0x10000004
    .word 0x00621014
    .word 0xdfa20010
    .word 0x00641823
    .word 0x00621016
    .word 0x0002103c
    .word 0x0002103f
    .word 0xdfbf0030
    .word 0x03e00008
    .word 0x27bd0040
}
