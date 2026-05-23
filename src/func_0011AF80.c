// cfc2 $v1,$28; li $v0,1; bne $a0,$v0,.+16; nop; b .+20; ori $v1,$v1,0x800;
// lui $v0,0xFFFF; ori $v0,$v0,0xF7FF; and $v1,$v1,$v0; ctc2 $v1,$28; jr $ra; nop
asm void func_0011AF80(int a0) {
    cfc2 $3, $28
    addiu $2, $zero, 0x1
    .word 0x14820003
    nop
    .word 0x10000004
    ori $3, $3, 0x800
    lui $2, 0xFFFF
    ori $2, $2, 0xF7FF
    and $3, $3, $2
    ctc2 $3, $28
    jr $ra
    nop
}
