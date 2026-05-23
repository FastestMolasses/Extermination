// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001E7C60(void) {
    .word 0x70003628
    .word 0x70804628
    .word 0x70002e28
    .word 0x71003e28
    .word 0x24a50001
    .word 0xe4ec0064
    .word 0x28a30020
    .word 0x24e70010
    .word 0xe48c0004
    .word 0x1460fffa
    .word 0x00000000
    .word 0x24c60001
    .word 0x28c30020
    .word 0x1460fff4
    .word 0x25080200
    .word 0x03e00008
    .word 0x00000000
}
