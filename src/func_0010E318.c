// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0010E318(void) {
    .word 0x8c830010
    .word 0x3c02ffff
    .word 0x3442fffe
    .word 0xac800018
    .word 0x00621824
    .word 0x03e00008
    .word 0xac830010
}
