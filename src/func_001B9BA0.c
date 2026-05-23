// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001B9BA0(void) {
    .word 0x90a20004
    .word 0x14400007
    .word 0x00000000
    .word 0xc4c0000c
    .word 0xe4c00010
    .word 0x90a20004
    .word 0x24420001
    .word 0x1000000f
    .word 0xa0a20004
    .word 0xc4c10010
    .word 0x44800000
    .word 0x00000000
    .word 0x46000836
    .word 0x00000000
    .word 0x45000004
    .word 0x3c023f80
    .word 0x10000007
    .word 0x24020001
    .word 0x3c023f80
    .word 0x44820000
    .word 0x00000000
    .word 0x46000801
    .word 0xe4c00010
    .word 0x70001628
    .word 0x03e00008
    .word 0x00000000
}
