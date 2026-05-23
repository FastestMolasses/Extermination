// Matched via asm int: target uses addiu v1,zero,-0xE1 (not andi), bne (not beq),
// and paddub v0,zero,zero as zero-move, and branch structure differs from C output.
asm int func_0017D040(unsigned char *a0) {
    lbu $a1, 2($a0)
    addiu $v1, $zero, -0xE1
    addiu $v0, $zero, 4
    and $v1, $a1, $v1
    bne $v1, $v0, d040_end
    paddub $v0, $zero, $zero
    lbu $v1, 3($a0)
    addiu $v0, $zero, 2
    bne $v1, $v0, d040_pad
    nop
    b d040_end
    addiu $v0, $zero, 1
d040_pad:
    paddub $v0, $zero, $zero
d040_end:
    jr $ra
    nop
}
