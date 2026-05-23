// HW register poll loop: write 0x4 to 0x1000F000, spin waiting for FIFO-ready bit
asm void func_0010BC90(void) {
    lui $2, 0x1000
    addiu $3, $zero, 0x4
    ori $2, $2, 0xF000
    sw $3, 0x0($2)
    lui $2, 0x1001
    lw $2, -0x1000($2)
    andi $2, $2, 0x4
    nop
    nop
    nop
    .word 0x1040FFF9
    nop
    addiu $2, $zero, 0x4
    lui $1, 0x1001
    jr $ra
    sw $2, -0x1000($1)
}
