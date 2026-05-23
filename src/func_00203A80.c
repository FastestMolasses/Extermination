asm void func_00203A80(void) {
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    lw $v1, (0x50004 & 0xFFFF)($at)
    .word 0x10600010
    .word 0x3c010005
    .word 0x3c010005
    addu $at, $a0, $at
    lw $a2, (0x50008 & 0xFFFF)($at)
    lui $at, (0x50000 >> 16)
    addu $at, $a0, $at
    lw $v0, (0x50000 & 0xFFFF)($at)
    subu $v0, $v0, $v1
    addu $v0, $a2, $v0
    .word 0x0046001a
    nop
    nop
    mfhi $v0
    addu $v0, $a0, $v0
    sw $v0, 0x0($a1)
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    jr $ra
    lw $v0, (0x50004 & 0xFFFF)($at)
    nop
}
