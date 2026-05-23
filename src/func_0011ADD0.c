// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0011ADD0(void) {
    .word 0x4843e800
    .word 0x24040001
    .word 0x3062000f
    .word 0x14440003
    .word 0x30630f00
    .word 0x03e00008
    .word 0x24020001
    .word 0x0080102d
    .word 0x38630100
    .word 0x03e00008
    .word 0x0003100b
}
