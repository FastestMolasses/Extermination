// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0011AFC0(void) {
    .word 0x4842e000
    .word 0x000212c2
    .word 0x03e00008
    .word 0x30420001
}
