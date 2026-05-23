/* Handwritten function */
// Image bilinear decode loop: average adjacent rows via paddh, expand pextlb/pextub
asm void func_00104488(void *a0) {
    pnor $25, $zero, $zero
    psrlh $25, $25, 15
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $7, 0x8($4)
    lw $14, 0x0($4)
    lw $13, 0x4($4)
    lw $24, 0x10($4)
    lq $8, 0x0($5)
    sll $12, $24, 1
    lq $9, 0x0($6)
    .word 0x05B80000
    .word 0x712856E8
    pextlb $8, $zero, $10
    addiu $11, $zero, -0x1
    .word 0x10E00015
    pextub $9, $zero, $10
L_001044CC:
    addu $5, $5, $24
    addu $6, $6, $24
    lq $10, 0x0($5)
    lq $15, 0x0($6)
    .word 0x71EA16E8
    pextlb $10, $zero, $2
    .word 0x20E7FFFF
    pextub $15, $zero, $2
    paddh $2, $8, $10
    paddh $3, $9, $15
    por $8, $10, $zero
    por $9, $15, $zero
    paddh $2, $2, $25
    paddh $3, $3, $25
    psrlh $2, $2, 1
    psrlh $3, $3, 1
    sq $2, 0x0($14)
    sq $3, 0x10($14)
    .word 0x1CE0FFED
    addu $14, $14, $12
    addiu $5, $5, 0x80
    addiu $6, $6, 0x80
    lw $7, 0xC($4)
    and $10, $11, $7
    .word 0x1540FFE7
    daddu $11, $zero, $zero
    jr $ra
    nop
}
