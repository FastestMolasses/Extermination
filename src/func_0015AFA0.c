// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0015AC00(int, int, int, int);
extern void func_0015AE20(int, int, int, int);
extern void func_001AFC10(int, int, int, int);
extern void func_001B1190(int, int, int, int);

asm void func_0015AFA0(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    lbu $v1, 0x4($a0)
    paddub $s0, $a0, $zero
    addiu $v0, $zero, 0x3
    .word 0x10620013
    addiu $a1, $s0, 0x1F0
    addiu $v0, $zero, 0x2
    .word 0x10620010
    nop
    addiu $v0, $zero, 0x1
    .word 0x10620009
    nop
    .word 0x10600003
    nop
    .word 0x1000000a
    lbu $a0, 0x9A($s0)
    jal func_0015AC00
    nop
    .word 0x1000000b
    lq $ra, 0x10($sp)
    jal func_0015AE20
    nop
    .word 0x10000006
    nop
    lbu $a0, 0x9A($s0)
    jal func_001B1190
    nop
    jal func_001AFC10
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
