// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001015A8(void) {
    .word 0xdc820030
    .word 0x00052c00
    .word 0x00063400
    .word 0x00052c03
    .word 0x00021c3a
    .word 0x00063403
    .word 0x0002143e
    .word 0x306307ff
    .word 0x304207ff
    .word 0x0003183c
    .word 0x0003183f
    .word 0x0002103c
    .word 0x0002103f
    .word 0x0006343c
    .word 0x24420001
    .word 0x00052c3c
    .word 0x24630001
    .word 0x0002107a
    .word 0x0006343f
    .word 0x00052c3f
    .word 0x0003187a
    .word 0x00c2302f
    .word 0x00a3282f
    .word 0x00061138
    .word 0x00073c00
    .word 0x10e00004
    .word 0x00052938
    .word 0x64420008
    .word 0x10000002
    .word 0x0002103c
    .word 0x0006113c
    .word 0x00a21025
    .word 0x03e00008
    .word 0xfc820020
}
