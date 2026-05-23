asm void func_002039A0(char *a0) {
    lui $at, (0x50004 >> 16)
    addu $at, $a0, $at
    sw $zero, (0x50004 & 0xFFFF)($at)
    lui $at, (0x50000 >> 16)
    addu $at, $a0, $at
    sw $zero, (0x50000 & 0xFFFF)($at)
    lui $at, (0x50008 >> 16)
    addu $at, $a0, $at
    lui $v1, (0x50000 >> 16)
    jr $ra
    sw $v1, (0x50008 & 0xFFFF)($at)
}
