// cfc2 $v1,$29; cascade check bits 2,4,8,1; return 1,2,3,sll($v0,2)
asm int func_0011AB20(void) {
    cfc2 $3, $29
    andi $2, $3, 0x2
    .word 0x10400003
    andi $2, $3, 0x4
    jr $ra
    addiu $2, $zero, 0x1
    .word 0x10400003
    andi $2, $3, 0x8
    jr $ra
    addiu $2, $zero, 0x2
    .word 0x10400003
    andi $2, $3, 0x1
    jr $ra
    addiu $2, $zero, 0x3
    jr $ra
    sll $2, $2, 2
}
