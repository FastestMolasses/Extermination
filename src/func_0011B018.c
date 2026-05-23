// andi $a0,$a0,0xFFFF; ctc2 $a0,$31; jr $ra; nop
asm void func_0011B018(int a0) {
    andi $4, $4, 0xFFFF
    ctc2 $4, $31
    jr $ra
    nop
}
