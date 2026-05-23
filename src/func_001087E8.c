// lw $v1,8($a0); sra $a1,$a1,3; lw $v0,0x24($a0); addu $v1,$v1,$a1;
// sltu $v0,$v1,$v0; bnez $v0,.L; nop; lw $v0,0x28($a0); subu $v1,$v1,$v0;
// .L: jr $ra; daddu $v0,$v1,$zero
asm int func_001087E8(int *a0, int a1) {
    lw $3, 0x8($4)
    sra $5, $5, 3
    lw $2, 0x24($4)
    addu $3, $3, $5
    sltu $2, $3, $2
    .word 0x14400003
    nop
    lw $2, 0x28($4)
    subu $3, $3, $2
    jr $ra
    daddu $2, $3, $zero
}
