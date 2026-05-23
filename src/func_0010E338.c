// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0010E338(void) {
    .word 0x8c850024
    .word 0x8c830018
    .word 0x00a3001a
    .word 0x50600001
    .word 0x000001cd
    .word 0x8c850014
    .word 0x00001010
    .word 0x24430001
    .word 0x00021180
    .word 0xac830024
    .word 0x03e00008
    .word 0x00a21021
}
