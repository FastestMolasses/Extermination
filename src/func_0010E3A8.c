// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void _iSignalSema(int, int, int, int);
extern void func_0010E318(int, int, int, int);

asm void func_0010E3A8(void) {
    .word 0x27bdffd0
    .word 0x3c028000
    .word 0xffb10010
    .word 0x3442000a
    .word 0xffbf0020
    .word 0x0080882d
    .word 0xffb00000
    .word 0x8e230020
    .word 0x10620009
    .word 0x0043102b
    .word 0x54400015
    .word 0x8e30001c
    .word 0x3c028000
    .word 0x34420009
    .word 0x5062000b
    .word 0x8e220024
    .word 0x1000000f
    .word 0x8e30001c
    .word 0x8e30001c
    .word 0x8e02001c
    .word 0x5040000c
    .word 0x8e040008
    .word 0x0040f809
    .word 0x8e040020
    .word 0x10000007
    .word 0x8e30001c
    .word 0x8e30001c
    .word 0xae020024
    .word 0x8e230028
    .word 0xae030014
    .word 0x8e22002c
    .word 0xae020018
    .word 0x8e040008
    .word 0x04800003
    .word 0x00000000
    jal       _iSignalSema
    .word 0x00000000
    jal       func_0010E318
    .word 0x8e040000
    .word 0xae000000
    .word 0xdfbf0020
    .word 0xdfb10010
    .word 0xdfb00000
    .word 0x03e00008
    .word 0x27bd0030
}
