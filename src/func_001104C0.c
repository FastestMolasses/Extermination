// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void RFU116(int, int, int, int);
extern void SetVSyncFlag(int, int, int, int);
extern void func_0010CE28(int, int, int, int);

asm void func_001104C0(void) {
    addiu $sp, $sp, -0x10
    sd $ra, 0x0($sp)
    jal RFU116
    addiu $a0, $zero, 0x4
    lui $v1, (0x40000 >> 16)
    and $v0, $v0, $v1
    .word 0x10400007
    addiu $a0, $zero, 0x4
    jal SetVSyncFlag
    lui $a1, (0x40000 >> 16)
    jal func_0010CE28
    nop
    .word 0x10000002
    addiu $v0, $zero, 0x1
    daddu $v0, $zero, $zero
    ld $ra, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x10
}
