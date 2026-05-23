/* Handwritten function */
// Image bilinear 1D decode: ld+pcpyld+qfsrv+pextlb, odd/even loop variants
asm void func_00104540(void *a0) {
    pnor $25, $zero, $zero
    psrlh $25, $25, 15
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $14, 0x0($4)
    lw $13, 0x4($4)
    lw $12, 0x10($4)
    addiu $11, $zero, 0x1
    sll $24, $12, 1
    .word 0x05B80000
L_00104568:
    lw $7, 0x8($4)
    ld $8, 0x0($5)
    ld $9, 0x0($6)
    .word 0x71284389
    .word 0x710846E8
    ori $11, $11, 0x8000
    .word 0x10E00010
    pextlb $15, $zero, $8
L_00104588:
    addu $5, $5, $12
    addu $6, $6, $12
    ld $8, 0x0($5)
    ld $9, 0x0($6)
    .word 0x71284389
    .word 0x710846E8
    pextlb $10, $zero, $8
    .word 0x20E7FFFF
    paddh $9, $10, $15
    por $15, $10, $zero
    paddh $10, $9, $25
    psrlh $10, $10, 1
    sq $10, 0x0($14)
    .word 0x1CE0FFF2
    addu $14, $14, $24
L_001045C4:
    .word 0x700B53F7
    addiu $5, $5, 0x140
    lw $7, 0xC($4)
    addiu $6, $6, 0x140
    and $10, $10, $7
    .word 0x1540FFEB
    andi $11, $11, 0x7FFF
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $14, 0x0($4)
    addiu $5, $5, 0x40
    addiu $6, $6, 0x40
    addiu $14, $14, 0x80
    andi $10, $11, 0x1
    .word 0x1540FFDA
    andi $11, $11, 0xFFFE
    jr $ra
    nop
}
