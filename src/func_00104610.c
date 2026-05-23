/* Handwritten function */
// Image bilinear decode (qfsrv both directions, average): lq rows, byte-expand, paddh avg
asm void func_00104610(void *a0) {
    pnor $25, $zero, $zero
    psrlh $25, $25, 15
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $7, 0x8($4)
    lw $14, 0x0($4)
    lw $13, 0x4($4)
    addiu $24, $zero, 0x1
    lw $9, 0x10($4)
    sll $8, $9, 1
    addiu $11, $zero, -0x1
L_0010463C:
    lq $10, 0x0($5)
    lq $15, 0x0($6)
    .word 0x05B80000
    .word 0x71EA16E8
    .word 0x714F1EE8
    pextlb $10, $zero, $2
    .word 0x20E7FFFF
    pextub $15, $zero, $2
    .word 0x07180000
    .word 0x70621EE8
    pextlb $2, $zero, $3
    pextub $3, $zero, $3
    paddh $10, $10, $2
    paddh $15, $15, $3
    paddh $2, $10, $25
    paddh $3, $15, $25
    psrlh $2, $2, 1
    psrlh $3, $3, 1
    sq $2, 0x0($14)
    sq $3, 0x10($14)
    addu $5, $5, $9
    addu $6, $6, $9
    .word 0x1CE0FFE9
    addu $14, $14, $8
    addiu $5, $5, 0x80
    addiu $6, $6, 0x80
    lw $7, 0xC($4)
    and $12, $11, $7
    .word 0x1580FFE3
    daddu $11, $zero, $zero
    jr $ra
    nop
}
