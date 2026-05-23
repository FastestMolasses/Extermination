// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_002040A0(void) {
    .word 0x8c830008
    .word 0x8c820004
    .word 0x24630001
    .word 0x00031900
    .word 0x00431021
    .word 0x0002113c
    .word 0x0002113e
    .word 0x14a20003
    .word 0x00000000
    .word 0x10000004
    .word 0x70001628
    .word 0x8c820000
    .word 0x00a21023
    .word 0x000212c2
    .word 0x03e00008
    .word 0x00000000
}
