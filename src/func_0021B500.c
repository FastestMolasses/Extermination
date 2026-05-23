asm void func_0021B500(void) {
    lwc1 $2, 0x4($4)
    lui $3, (0x3BE56042 >> 16)
    ori $3, $3, (0x3BE56042 & 0xFFFF)
    mtc1 $3, $0
    lui $3, (0x3F800000 >> 16)
    add.s $0, $2, $0
    mtc1 $3, $1
    nop
    c.lt.s $0, $1
    nop
    .word 0x45010004
    swc1 $0, 0x4($4)
    .word 0xC4800004
    sub.s $0, $0, $1
    swc1 $0, 0x4($4)
    jr $ra
    nop
}
