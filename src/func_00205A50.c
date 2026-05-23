// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00205A50(void) {
    .word 0x0006103c
    .word 0x0002103e
    .word 0xac820000
    .word 0x0006103e
    .word 0x0002103c
    .word 0x0002103f
    .word 0xac820004
    .word 0xac850008
    .word 0x24820010
    .word 0x03e00008
    .word 0xac80000c
}
