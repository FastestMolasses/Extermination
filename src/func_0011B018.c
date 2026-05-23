// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0011B018(void) {
    .word 0x3084ffff
    .word 0x48c4f800
    .word 0x03e00008
    .word 0x00000000
}
