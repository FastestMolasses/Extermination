// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00162DB0(int, int, int, int);
extern void func_00163B40(int, int, int, int);
extern void func_00182DF0(int, int, int, int);
extern void func_001837A0(int, int, int, int);
extern void func_001837B0(int, int, int, int);
extern void func_001838B0(int, int, int, int);
extern void func_00183910(int, int, int, int);

asm void func_0015B530(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    lui $at, (0x70003B8D >> 16)
    lbu $v1, (0x70003B8D & 0xFFFF)($at)
    .word 0x14600005
    nop
    jal func_00182DF0
    nop
    .word 0x1000002c
    lq $ra, 0x0($sp)
    lbu $a1, 0x5($a0)
    addiu $v1, $zero, 0xC
    .word 0x10a30025
    nop
    addiu $v1, $zero, 0x8
    .word 0x10a3001e
    nop
    addiu $v1, $zero, 0x5
    .word 0x10a30017
    nop
    addiu $v1, $zero, 0x1
    .word 0x10a30010
    nop
    addiu $v1, $zero, 0x17
    .word 0x10a30009
    nop
    .word 0x10a00003
    nop
    .word 0x10000017
    nop
    jal func_001837A0
    nop
    .word 0x10000013
    nop
    jal func_00183910
    nop
    .word 0x1000000f
    nop
    jal func_001837B0
    nop
    .word 0x1000000b
    nop
    jal func_00162DB0
    nop
    .word 0x10000007
    nop
    jal func_00163B40
    nop
    .word 0x10000003
    nop
    jal func_001838B0
    nop
    lq $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
