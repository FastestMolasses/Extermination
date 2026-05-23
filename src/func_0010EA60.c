// Check if a0->a1 is non-null, a0->4 == a1->0x18, and a1->0x10 & 1
asm int func_0010EA60(int *a0) {
    lw $5, 0x0($4)
    .word 0x10a00009
    nop
    lw $3, 0x4($4)
    lw $2, 0x18($5)
    .word 0x14620005
    nop
    lw $2, 0x10($5)
    andi $2, $2, 0x1
    .word 0x14400003
    nop
    jr $ra
    daddu $2, $zero, $zero
    jr $ra
    addiu $2, $zero, 0x1
}
