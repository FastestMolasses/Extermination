asm void func_001B7670(void) {
    lw $2, 0x8($6)
    addiu $3, $0, 0x1
    .word 0x1043000C
    nop
    .word 0x10400003
    nop
    .word 0x1000000F
    .word 0x24020001
    lui $at, (0x70003B91 >> 16)
    lbu $2, (0x70003B91 & 0xFFFF)($at)
    .word 0x1443000A
    nop
    .word 0x3C017000
    .word 0x10000007
    sb $0, (0x70003B91 & 0xFFFF)($at)
    .word 0x3C017000
    lbu $2, (0x70003B91 & 0xFFFF)($at)
    .word 0x14400003
    nop
    .word 0x3C017000
    sb $3, (0x70003B91 & 0xFFFF)($at)
    .word 0x24020001
    jr $ra
    nop
}
