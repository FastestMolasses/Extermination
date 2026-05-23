asm void func_0021C350(void) {
    lwc1 $2, 0x224($4)
    mtc1 $0, $0
    nop
    c.eq.s $2, $0
    nop
    .word 0x4501001D
    nop
    lwc1 $1, 0x220($4)
    lui $3, (0x420C0000 >> 16)
    mtc1 $3, $0
    nop
    sub.s $1, $1, $2
    swc1 $1, 0x220($4)
    sw $0, 0x224($4)
    lwc1 $1, 0x220($4)
    c.le.s $1, $0
    nop
    .word 0x45000007
    nop
    lbu $3, 0x235($4)
    andi $3, $3, 0xFE
    sb $3, 0x235($4)
    lbu $3, 0x235($4)
    ori $3, $3, 0x1
    sb $3, 0x235($4)
    lwc1 $1, 0x220($4)
    mtc1 $0, $0
    nop
    c.le.s $1, $0
    nop
    .word 0x45000004
    nop
    addiu $3, $0, 0x2
    sw $0, 0x220($4)
    sb $3, 0x0($4)
    jr $ra
    nop
}
