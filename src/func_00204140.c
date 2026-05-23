// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00204140(void) {
    .word 0x3c090001
    .word 0x00000000
    .word 0x42000039
    .word 0x0000040f
    .word 0x40036000
    .word 0x00691824
    .word 0x1460fffa
    .word 0x00000000
    .word 0x3c031000
    .word 0x3468f520
    .word 0x8d070000
    .word 0x3466f590
    .word 0x3465b400
    .word 0x3c03fffe
    .word 0x3463ffff
    .word 0x00e93825
    .word 0xacc70000
    .word 0xaca40000
    .word 0x8d040000
    .word 0x00831824
    .word 0xacc30000
    .word 0x03e00008
    .word 0x42000038
}
