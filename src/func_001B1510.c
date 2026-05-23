asm void func_001B1510(void) {
    lui $2, (0x40C90FDB >> 16)
    ori $2, $2, (0x40C90FDB & 0xFFFF)
    mtc1 $2, $0
    nop
    c.le.s $12, $0
    nop
    .word 0x4503000A
    .word 0x46006006
    .word 0x46006301
    c.le.s $12, $0
    nop
    nop
    nop
    .word 0x4500FFFA
    nop
    nop
    .word 0x46006006
    jr $ra
    nop
    nop
}
