// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00181110(void) {
    .word 0xc4800224
    .word 0x44800800
    .word 0x00000000
    .word 0x46010032
    .word 0x00000000
    .word 0x4500000b
    .word 0x24020002
    .word 0xc480022c
    .word 0x46010032
    .word 0x00000000
    .word 0x45000005
    .word 0x00000000
    .word 0x9082000f
    .word 0x30420002
    .word 0x1040000a
    .word 0x70001628
    .word 0x24020002
    .word 0xa0820004
    .word 0x24020004
    .word 0xa0820005
    .word 0xa0800006
    .word 0x24020001
    .word 0x10000002
    .word 0xa0850302
    .word 0x70001628
    .word 0x03e00008
    .word 0x00000000
}
