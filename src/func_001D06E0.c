// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001D06E0(void) {
    .word 0x8c830090
    .word 0x24640070
    .word 0x14a00009
    .word 0xa0650080
    .word 0x70001e28
    .word 0x24630001
    .word 0xac800020
    .word 0x28610006
    .word 0x24840004
    .word 0x00000000
    .word 0x1420fffa
    .word 0x00000000
    .word 0x03e00008
    .word 0x00000000
}
