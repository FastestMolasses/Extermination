// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0011A9C0(void) {
    .word 0x4842e800
    .word 0x3042000f
    .word 0x38420001
    .word 0x03e00008
    .word 0x2c420001
}
