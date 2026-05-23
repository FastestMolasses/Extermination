// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011E878(int, int, int, int);

asm void func_00121AF8(void) {
    addiu $sp, $sp, -0x30
    sd $s1, 0x10($sp)
    sd $s0, 0x0($sp)
    sd $ra, 0x20($sp)
    daddu $s0, $a0, $zero
    lw $v1, 0x4C($s0)
    .word 0x14600007
    daddu $s1, $a1, $zero
    addiu $a1, $zero, 0x4
    jal func_0011E878
    addiu $a2, $zero, 0x10
    .word 0x10400013
    sw $v0, 0x4C($s0)
    daddu $v1, $v0, $zero
    sll $v0, $s1, 2
    addu $a0, $v0, $v1
    lw $v1, 0x0($a0)
    .word 0x10600004
    addiu $v0, $zero, 0x1
    lw $v0, 0x0($v1)
    .word 0x1000000d
    sw $v0, 0x0($a0)
    daddu $a0, $s0, $zero
    sllv $s0, $v0, $s1
    addiu $a1, $zero, 0x1
    sll $a2, $s0, 2
    jal func_0011E878
    addiu $a2, $a2, 0x14
    daddu $v1, $v0, $zero
    .word 0x54600003
    sw $s1, 0x4($v1)
    .word 0x10000005
    daddu $v0, $zero, $zero
    sw $s0, 0x8($v1)
    sw $zero, 0x10($v1)
    daddu $v0, $v1, $zero
    sw $zero, 0xC($v1)
    ld $ra, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
