// sll $a1,$a1,1; sll $a2,$a2,2; or $v0,$a0,$a1; lui $v1,(0x10003820>>16); or $v0,$v0,$a2; ori $v1,$v1,0x3820; jr $ra; sw $v0,0x0($v1)
asm void func_0011B6B0(int a0, int a1, int a2) {
    sll $5, $5, 1
    sll $6, $6, 2
    or $2, $4, $5
    lui $3, 0x1000
    or $2, $2, $6
    ori $3, $3, 0x3820
    jr $ra
    sw $2, 0x0($3)
}
