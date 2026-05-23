// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void ReferThreadStatus(int, int, int, int);
extern void func_0010ECC8(int, int, int, int);
extern void func_0010ED18(int, int, int, int);

asm void func_0010EEF8(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    .word 0x10000004
    daddu $s0, $a0, $zero
    nop
    jal func_0010ED18
    nop
    jal func_0010ECC8
    daddu $a0, $s0, $zero
    .word 0x1440fffb
    daddu $a0, $v0, $zero
    jal ReferThreadStatus
    nop
    .word 0x1000fff9
    nop
}
