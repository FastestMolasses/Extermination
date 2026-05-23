asm void func_002063B0(void) {
    lw $3, 0x0($4)
    .word 0x1460000E
    nop
    lw $9, 0x2C($4)
    addiu $10, $4, 0x4
    addiu $3, $0, 0x28
    addu $9, $10, $9
    sw $9, 0x0($5)
    lw $5, 0x2C($4)
    subu $3, $3, $5
    sw $3, 0x0($6)
    lw $3, 0x30($4)
    sw $3, 0x0($7)
    lw $3, 0x3C($4)
    .word 0x1000001E
    sw $3, 0x0($8)
    lw $10, 0x3C($4)
    lw $3, 0x38($4)
    lw $11, 0x34($4)
    subu $9, $10, $3
    subu $3, $10, $11
    slt $3, $3, $9
    .word 0x14600008
    nop
    lw $3, 0x30($4)
    addu $3, $3, $11
    sw $3, 0x0($5)
    sw $9, 0x0($6)
    sw $0, 0x0($7)
    .word 0x1000000F
    sw $0, 0x0($8)
    lw $3, 0x30($4)
    addu $3, $3, $11
    sw $3, 0x0($5)
    lw $5, 0x3C($4)
    lw $3, 0x34($4)
    subu $3, $5, $3
    sw $3, 0x0($6)
    lw $3, 0x30($4)
    sw $3, 0x0($7)
    lw $5, 0x3C($4)
    lw $3, 0x34($4)
    subu $3, $5, $3
    subu $3, $9, $3
    sw $3, 0x0($8)
    jr $ra
    nop
}
