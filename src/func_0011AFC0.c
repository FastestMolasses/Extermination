// cfc2 $v0,$28; srl $v0,$v0,11; jr $ra; andi $v0,$v0,1
asm int func_0011AFC0(void) {
    cfc2 $2, $28
    srl $2, $2, 11
    jr $ra
    andi $2, $2, 0x1
}
