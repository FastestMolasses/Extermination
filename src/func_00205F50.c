// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00205F50(void) {
    .word 0x0004193c
    .word 0x0003193e
    .word 0x3c011001
    .word 0xac23a030
    .word 0x3c011001
    .word 0xac20a020
    .word 0x24030105
    .word 0x3c011001
    .word 0x03e00008
    .word 0xac23a000
}
