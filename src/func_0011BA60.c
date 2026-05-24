// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011BA10(int, int, int, int);

asm void func_0011BA60(void) {
    addiu      $sp, $sp, -0x90
    .word 0x3c021000
    .word 0xffb60060
    .word 0x34423c40
    .word 0xffbf0080
    daddu      $s6, $a0, $zero
    .word 0xffb50050
    .word 0xffb40040
    .word 0xffb30030
    .word 0xffb20020
    .word 0xffb10010
    .word 0xffb00000
    .word 0xffb70070
    jal        func_0011BA10
    .word 0x8c570000
    sltiu      $v0, $v0, 0x2
    .word 0x14400003
    .word 0x3c021000
    .word 0x10000058
    daddu     $v0, $zero, $zero
    .word 0x3c031000
    .word 0x34423d00
    .word 0x34633d10
    .word 0x8c450000
    .word 0x3c041000
    .word 0x34843d20
    .word 0x3c061000
    .word 0xaec50000
    .word 0x34c63d30
    .word 0x3c051000
    .word 0x3c071000
    .word 0x8c620000
    .word 0x34a53d40
    .word 0x34e73d50
    .word 0x3c0c1000
    .word 0xaec20004
    .word 0x358c3d60
    .word 0x3c131000
    .word 0x3c141000
    .word 0x8c820000
    .word 0x36733d70
    .word 0x36943c70
    .word 0x3c111000
    .word 0xaec20008
    .word 0x36313c80
    .word 0x3c101000
    .word 0x3c121000
    .word 0x8cc20000
    .word 0x36103c00
    .word 0x36523cd0
    .word 0x3c061000
    .word 0xaec2000c
    .word 0x34c63c90
    .word 0x3c081000
    .word 0x3c0e1000
    .word 0x8ca20000
    .word 0x35083ca0
    .word 0x35ce3cb0
    .word 0x3c0b1000
    .word 0xaec20010
    .word 0x356b3ce0
    .word 0x3c091000
    .word 0x3c0f1000
    .word 0x8ce20000
    .word 0x35293cc0
    .word 0x35ef3c30
    .word 0x3c0d1000
    .word 0xaec20014
    .word 0x35ad3c60
    .word 0x3c0a1000
    srl        $s5, $s7, 8
    .word 0x8d830000
    .word 0x354a3c20
    .word 0x3c0c1000
    addiu      $v0, $zero, 0x1
    .word 0xaec30018
    .word 0x358c3c50
    .word 0x8e630000
    .word 0xaec3001c
    .word 0x8e840000
    .word 0xaec40020
    .word 0x8e230000
    .word 0xaec30024
    .word 0x8e040000
    .word 0xaec40028
    .word 0x8e430000
    .word 0x8cc40000
    .word 0x8d050000
    .word 0x8dc60000
    .word 0x8d670000
    .word 0x8d280000
    .word 0xa6c3002c
    .word 0xa6c4002e
    .word 0xa6c50030
    .word 0xa6c60032
    .word 0xa6c70034
    .word 0xa6c80036
    .word 0x8de40000
    .word 0x8da50000
    .word 0x8d430000
    .word 0xa2d5003e
    .word 0xa2c3003c
    .word 0xa2d7003d
    .word 0xa6c40038
    .word 0x8d830000
    .word 0xa6c5003a
    .word 0xa2c3003f
    .word 0xdfbf0080
    .word 0xdfb70070
    .word 0xdfb60060
    .word 0xdfb50050
    .word 0xdfb40040
    .word 0xdfb30030
    .word 0xdfb20020
    .word 0xdfb10010
    .word 0xdfb00000
    jr         $ra
    addiu     $sp, $sp, 0x90
}
