// cfc2 $v0,$29; andi $v0,$v0,0xF; xori $v0,$v0,1; jr $ra; sltiu $v0,$v0,1
asm int func_0011A9C0(void) {
    cfc2 $2, $29
    andi $2, $2, 0xF
    xori $2, $2, 0x1
    jr $ra
    sltiu $2, $2, 0x1
}
