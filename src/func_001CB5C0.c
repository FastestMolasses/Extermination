// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001CB5C0(void) {
    .word 0x70002e28
    .word 0x24a50001
    .word 0xac800000
    .word 0xac804000
    .word 0x28a31000
    .word 0x24840004
    .word 0x1460fffa
    .word 0x00000000
    .word 0x03e00008
    .word 0x00000000
}
