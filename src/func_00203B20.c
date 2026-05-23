// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_00203B20(void) {
    .word 0xac850000
    .word 0xac860004
    .word 0xac870010
    .word 0xac80000c
    .word 0x0007082a
    .word 0xac800008
    .word 0x1020000b
    .word 0x70004628
    .word 0x70004e28
    .word 0x3c030003
    .word 0x34659640
    .word 0x8c860004
    .word 0x25080001
    .word 0x0107182a
    .word 0x00c93021
    .word 0x01254821
    .word 0x1460fffa
    .word 0xacc00000
    .word 0x03e00008
    .word 0x00000000
}
