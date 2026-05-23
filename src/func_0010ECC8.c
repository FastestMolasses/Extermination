// Interrupt disable spinlock + conditional store
asm int func_0010ECC8(int *a0) {
    lw $5, 0xC($4)
    lui $3, 0x1
    .word 0x42000039
    .word 0x0000040F
    .word 0x40026000
    and $2, $2, $3
    nop
    .word 0x1440FFFA
    nop
    .word 0x54A00003
    lw $3, 0x3C($5)
    .word 0x10000004
    sw $zero, 0x4($4)
    addiu $2, $zero, 0x1
    sw $2, 0x4($4)
    sw $3, 0xC($4)
    .word 0x42000038
    jr $ra
    daddu $2, $5, $zero
}
