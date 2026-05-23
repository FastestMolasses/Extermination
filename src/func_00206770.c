asm void func_00206770(void) {
    lw $10, 0x48($8)
    lw $11, 0x4C($8)
    addu $3, $9, $10
    subu $3, $3, $11
    addiu $3, $3, -0x400
    .word 0x006A001A
    nop
    nop
    mfhi $9
    subu $3, $10, $11
    sra $9, $9, 0xA
    sll $9, $9, 0xA
    slt $3, $3, $9
    .word 0x14600008
    nop
    lw $3, 0x44($8)
    addu $3, $3, $11
    sw $3, 0x0($4)
    sw $9, 0x0($5)
    sw $0, 0x0($6)
    .word 0x1000000F
    sw $0, 0x0($7)
    lw $3, 0x44($8)
    addu $3, $3, $11
    sw $3, 0x0($4)
    lw $4, 0x48($8)
    lw $3, 0x4C($8)
    subu $3, $4, $3
    sw $3, 0x0($5)
    lw $3, 0x44($8)
    sw $3, 0x0($6)
    lw $4, 0x48($8)
    lw $3, 0x4C($8)
    subu $3, $4, $3
    subu $3, $9, $3
    sw $3, 0x0($7)
    jr $ra
    nop
}
