// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00228C50(void) {
    .word 0x70003628
    .word 0x70002e28
    .word 0x00851021
    .word 0x90430000
    .word 0x24a50001
    .word 0x00c31821
    .word 0x2ca20640
    .word 0x1440fffa
    .word 0x306600ff
    .word 0x24030001
    .word 0x30c400ff
    .word 0x70001628
    .word 0x03e00008
    .word 0x0064100b
}
