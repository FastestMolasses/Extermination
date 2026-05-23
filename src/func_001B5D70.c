// Asm-void leaf, encoded entirely as .word directives — used when
// expressing the function in source-level C or even labeled asm would
// be impractical or would force mwcc into non-matching codegen.
asm void func_001B5D70(void) {
    .word 0x14a00005
    .word 0x70001628
    .word 0x34038000
    .word 0x3063ffff
    .word 0x10000003
    .word 0x64052000
    .word 0x64031000
    .word 0x64054000
    .word 0x308400ff
    .word 0x28810010
    .word 0x10200004
    .word 0x288100e1
    .word 0x10000005
    .word 0x3062ffff
    .word 0x288100e1
    .word 0x14200002
    .word 0x00000000
    .word 0x30a2ffff
    .word 0x03e00008
    .word 0x00000000
}
