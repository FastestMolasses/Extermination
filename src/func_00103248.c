// VU clip test: load vf7 from a0, build clipping config, check clip flags
asm int func_00103248(void *a0) {
    .word 0x4BE0012C
    lui $2, 0x4580
    dsll $2, $2, 16
    ori $2, $2, 0x4580
    dsll $2, $2, 16
    lqc2 $vf7, 0x0($4)
    qmtc2.ni $2, $vf6
    ctc2.ni $zero, $vi16
    .word 0x4BA4396C
    .word 0x4B87316C
    .word 0x4A0002FF
    .word 0x4A0002FF
    .word 0x4A0002FF
    .word 0x4A0002FF
    .word 0x4A0002FF
    cfc2.ni $2, $vi16
    jr $ra
    andi $2, $2, 0xC0
}
