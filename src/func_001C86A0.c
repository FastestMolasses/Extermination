asm void func_001C86A0(void) {
    lui $v1, (0x3F800000 >> 16)
    mtc1 $v1, $0
    lui $at, (0x70003A3C >> 16)
    div.s $0, $0, $12
    swc1 $0, (0x70003A3C & 0xFFFF)($at)
    lwc1 $2, 0x0($5)
    lwc1 $1, 0x0($6)
    .word 0x3c017000
    lwc1 $0, (0x70003A3C & 0xFFFF)($at)
    sub.s $1, $2, $1
    .word 0x3c017000
    mul.s $0, $0, $1
    swc1 $0, 0x0($4)
    lwc1 $2, 0x4($5)
    lwc1 $1, 0x4($6)
    lwc1 $0, (0x70003A3C & 0xFFFF)($at)
    sub.s $1, $2, $1
    .word 0x3c017000
    mul.s $0, $0, $1
    swc1 $0, 0x4($4)
    lwc1 $2, 0x8($5)
    lwc1 $1, 0x8($6)
    lwc1 $0, (0x70003A3C & 0xFFFF)($at)
    sub.s $1, $2, $1
    mul.s $0, $0, $1
    jr $ra
    swc1 $0, 0x8($4)
    nop
}
