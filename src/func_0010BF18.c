// Clears the 64-bit field at a1+0x48, stores 0x2000 to a1+0x4, returns 0.
asm int func_0010BF18(int a0, int *a1) {
    addiu $2, $zero, 0x2000
    sd $zero, 0x48($5)
    sw $2, 0x4($5)
    jr $ra
    daddu $2, $zero, $zero
}
