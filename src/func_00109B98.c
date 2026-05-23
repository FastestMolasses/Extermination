// lw $v0,0x40($a0); li $v1,1; sd $a1,0x78($v0); jr $ra; sw $v1,0x70($v0)
asm void func_00109B98(void) {
    lw $2, 0x40($4)
    addiu $3, $zero, 0x1
    sd $5, 0x78($2)
    jr $ra
    sw $3, 0x70($2)
}
