/* Handwritten function */
// Image decode 4bpp loop: pcpyld interleave, qfsrv barrel-shift, pextlb expand, sq
asm void func_001043F0(void *a0) {
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $14, 0x0($4)
    lw $13, 0x4($4)
    lw $12, 0x10($4)
    sll $11, $12, 1
    .word 0x05B80000
    addiu $24, $zero, -0x1
L_00104410:
    lw $7, 0x8($4)
    .word 0x240FFFFF
L_00104418:
    ld $8, 0x0($5)
    ld $9, 0x0($6)
    .word 0x71284389
    .word 0x71084EE8
    pextlb $8, $zero, $9
    sq $8, 0x0($14)
    .word 0x20E7FFFF
    addu $5, $5, $12
    addu $14, $14, $11
    .word 0x1CE0FFF6
    addu $6, $6, $12
    addiu $5, $5, 0x140
    addiu $6, $6, 0x140
    lw $7, 0xC($4)
    and $10, $15, $7
    .word 0x1540FFF0
    daddu $15, $zero, $zero
    lw $5, 0x14($4)
    lw $6, 0x18($4)
    lw $14, 0x0($4)
    addiu $5, $5, 0x40
    addiu $6, $6, 0x40
    addiu $14, $14, 0x80
    .word 0x1700FFE6
    .word 0x0000C02D
    jr $ra
    nop
}
