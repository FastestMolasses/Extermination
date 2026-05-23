asm void func_001B1470(void) {
    lui $2, (0x40490FDB >> 16)
    ori $2, $2, (0x40490FDB & 0xFFFF)
    mtc1 $2, $1
    nop
    c.le.s $12, $1
    nop
    .word 0x4501000C
    .word 0x3C02C049
    lui $2, (0x40C90FDB >> 16)
    ori $2, $2, (0x40C90FDB & 0xFFFF)
    mtc1 $2, $0
    sub.s $12, $12, $0
    c.le.s $12, $1
    nop
    nop
    nop
    .word 0x4500FFFA
    nop
    .word 0x3C02C049
    ori $2, $2, (0xC0490FDB & 0xFFFF)
    mtc1 $2, $1
    nop
    c.le.s $12, $1
    nop
    .word 0x4500000C
    mov.s $0, $12
    lui $2, (0x40C90FDB >> 16)
    ori $2, $2, (0x40C90FDB & 0xFFFF)
    mtc1 $2, $0
    add.s $12, $12, $0
    c.le.s $12, $1
    nop
    nop
    nop
    .word 0x4501FFFA
    nop
    mov.s $0, $12
    jr $ra
    nop
    nop
}
