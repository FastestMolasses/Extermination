// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001BC6D0(void) {
    .word 0x8c840018
    .word 0x90a30004
    .word 0x24020001
    .word 0x8c840018
    .word 0x1062000e
    .word 0x8c840018
    .word 0x10600003
    .word 0x00000000
    .word 0x10000011
    .word 0x70001628
    .word 0x9082000b
    .word 0x1440000d
    .word 0x00000000
    .word 0x24020002
    .word 0xa082000b
    .word 0x90a20004
    .word 0x24420001
    .word 0x10000007
    .word 0xa0a20004
    .word 0x9084000b
    .word 0x24030003
    .word 0x14830003
    .word 0x00000000
    .word 0x10000002
    .word 0x00000000
    .word 0x70001628
    .word 0x03e00008
    .word 0x00000000
}
