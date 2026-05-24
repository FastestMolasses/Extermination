// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_1026A0(int, int, int, int);
extern void func_102760(int, int, int, int);
extern void func_1028D0(int, int, int, int);
extern void func_1EFD20(int, int, int, int);
extern void func_1EFD90(int, int, int, int);

asm void func_overlay_AREA01_008238C0(void) {
    addiu $sp, $sp, -0x30
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    lbu $a0, 0x4($a0)
    addiu $v1, $zero, 0x3
    .word 0x10830024
    nop
    addiu $v1, $zero, 0x2
    .word 0x10830021
    nop
    addiu $v1, $zero, 0x1
    .word 0x10830005
    nop
    .word 0x1080001c
    nop
    .word 0x1000001b
    lq $ra, 0x10($sp)
    sw $zero, 0x20($sp)
    addiu $a0, $sp, 0x20
    sw $zero, 0x24($sp)
    lui $v0, (0x3F800000 >> 16)
    sw $v0, 0x28($sp)
    addiu $a1, $s0, 0xD0
    sw $v0, 0x2C($sp)
    jal func_1026A0
    paddub $a2, $a0, $zero
    addiu $a0, $sp, 0x20
    addiu $a2, $s0, 0x100
    jal func_1028D0
    paddub $a1, $a0, $zero
    addiu $a0, $sp, 0x20
    jal func_102760
    paddub $a1, $a0, $zero
    lui $v0, (0x80000003 >> 16)
    addiu $a1, $s0, 0x100
    addiu $a2, $sp, 0x20
    jal func_1EFD90
    ori $a0, $v0, (0x80000003 & 0xFFFF)
    lui $v0, (0x80000024 >> 16)
    addiu $a1, $s0, 0x100
    jal func_1EFD20
    ori $a0, $v0, (0x80000024 & 0xFFFF)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
