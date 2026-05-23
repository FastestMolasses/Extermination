// lw $v1,0x40($a0); li $v0,1; sw $v0,0xF8($v1); jr $ra; sd $a1,0xF0($v1)
asm void func_00109C28(void) {
    lw $3, 0x40($4)
    addiu $2, $zero, 0x1
    sw $2, 0xF8($3)
    jr $ra
    sd $5, 0xF0($3)
}
