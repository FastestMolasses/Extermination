// lui $v0,(0x10003810>>16); addiu $a0,$zero,0x1; ori $v0,$v0,0x3810; lui $v1,(0x10003820>>16);
// sw $a0,0x0($v0); ori $v1,$v1,0x3820; addiu $v0,$zero,0x6; jr $ra; sw $v0,0x0($v1)
asm void func_0011B5E0(void) {
    lui $2, 0x1000
    addiu $4, $zero, 0x1
    ori $2, $2, 0x3810
    lui $3, 0x1000
    sw $4, 0x0($2)
    ori $3, $3, 0x3820
    addiu $2, $zero, 0x6
    jr $ra
    sw $2, 0x0($3)
}
