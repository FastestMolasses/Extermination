/* Handwritten function */
// Image bilinear 1D decode nested: ld+pcpyld+mtsab+qfsrv+pextlb + second pass mtsab+qfsrv
asm void func_001046C0(void *a0) {
    pnor $25, $zero, $zero
    psrlh $25, $25, 15
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $14, 0x0($4)
    lw $13, 0x4($4)
    addiu $24, $zero, 0x1
    addiu $12, $zero, -0x1
    lw $3, 0x10($4)
    sll $2, $3, 1
L_001046E8:
    lw $7, 0x8($4)
    .word 0x240BFFFF
L_001046F0:
    ld $8, 0x0($5)
    ld $9, 0x0($6)
    .word 0x71284389
    .word 0x05B80000
    .word 0x710846E8
    pextlb $9, $zero, $8
    .word 0x20E7FFFF
    addu $5, $5, $3
    addu $6, $6, $3
    .word 0x07180000
    .word 0x700856E8
    pextlb $8, $zero, $10
    paddh $10, $9, $8
    paddh $10, $10, $25
    psrlh $10, $10, 1
    sq $10, 0x0($14)
    .word 0x1CE0FFEF
    addu $14, $14, $2
    addiu $5, $5, 0x140
    addiu $6, $6, 0x140
    lw $7, 0xC($4)
    and $10, $11, $7
    .word 0x1540FFE9
    daddu $11, $zero, $zero
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $14, 0x0($4)
    addiu $5, $5, 0x40
    addiu $6, $6, 0x40
    addiu $14, $14, 0x80
    .word 0x1580FFDF
    daddu $12, $zero, $zero
    jr $ra
    nop
}
