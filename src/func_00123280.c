// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00123280(void) {
    .word 0x10000002
    .word 0x0080102d
    .word 0x24840001
    .word 0x80830000
    .word 0x10600010
    .word 0x00a0302d
    .word 0x80c70000
    .word 0x10e0fffa
    .word 0x90c80000
    .word 0x10670009
    .word 0x00000000
    .word 0x0060382d
    .word 0x24c60001
    .word 0x00000000
    .word 0x80c30000
    .word 0x1060fff2
    .word 0x90c80000
    .word 0x54e3fffc
    .word 0x24c60001
    .word 0x5100ffef
    .word 0x24840001
    .word 0x03e00008
    .word 0x00821023
}
