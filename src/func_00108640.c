// ld $v0,0($a0); li $v1,64; subu $v1,$v1,$a1; dsrlv $v0,$v0,$v1;
// dsll32 $v0,$v0,0; jr $ra; dsra32 $v0,$v0,0
asm long long func_00108640(long long *a0, int a1) {
    ld $2, 0x0($4)
    addiu $3, $zero, 0x40
    subu $3, $3, $5
    dsrlv $2, $2, $3
    dsll32 $2, $2, 0
    jr $ra
    dsra32 $2, $2, 0
}
