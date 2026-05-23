// EE asm-void: leaf uses EE/COP2/VU instructions; reproduced verbatim
// (mwccmips accepts Sony's syntax directly).
asm void func_001C6120(void) {
    lui $v0, (0xFFFF7FFF >> 16)
    andi $a1, $a1, 0xFFFF
    ori $v0, $v0, (0xFFFF7FFF & 0xFFFF)
    and $a1, $a1, $v0
    sll $v0, $a1, 2
    addu $v0, $v0, $a0
    lw $v0, 0x4($v0)
    sra $v0, $v0, 2
    sll $v0, $v0, 2
    addu $a0, $a0, $v0
    jr $ra
    paddub $v0, $a0, $zero
}
