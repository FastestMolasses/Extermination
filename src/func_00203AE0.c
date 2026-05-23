// EE asm-void: leaf uses EE/COP2/VU instructions; reproduced verbatim
// (mwccmips accepts Sony's syntax directly).
asm void func_00203AE0(void) {
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    lw $v0, (0x50004 & 0xFFFF)($at)
    slt $at, $a1, $v0
    movz $a1, $v0, $at
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    lw $v1, (0x50004 & 0xFFFF)($at)
    paddub $v0, $a1, $zero
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    subu $v1, $v1, $a1
    jr $ra
    sw $v1, (0x50004 & 0xFFFF)($at)
}
