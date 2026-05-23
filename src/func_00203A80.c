asm void func_00203A80(char *a0, int **a1) {
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    lw $v1, (0x50004 & 0xFFFF)($at)
    .word 0x10600010
    lui $at, (0x50004 >> 16)
    lui $at, (0x50008 >> 16)
    addu $at, $a0, $at
    lw $a2, (0x50008 & 0xFFFF)($at)
    lui $at, (0x50000 >> 16)
    addu $at, $a0, $at
    lw $v0, (0x50000 & 0xFFFF)($at)
    subu $v0, $v0, $v1
    addu $v0, $a2, $v0
    div $zero, $v0, $a2
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
