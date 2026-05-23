// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00125848(int, int, int, int);
extern void func_00126EC8(int, int, int, int);
extern void func_00126F20(int, int, int, int);
extern void func_00126F88(int, int, int, int);
extern void func_001274B0(int, int, int, int);
extern void func_00127650(int, int, int, int);

asm void func_00125758(void) {
    addiu $sp, $sp, -0x40
    daddu $a1, $zero, $zero
    sd $s1, 0x10($sp)
    daddu $s1, $a0, $zero
    sd $ra, 0x30($sp)
    sd $s2, 0x20($sp)
    jal func_001274B0
    sd $s0, 0x0($sp)
    .word 0x0440002c
    daddu $v0, $zero, $zero
    ori $a1, $zero, 0xF7C0
    dsll32 $a1, $a1, 14
    jal func_00126F88
    daddu $a0, $s1, $zero
    jal func_00127650
    daddu $a0, $v0, $zero
    dsll32 $s0, $v0, 0
    .word 0x06000005
    andi $v0, $s0, 0x1
    jal func_00125848
    daddu $a0, $s0, $zero
    .word 0x10000008
    daddu $a0, $s1, $zero
    dsrl $a0, $s0, 1
    jal func_00125848
    or $a0, $v0, $a0
    daddu $a0, $v0, $zero
    jal func_00126EC8
    daddu $a1, $a0, $zero
    daddu $a0, $s1, $zero
    jal func_00126F20
    daddu $a1, $v0, $zero
    daddu $s2, $zero, $zero
    daddu $s1, $v0, $zero
    daddu $a1, $s2, $zero
    jal func_001274B0
    daddu $a0, $s1, $zero
    .word 0x04410009
    daddu $a0, $s2, $zero
    jal func_00126F20
    daddu $a1, $s1, $zero
    jal func_00127650
    daddu $a0, $v0, $zero
    dsll32 $v0, $v0, 0
    dsrl32 $v0, $v0, 0
    .word 0x10000006
    dsubu $s0, $s0, $v0
    jal func_00127650
    daddu $a0, $s1, $zero
    dsll32 $v0, $v0, 0
    dsrl32 $v0, $v0, 0
    daddu $s0, $s0, $v0
    daddu $v0, $s0, $zero
    ld $ra, 0x30($sp)
    ld $s2, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x40
}
