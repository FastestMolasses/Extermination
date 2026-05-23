// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001B0D80(void) {
    .word 0xc48100b4
    .word 0x3c02c348
    .word 0x44820000
    .word 0x00000000
    .word 0x46000834
    .word 0x00000000
    .word 0x45000006
    .word 0x70001628
    .word 0x24020003
    .word 0xa0820004
    .word 0x10000002
    .word 0x24020001
    .word 0x70001628
    .word 0x03e00008
    .word 0x00000000
}
