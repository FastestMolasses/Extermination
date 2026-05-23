// Circular buffer read-slot decrement (lw $a0 → read ptr + 1; wrap if at limit)
asm void func_0010CA30(void *a0) {
    daddu $5, $4, $zero
    lw $2, 0x4($5)
    lw $4, 0x8($5)
    lw $3, 0x0($5)
    addiu $2, $2, -0x1
    addiu $4, $4, 0x1
    sw $2, 0x4($5)
    addiu $3, $3, 0x10
    addu $3, $5, $3
    .word 0x14830003
    sw $4, 0x8($5)
    addiu $2, $5, 0x10
    sw $2, 0x8($5)
    jr $ra
    nop
}
