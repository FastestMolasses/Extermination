// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void ReferThreadStatus(int, int, int, int);
extern void func_001152B0(int, int, int, int);
extern void func_001192D0(int, int, int, int);

asm void func_001FB0C0(void) {
    addiu $sp, $sp, -0x10
    sq $ra, 0x0($sp)
    jal ReferThreadStatus
    nop
    jal func_001192D0
    nop
    nop
    nop
    nop
    nop
    .word 0x1440fff9
    nop
    jal func_001152B0
    nop
    .word 0x1000fff3
    nop
}
