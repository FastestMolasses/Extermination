// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00205A00(void) {
    .word 0x000a13c0
    .word 0x004b1025
    .word 0xac820000
    .word 0x00091b80
    .word 0x000813c0
    .word 0x00621825
    .word 0x00071680
    .word 0x00431825
    .word 0x00061700
    .word 0x00431025
    .word 0xac820004
    .word 0x0005103e
    .word 0x0002103c
    .word 0xac850008
    .word 0x0002103f
    .word 0xac82000c
    .word 0x03e00008
    .word 0x24820010
}
