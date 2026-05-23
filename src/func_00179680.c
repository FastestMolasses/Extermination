// Matched via asm void: mwcc schedules li v1,11 before sb zero,6(a0)
// which cannot be produced by C alone (instruction scheduling).
asm void func_00179680(unsigned char *a0) {
    addiu $v1, $zero, 5
    sb $v1, 5($a0)
    addiu $v1, $zero, 11
    sb $zero, 6($a0)
    sb $v1, 496($a0)
    addiu $v1, $zero, 2
    sb $v1, 607($a0)
    lbu $v1, 566($a0)
    beqz $v1, ret
    nop
    lbu $v1, 565($a0)
    andi $v1, $v1, 1
    sb $v1, 565($a0)
    sb $zero, 566($a0)
ret:
    jr $ra
    nop
}
