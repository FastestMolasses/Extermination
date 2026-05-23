// Matched via asm void: loop has 2 nops and uses bne $v0,$v1 comparison
// that cannot be exactly reproduced from C with mwcc at -O4,p.
asm void func_00101B80(unsigned char *a0, int a1) {
    beqz $a1, b80_end
    addiu $v0, $a1, -1
    addiu $v1, $zero, -1
    nop
b80_loop:
    sb $zero, 0($a0)
    addiu $v0, $v0, -1
    addiu $a0, $a0, 1
    nop
    nop
    bne $v0, $v1, b80_loop
    nop
b80_end:
    jr $ra
    nop
}
