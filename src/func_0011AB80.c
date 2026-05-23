// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011A9C0(int, int, int, int);

asm void func_0011AB80(void) {
    addiu $sp, $sp, -0x20
    sd $s0, 0x0($sp)
    sd $ra, 0x10($sp)
    jal func_0011A9C0
    daddu $s0, $a0, $zero
    .word 0x1440002e
    daddu $v0, $zero, $zero
    cfc2.ni $v1, $vi16
    addiu $v0, $zero, 0x1
    sw $v1, 0x200($s0)
    cfc2.ni $v1, $vi17
    sw $v1, 0x204($s0)
    cfc2.ni $v1, $vi18
    sw $v1, 0x208($s0)
    cfc2.ni $v1, $vi20
    sw $v1, 0x20C($s0)
    cfc2.ni $v1, $vi21
    sw $v1, 0x210($s0)
    cfc2.ni $v1, $vi22
    sw $v1, 0x214($s0)
    cfc2.ni $v1, $vi0
    sh $v1, 0x218($s0)
    cfc2.ni $v1, $vi1
    sh $v1, 0x21A($s0)
    cfc2.ni $v1, $vi2
    sh $v1, 0x21C($s0)
    cfc2.ni $v1, $vi3
    sh $v1, 0x21E($s0)
    cfc2.ni $v1, $vi4
    sh $v1, 0x220($s0)
    cfc2.ni $v1, $vi5
    sh $v1, 0x222($s0)
    cfc2.ni $v1, $vi6
    sh $v1, 0x224($s0)
    cfc2.ni $v1, $vi7
    sh $v1, 0x226($s0)
    cfc2.ni $v1, $vi8
    sh $v1, 0x228($s0)
    cfc2.ni $v1, $vi9
    sh $v1, 0x22A($s0)
    cfc2.ni $v1, $vi10
    sh $v1, 0x22C($s0)
    cfc2.ni $v1, $vi11
    sh $v1, 0x22E($s0)
    cfc2.ni $v1, $vi12
    sh $v1, 0x230($s0)
    cfc2.ni $v1, $vi13
    sh $v1, 0x232($s0)
    cfc2.ni $v1, $vi14
    sh $v1, 0x234($s0)
    cfc2.ni $v1, $vi15
    sh $v1, 0x236($s0)
    ld $ra, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
