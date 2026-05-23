// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001216B8(void) {
    .word 0x27bdfff0
    .word 0x03a0182d
    .word 0x10c00005
    .word 0x00a5180b
    .word 0x54e00005
    .word 0x90c20000
    .word 0x10000006
    .word 0x2402ffff
    .word 0x10000004
    .word 0x0000102d
    .word 0xac620000
    .word 0x90c20000
    .word 0x0002102b
    .word 0x03e00008
    .word 0x27bd0010
}
