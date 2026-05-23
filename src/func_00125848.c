// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00126EC8(int, int, int, int);
extern void func_00126F88(int, int, int, int);
extern void func_00127500(int, int, int, int);

asm void func_00125848(void) {
    addiu $sp, $sp, -0x30
    sd $s0, 0x0($sp)
    daddu $s0, $a0, $zero
    sd $s1, 0x10($sp)
    ori $s1, $zero, 0x81E0
    dsll32 $s1, $s1, 15
    dsra32 $a0, $s0, 0
    sd $ra, 0x20($sp)
    jal func_00127500
    nop
    daddu $a1, $s1, $zero
    jal func_00126F88
    daddu $a0, $v0, $zero
    daddu $a1, $s1, $zero
    jal func_00126F88
    daddu $a0, $v0, $zero
    daddu $s1, $v0, $zero
    lui $v0, (0xFFFF0000 >> 16)
    dsrl32 $v0, $v0, 0
    and $s0, $s0, $v0
    dsll32 $s0, $s0, 0
    dsra32 $s0, $s0, 0
    jal func_00127500
    daddu $a0, $s0, $zero
    .word 0x06010006
    daddu $a0, $s1, $zero
    ori $a1, $zero, 0x83E0
    dsll32 $a1, $a1, 15
    jal func_00126EC8
    daddu $a0, $v0, $zero
    daddu $a0, $s1, $zero
    jal func_00126EC8
    daddu $a1, $v0, $zero
    ld $ra, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
