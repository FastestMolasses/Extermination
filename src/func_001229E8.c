// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_001226E8(int, int, int, int);
extern void func_00127230(int, int, int, int);

asm void func_001229E8(void) {
    addiu $sp, $sp, -0x50
    sd $s1, 0x20($sp)
    sd $s0, 0x10($sp)
    daddu $s1, $a1, $zero
    daddu $s0, $a0, $zero
    sd $s2, 0x30($sp)
    sd $ra, 0x40($sp)
    jal func_001226E8
    daddu $a1, $sp, $zero
    daddu $s2, $v0, $zero
    daddu $a0, $s1, $zero
    jal func_001226E8
    ori $a1, $sp, 0x4
    lw $v1, 0x10($s0)
    daddu $a2, $v0, $zero
    lw $a0, 0x10($s1)
    lw $v0, 0x0($sp)
    lw $a1, 0x4($sp)
    subu $v1, $v1, $a0
    sll $v1, $v1, 5
    subu $v0, $v0, $a1
    addu $v0, $v0, $v1
    .word 0x18400009
    sll $v0, $v0, 20
    dsra32 $v1, $s2, 0
    addu $v1, $v1, $v0
    lui $a0, (0xFFFF0000 >> 16)
    dsrl32 $a0, $a0, 0
    dsll32 $v1, $v1, 0
    and $s2, $s2, $a0
    .word 0x10000008
    or $s2, $s2, $v1
    dsra32 $v1, $a2, 0
    subu $v1, $v1, $v0
    lui $a0, (0xFFFF0000 >> 16)
    dsrl32 $a0, $a0, 0
    dsll32 $v1, $v1, 0
    and $a2, $a2, $a0
    or $a2, $a2, $v1
    daddu $a0, $s2, $zero
    jal func_00127230
    daddu $a1, $a2, $zero
    ld $ra, 0x40($sp)
    ld $s2, 0x30($sp)
    ld $s1, 0x20($sp)
    ld $s0, 0x10($sp)
    jr $ra
    addiu $sp, $sp, 0x50
}
