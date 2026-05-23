// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00119080(void) {
    .word 0x308200ff
    .word 0x30a500ff
    .word 0x00a22823
    .word 0x30e700ff
    .word 0x00a72818
    .word 0x30c600ff
    .word 0x50c00001
    .word 0x000001cd
    .word 0x00a6001a
    .word 0x00002812
    .word 0x00451021
    .word 0x03e00008
    .word 0x304200ff
}
