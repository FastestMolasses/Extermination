// double normalization / exponent check
asm int func_0011DB10(int a0) {
    dsll32 $2, $4, 0
    dsra32 $2, $2, 0
    dsra32 $4, $4, 0
    lui $3, 0x7FFF
    negu $5, $2
    ori $3, $3, 0xFFFF
    or $2, $2, $5
    and $4, $4, $3
    srl $2, $2, 31
    .word 0x3C057FF0
    or $4, $4, $2
    subu $4, $5, $4
    addiu $2, $zero, 0x1
    negu $3, $4
    or $4, $4, $3
    srl $4, $4, 31
    jr $ra
    subu $2, $2, $4
}
