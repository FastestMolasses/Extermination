// Writes 1 to the hardware register at 0x10003000.
asm void func_0011B328(void) {
    lui $2, 0x1000
    addiu $3, $zero, 0x1
    ori $2, $2, 0x3000
    jr $ra
    sw $3, 0x0($2)
}
