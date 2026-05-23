// Circular buffer write-slot increment (lw $a0 → write ptr + 1; wrap if at limit)
asm void func_0010C9F0(void *a0) {
    daddu $5, $4, $zero
    lw $2, 0x4($5)
    lw $4, 0xC($5)
    lw $3, 0x0($5)
    addiu $2, $2, 0x1
    addiu $4, $4, 0x1
    sw $2, 0x4($5)
    addiu $3, $3, 0x10
    addu $3, $5, $3
    .word 0x14830003
    sw $4, 0xC($5)
    addiu $2, $5, 0x10
    sw $2, 0xC($5)
    jr $ra
    nop
}
