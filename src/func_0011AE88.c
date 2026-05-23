// cfc2 $v0,$28; ori $v0,$v0,0x200; ctc2 $v0,$28; jr $ra; nop
asm void func_0011AE88(void) {
    cfc2 $2, $28
    ori $2, $2, 0x200
    ctc2 $2, $28
    jr $ra
    nop
}
