// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00105148(void) {
    .word 0x3c051000
    .word 0x3c03ff7f
    .word 0x34a52010
    .word 0x3463ffff
    .word 0x8ca20000
    .word 0x000425c0
    .word 0x00431024
    .word 0x00441025
    .word 0x03e00008
    .word 0xaca20000
}
