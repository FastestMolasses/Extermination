// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_15AC00(int, int, int, int);
extern void func_15AE20(int, int, int, int);
extern void func_1AFC10(int, int, int, int);
extern void func_1B1190(int, int, int, int);

asm void func_overlay_AREA21_00828310(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v1, 0x4($a0)
    paddub $s0, $a0, $zero
    addiu $v0, $zero, 0x3
    .word 0x10620020
    addiu $a1, $s0, 0x1F0
    addiu $v0, $zero, 0x2
    .word 0x1062001d
    nop
    addiu $v0, $zero, 0x1
    .word 0x10620009
    nop
    .word 0x10600003
    nop
    .word 0x10000017
    lbu $a0, 0x9A($s0)
    jal func_15AC00
    nop
    .word 0x10000018
    lq $ra, 0x10($sp)
    jal func_15AE20
    nop
    lui $at, (0x81080E >> 16)
    lbu $a0, (0x81080E & 0xFFFF)($at)
    addiu $v1, $zero, 0x10
    .word 0x14830010
    nop
    lh $v1, 0x28($s0)
    addiu $v1, $v1, 0x1
    sh $v1, 0x28($s0)
    lh $v1, 0x28($s0)
    slti $at, $v1, 0xFB
    .word 0x14200009
    nop
    addiu $v1, $zero, 0x3
    .word 0x10000006
    sb $v1, 0x4($s0)
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
