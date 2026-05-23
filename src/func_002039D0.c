asm void func_002039D0(char *a0, int **a1) {
    lui $at, (0x50008 >> 16)
    addu $at, $a0, $at
    lw $v1, (0x50008 & 0xFFFF)($at)
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    lw $v0, (0x50004 & 0xFFFF)($at)
    subu $v0, $v1, $v0
    .word 0x10400006
    nop
    lui $at, (0x50000 >> 16)
    addu $at, $a0, $at
    lw $v1, (0x50000 & 0xFFFF)($at)
    addu $v1, $a0, $v1
    sw $v1, 0x0($a1)
    jr $ra
    nop
}
