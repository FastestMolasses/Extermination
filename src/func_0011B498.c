// lui $v0,(0x10003020>>16); ori $v0,$v0,0x3020; lw $v0,0x0($v0); andi $v1,$v0,0x8;
// beqz $v1,.L; andi $v1,$v0,0xFC0 (delay); jr $ra; addiu $v0,$zero,1 (delay)
// .L: addiu $v0,$zero,2; jr $ra; movz $v0,$zero,$v1 (delay)
asm int func_0011B498(void) {
    lui $2, 0x1000
    ori $2, $2, 0x3020
    lw $2, 0x0($2)
    andi $3, $2, 0x8
    .word 0x10600003
    andi $3, $2, 0xFC0
    .word 0x03E00008
    addiu $2, $zero, 0x1
    addiu $2, $zero, 0x2
    jr $ra
    .word 0x0003100A
}
