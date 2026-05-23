asm void func_00206470(void) {
    lw $3, 0x0($4)
    .word 0x14600010
    nop
    lw $6, 0x2C($4)
    addiu $3, $0, 0x28
    subu $6, $3, $6
    sltu $at, $6, $5
    movz $6, $5, $at
    lw $3, 0x2C($4)
    addu $3, $3, $6
    sw $3, 0x2C($4)
    lw $3, 0x2C($4)
    sltiu $3, $3, 0x28
    .word 0x54600004
    subu $5, $5, $6
    addiu $3, $0, 0x1
    sw $3, 0x0($4)
    subu $5, $5, $6
    lw $6, 0x34($4)
    lw $3, 0x3C($4)
    addu $6, $6, $5
    .word 0x00C3001A
    nop
    nop
    mfhi $3
    sw $3, 0x34($4)
    lw $3, 0x38($4)
    addu $3, $3, $5
    sw $3, 0x38($4)
    lw $3, 0x40($4)
    addu $3, $3, $5
    jr $ra
    sw $3, 0x40($4)
}
