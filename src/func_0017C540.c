// Matched via asm void: the beqz has a nop delay slot and else uses b
// with sb in its delay slot -- not reproducible from C with mwcc.
asm void func_0017C540(unsigned char *a0) {
    lbu $v1, 0x25C($a0)
    beqz $v1, else_branch
    nop
    addiu $v1, $zero, 1
    sb $v1, 5($a0)
    sb $zero, 6($a0)
    sb $v1, 0x1F0($a0)
    b end_branch
    sb $zero, 0x1F1($a0)
else_branch:
    sb $zero, 5($a0)
    sb $zero, 6($a0)
    sb $zero, 0x1F0($a0)
    sw $zero, 0x38($a0)
end_branch:
    addiu $v1, $zero, 1
    jr $ra
    sb $v1, 4($a0)
}
