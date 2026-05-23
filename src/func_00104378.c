/* Handwritten function */
// Image decode loop: lq rows from src, qfsrv byte-align, pextlb/pextub expand, sq to dst
asm void func_00104378(void *a0) {
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $7, 0x8($4)
    lw $14, 0x0($4)
    lw $13, 0x4($4)
    lw $12, 0x10($4)
    sll $11, $12, 1
    addiu $15, $zero, -0x1
    .word 0x05B80000
L_0010439C:
    lq $8, 0x0($5)
    .word 0x20E7FFFF
    lq $9, 0x0($6)
    addu $5, $5, $12
    .word 0x712856E8
    pextlb $8, $zero, $10
    pextub $9, $zero, $10
    sq $8, 0x0($14)
    addu $6, $6, $12
    sq $9, 0x10($14)
    .word 0x1CE0FFF5
    addu $14, $14, $11
    addiu $5, $5, 0x80
    addiu $6, $6, 0x80
    lw $7, 0xC($4)
    and $10, $15, $7
    .word 0x1540FFEF
    daddu $15, $zero, $zero
    jr $ra
    nop
}
