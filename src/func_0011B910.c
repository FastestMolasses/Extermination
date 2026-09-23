// Writes 1 to the hardware register at 0x10003C10, then 6 to 0x10003C20
// (the second store sits in the return delay slot).
asm void func_0011B910(void) {
    lui $2, 0x1000
    addiu $4, $zero, 0x1
    ori $2, $2, 0x3C10
    lui $3, 0x1000
    sw $4, 0x0($2)
    ori $3, $3, 0x3C20
    addiu $2, $zero, 0x6
    jr $ra
    sw $2, 0x0($3)
}
