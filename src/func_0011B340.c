// func_0011B340: writes 8 to the GIF control register at 0x10003000; returns 1.
asm int func_0011B340(void) {
    lui $3, 0x1000
    addiu $4, $zero, 0x8
    ori $3, $3, 0x3000
    addiu $2, $zero, 0x1
    jr $ra
    sw $4, 0x0($3)
}
