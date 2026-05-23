// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00179880(void) {
    .word 0xc4a20000
    .word 0x3c03bd23
    .word 0x3463d70a
    .word 0x44830800
    .word 0x3c03c080
    .word 0x46011040
    .word 0x44830000
    .word 0x00000000
    .word 0x46000834
    .word 0x00000000
    .word 0x45000002
    .word 0xe4a10000
    .word 0xaca30000
    .word 0xc4a00000
    .word 0xc48100b4
    .word 0x24030002
    .word 0x46000800
    .word 0xe48000b4
    .word 0x03e00008
    .word 0xa083025f
}
