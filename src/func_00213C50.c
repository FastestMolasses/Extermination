// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00213C50(void) {
    .word 0x24030001
    .word 0x14a30007
    .word 0x90870019
    .word 0x54e00005
    .word 0x24e7ffff
    .word 0x90830018
    .word 0x10000002
    .word 0x2467ffff
    .word 0x24e7ffff
    .word 0x70003628
    .word 0x00871821
    .word 0x90650050
    .word 0x24e70001
    .word 0x00861821
    .word 0xa0650090
    .word 0x90830018
    .word 0x00e3182a
    .word 0x54600003
    .word 0x24c60001
    .word 0x70003e28
    .word 0x24c60001
    .word 0x28c30009
    .word 0x1460fff4
    .word 0x00871821
    .word 0x03e00008
    .word 0x00000000
}
