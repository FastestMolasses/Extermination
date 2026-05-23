// VU tri-point clip test: load 3 vecs from a0/a1/a2, check clip flags
asm int func_00103290(void *a0, void *a1, void *a2) {
    .word 0x4BE0012C
    lui $2, 0x4580
    dsll $2, $2, 16
    ori $2, $2, 0x4580
    dsll $2, $2, 16
    lqc2 $vf6, 0x0($4)
    lqc2 $vf8, 0x0($5)
    lqc2 $vf9, 0x0($6)
    qmtc2.ni $2, $vf7
    ctc2.ni $zero, $vi16
    .word 0x4BA4316C
    .word 0x4B86396C
    .word 0x4BA4416C
    .word 0x4B88396C
    .word 0x4BA4496C
    .word 0x4B89396C
    .word 0x4A0002FF
    .word 0x4A0002FF
    .word 0x4A0002FF
    .word 0x4A0002FF
    .word 0x4A0002FF
    cfc2.ni $2, $vi16
    jr $ra
    andi $2, $2, 0xC0
}
