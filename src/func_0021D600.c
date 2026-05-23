// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0021D600(void) {
    .word 0x908301f1
    .word 0x24020001
    .word 0x10620006
    .word 0x24020001
    .word 0x2462fffd
    .word 0x2c410002
    .word 0x10200005
    .word 0x70001628
    .word 0x24020001
    .word 0x10000002
    .word 0x00000000
    .word 0x70001628
    .word 0x03e00008
    .word 0x00000000
}
