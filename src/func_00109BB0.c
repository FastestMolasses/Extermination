// lw $v0,0x40($a0); sd $zero,0x78($v0); jr $ra; sw $zero,0x70($v0)
asm void func_00109BB0(void) {
    lw $2, 0x40($4)
    sd $zero, 0x78($2)
    jr $ra
    sw $zero, 0x70($2)
}
