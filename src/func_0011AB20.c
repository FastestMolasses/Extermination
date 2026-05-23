// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_0011AB20(void) {
    .word 0x4843e800
    .word 0x30620002
    .word 0x10400003
    .word 0x30620004
    .word 0x03e00008
    .word 0x24020001
    .word 0x10400003
    .word 0x30620008
    .word 0x03e00008
    .word 0x24020002
    .word 0x10400003
    .word 0x30620001
    .word 0x03e00008
    .word 0x24020003
    .word 0x03e00008
    .word 0x00021080
}
