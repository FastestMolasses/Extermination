// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0011AAA0(void) {
    .word 0x4843e000
    .word 0x24020001
    .word 0x14820003
    .word 0x00000000
    .word 0x10000004
    .word 0x34630004
    .word 0x3c02ffff
    .word 0x3442fffb
    .word 0x00621824
    .word 0x48c3e000
    .word 0x03e00008
    .word 0x00000000
}
