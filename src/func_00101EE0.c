// Write a0 to HW reg 0x1000E060 if a0 != -1; always return current value
asm int func_00101EE0(int a0) {
    lui $2, 0x1000
    lui $3, 0xFFFF
    ori $2, $2, 0xE060
    ori $3, $3, 0xFFFF
    .word 0x10830003
    lw $2, 0x0($2)
    lui $1, 0x1001
    sw $4, -0x1FA0($1)
    jr $ra
    nop
}
