// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00111A80(void) {
    .word 0x90870000
    .word 0x2409000a
    .word 0x90880001
    .word 0x240a003c
    .word 0x00071102
    .word 0x90850002
    .word 0x00491018
    .word 0x00083102
    .word 0x30e7000f
    .word 0x00c93018
    .word 0x3108000f
    .word 0x00051902
    .word 0x240b004b
    .word 0x00691818
    .word 0x00471021
    .word 0x30a5000f
    .word 0x704a1018
    .word 0x00c83021
    .word 0x00651821
    .word 0x00461021
    .word 0x004b1018
    .word 0x00431021
    .word 0x03e00008
    .word 0x2442ff6a
}
