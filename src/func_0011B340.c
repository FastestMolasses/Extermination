// lui $v1,(0x10003000>>16); addiu $a0,$zero,0x8; ori $v1,$v1,0x3000; addiu $v0,$zero,0x1; jr $ra; sw $a0,0x0($v1)
asm int func_0011B340(void) {
    lui $3, 0x1000
    addiu $4, $zero, 0x8
    ori $3, $3, 0x3000
    addiu $2, $zero, 0x1
    jr $ra
    sw $4, 0x0($3)
}
