// func_00108640: returns the top a1 bits of the 64-bit value at *a0 (logical
// shift right by 64 - a1), sign-extended from its low 32 bits.
asm long long func_00108640(long long *a0, int a1) {
    ld $2, 0x0($4)
    addiu $3, $zero, 0x40
    subu $3, $3, $5
    dsrlv $2, $2, $3
    dsll32 $2, $2, 0
    jr $ra
    dsra32 $2, $2, 0
}
