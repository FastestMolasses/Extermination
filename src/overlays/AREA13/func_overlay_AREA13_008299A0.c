// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_15AC00(int, int, int, int);
extern void func_15AE20(int, int, int, int);
extern void func_1AFC10(int, int, int, int);
extern void func_1B1190(int, int, int, int);

asm void func_overlay_AREA13_008299A0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $a2, 0x4($a0)
    paddub $s0, $a0, $zero
    addiu $v1, $zero, 0x3
    .word 0x10c3001e
    addiu $a1, $s0, 0x1F0
    addiu $v1, $zero, 0x2
    .word 0x10c3001b
    nop
    addiu $v1, $zero, 0x1
    .word 0x10c30010
    nop
    .word 0x10c00003
    nop
    .word 0x10000015
    lbu $a0, 0x9A($s0)
    jal func_15AC00
    nop
    .word 0x14400015
    nop
    addiu $v1, $zero, 0x1
    sw $v1, 0x2EC($s0)
    lwc1 $f0, 0xB4($s0)
    addiu $v1, $zero, 0x71C
    swc1 $f0, 0x2E8($s0)
    .word 0x1000000e
    sh $v1, 0x28($s0)
    lui $at, (0x810774 >> 16)
    lbu $v1, (0x810774 & 0xFFFF)($at)
    .word 0x1460000a
    nop
    jal func_15AE20
    nop
    .word 0x10000007
    lq $ra, 0x10($sp)
    lbu $a0, 0x9A($s0)
    jal func_1B1190
    nop
    jal func_1AFC10
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
