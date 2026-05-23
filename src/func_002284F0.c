// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_002284F0(void) {
    .word 0x70003e28
    .word 0x70003628
    .word 0x00861821
    .word 0x90650000
    .word 0x24c60001
    .word 0x00e52821
    .word 0x2cc3063f
    .word 0x1460fffa
    .word 0x30a700ff
    .word 0x24030001
    .word 0xa083000f
    .word 0x24030100
    .word 0x00671823
    .word 0x03e00008
    .word 0xa083063f
}
