// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00203A10(void) {
    .word 0x3c010005
    .word 0x00810821
    .word 0x8c260008
    .word 0x3c010005
    .word 0x00810821
    .word 0x8c220004
    .word 0x00c21023
    .word 0x00a2082a
    .word 0x0041280a
    .word 0x3c010005
    .word 0x00810821
    .word 0x8c230000
    .word 0x70a01628
    .word 0x00651821
    .word 0x0066001a
    .word 0x3c010005
    .word 0x00810821
    .word 0x00001810
    .word 0xac230000
    .word 0x3c010005
    .word 0x00810821
    .word 0x8c230004
    .word 0x3c010005
    .word 0x00651821
    .word 0x00810821
    .word 0x03e00008
    .word 0xac230004
}
