// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_00182870(int, int, int, int);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, int);
extern void func_0021D490(int, int, int, int);

asm void func_0021E650(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a0, $zero
    lbu $a0, 0x7($a0)
    addiu $v1, $zero, 0x4
    .word 0x10830059
    nop
    addiu $v1, $zero, 0x3
    .word 0x1083003f
    nop
    addiu $v1, $zero, 0x2
    .word 0x1083002b
    nop
    addiu $a3, $zero, 0x1
    .word 0x10870018
    nop
    .word 0x10800003
    nop
    .word 0x1000005e
    lq $ra, 0x10($sp)
    lwc1 $f1, 0x3C($s0)
    lui $v1, (0x432D0000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000055
    nop
    addiu $v0, $a0, 0x1
    addiu $a1, $zero, 0xC0
    addiu $a2, $zero, 0x5
    sb $v0, 0x7($s0)
    jal func_001B61C0
    paddub $a0, $zero, $zero
    addiu $a1, $zero, 0x1
    jal func_00182870
    paddub $a0, $s0, $zero
    .word 0x1000004a
    nop
    lwc1 $f1, 0x3C($s0)
    lui $v1, (0x42D20000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000042
    nop
    addiu $v0, $a0, 0x1
    addiu $a1, $zero, 0xC0
    addiu $a2, $zero, 0x5
    sb $v0, 0x7($s0)
    jal func_001B61C0
    paddub $a0, $zero, $zero
    .word 0x1000003a
    nop
    lwc1 $f1, 0x3C($s0)
    lui $v1, (0x42AA0000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000032
    nop
    addiu $v0, $a0, 0x1
    addiu $a1, $zero, 0xC0
    addiu $a2, $zero, 0x5
    addiu $a3, $zero, 0x1
    sb $v0, 0x7($s0)
    jal func_001B61C0
    paddub $a0, $zero, $zero
    .word 0x10000029
    nop
    lwc1 $f1, 0x3C($s0)
    lui $v1, (0x42700000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x45000021
    nop
    addiu $v0, $a0, 0x1
    addiu $a1, $zero, 0xC0
    addiu $a2, $zero, 0x5
    addiu $a3, $zero, 0x1
    sb $v0, 0x7($s0)
    jal func_001B61C0
    paddub $a0, $zero, $zero
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x156
    paddub $a0, $s0, $zero
    jal func_001FBD50
    paddub $a2, $zero, $zero
    .word 0x10000012
    nop
    lwc1 $f1, 0x3C($s0)
    lui $v1, (0x42340000 >> 16)
    mtc1 $v1, $f0
    nop
    c.le.s $f1, $f0
    nop
    .word 0x4500000a
    nop
    addiu $v0, $a0, 0x1
    addiu $a1, $zero, 0xD0
    addiu $a2, $zero, 0xA
    addiu $a3, $zero, 0x1
    sb $v0, 0x7($s0)
    jal func_001B61C0
    paddub $a0, $zero, $zero
    jal func_0021D490
    paddub $a0, $s0, $zero
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
