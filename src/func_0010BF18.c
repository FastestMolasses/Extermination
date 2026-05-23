// addiu $v0,$zero,0x2000; sd $zero,0x48($a1); sw $v0,0x4($a1); jr $ra; daddu $v0,$zero,$zero
asm int func_0010BF18(int a0, int *a1) {
    addiu $2, $zero, 0x2000
    sd $zero, 0x48($5)
    sw $2, 0x4($5)
    jr $ra
    daddu $2, $zero, $zero
}
