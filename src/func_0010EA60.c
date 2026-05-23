// All instructions as .word to prevent mwcc dead-code elimination of second jr $ra
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
    .word 0x03e00008
    .word 0x0000102d
    .word 0x03e00008
    .word 0x24020001
}
