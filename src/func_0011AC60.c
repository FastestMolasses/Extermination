// Hybrid asm void: real mnemonics where mwcc accepts them,
// .word for branch instructions (mwcc rejects PC-relative labels).
extern void func_0011A9C0(int, int, int, int);

asm void func_0011AC60(void) {
    addiu $sp, $sp, -0x30
    sd $s1, 0x10($sp)
    sd $s0, 0x0($sp)
    daddu $s1, $a0, $zero
    sd $ra, 0x20($sp)
    jal func_0011A9C0
    daddu $s0, $s1, $zero
    .word 0x1440004e
    daddu $v0, $zero, $zero
    sqc2 $vf0, 0x0($s0)
    sqc2 $vf1, 0x10($s0)
    sqc2 $vf2, 0x20($s0)
    sqc2 $vf3, 0x30($s0)
    sqc2 $vf4, 0x40($s0)
    sqc2 $vf5, 0x50($s0)
    sqc2 $vf6, 0x60($s0)
    sqc2 $vf7, 0x70($s0)
    sqc2 $vf8, 0x80($s0)
    sqc2 $vf9, 0x90($s0)
    sqc2 $vf10, 0xA0($s0)
    sqc2 $vf11, 0xB0($s0)
    sqc2 $vf12, 0xC0($s0)
    sqc2 $vf13, 0xD0($s0)
    sqc2 $vf14, 0xE0($s0)
    sqc2 $vf15, 0xF0($s0)
    sqc2 $vf16, 0x100($s0)
    sqc2 $vf17, 0x110($s0)
    sqc2 $vf18, 0x120($s0)
    sqc2 $vf19, 0x130($s0)
    sqc2 $vf20, 0x140($s0)
    sqc2 $vf21, 0x150($s0)
    sqc2 $vf22, 0x160($s0)
    sqc2 $vf23, 0x170($s0)
    sqc2 $vf24, 0x180($s0)
    sqc2 $vf25, 0x190($s0)
    sqc2 $vf26, 0x1A0($s0)
    sqc2 $vf27, 0x1B0($s0)
    sqc2 $vf28, 0x1C0($s0)
    sqc2 $vf29, 0x1D0($s0)
    sqc2 $vf30, 0x1E0($s0)
    sqc2 $vf31, 0x1F0($s0)
    lw $v0, 0x200($s1)
    ctc2.ni $v0, $vi16
    lw $v0, 0x204($s1)
    ctc2.ni $v0, $vi17
    lw $v0, 0x208($s1)
    ctc2.ni $v0, $vi18
    lw $v0, 0x20C($s1)
    ctc2.ni $v0, $vi20
    lw $v0, 0x210($s1)
    ctc2.ni $v0, $vi21
    lw $v0, 0x214($s1)
    ctc2.ni $v0, $vi22
    lhu $v0, 0x218($s1)
    ctc2.ni $v0, $vi0
    lhu $v0, 0x21A($s1)
    ctc2.ni $v0, $vi1
    lhu $v0, 0x21C($s1)
    ctc2.ni $v0, $vi2
    lhu $v0, 0x21E($s1)
    ctc2.ni $v0, $vi3
    lhu $v0, 0x220($s1)
    ctc2.ni $v0, $vi4
    lhu $v0, 0x222($s1)
    ctc2.ni $v0, $vi5
    lhu $v0, 0x224($s1)
    ctc2.ni $v0, $vi6
    lhu $v0, 0x226($s1)
    ctc2.ni $v0, $vi7
    lhu $v0, 0x228($s1)
    ctc2.ni $v0, $vi8
    lhu $v0, 0x22A($s1)
    ctc2.ni $v0, $vi9
    lhu $v0, 0x22C($s1)
    ctc2.ni $v0, $vi10
    lhu $v0, 0x22E($s1)
    ctc2.ni $v0, $vi11
    lhu $v0, 0x230($s1)
    ctc2.ni $v0, $vi12
    lhu $v0, 0x232($s1)
    ctc2.ni $v0, $vi13
    lhu $v0, 0x234($s1)
    ctc2.ni $v0, $vi14
    lhu $v0, 0x236($s1)
    ctc2.ni $v0, $vi15
    addiu $v0, $zero, 0x1
    ld $ra, 0x20($sp)
    ld $s1, 0x10($sp)
    ld $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
