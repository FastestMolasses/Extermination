// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_002041A0(void) {
    .word 0x0005183c
    .word 0x0003183e
    .word 0x0003283c
    .word 0x0006183c
    .word 0x0003183e
    .word 0x00031f38
    .word 0x00a32825
    .word 0x0007183c
    .word 0x0003183e
    .word 0x00651825
    .word 0x03e00008
    .word 0xfc830000
}
