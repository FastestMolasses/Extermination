// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00182D40(void) {
    .word 0x908301f0
    .word 0x24020017
    .word 0x14620004
    .word 0x70001628
    .word 0x10000002
    .word 0x24020001
    .word 0x70001628
    .word 0x03e00008
    .word 0x00000000
}
