// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001FE4D0(void) {
    .word 0x8c820004
    .word 0x8c830000
    .word 0x24870010
    .word 0x00a2102a
    .word 0x14400003
    .word 0x00832021
    .word 0x1000000e
    .word 0x70001628
    .word 0x00051100
    .word 0x00471821
    .word 0x8c62000c
    .word 0x00021102
    .word 0x00c2102b
    .word 0x14400003
    .word 0x00000000
    .word 0x10000005
    .word 0x70001628
    .word 0x8c630000
    .word 0x00061100
    .word 0x00831821
    .word 0x00621021
    .word 0x03e00008
    .word 0x00000000
}
