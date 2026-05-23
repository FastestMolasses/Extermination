// cfc2 $v1,$29; addiu $a0,$zero,1; andi $v0,$v1,0xF; bne $v0,$a0,.Lec;
// andi $v1,$v1,0xF00 (delay); jr $ra; addiu $v0,$zero,1 (delay);
// .Lec: daddu $v0,$a0,$zero; xori $v1,$v1,0x100; jr $ra; movn $v0,$zero,$v1 (delay)
asm int func_0011ADD0(int a0) {
    cfc2 $3, $29
    addiu $4, $zero, 0x1
    andi $2, $3, 0xF
    .word 0x14440003
    andi $3, $3, 0xF00
    jr $ra
    addiu $2, $zero, 0x1
    daddu $2, $4, $zero
    xori $3, $3, 0x100
    jr $ra
    movn $2, $zero, $3
}
