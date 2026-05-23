// andi $a0,$a0,0xFFFF; ctc2 $a0,$27; jr $ra; vcallmsr (encoded as .word)
asm void func_0011AB60(int a0) {
    andi $4, $4, 0xFFFF
    ctc2 $4, $27
    jr $ra
    vcallmsr $27
}
