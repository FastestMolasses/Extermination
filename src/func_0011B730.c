// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0011B6E0(int, int, int, int);

asm void func_0011B730(void) {
    addiu      $sp, $sp, -0x60
    .word 0x3c021000
    .word 0xffb30030
    .word 0x34423840
    .word 0xffbf0050
    daddu      $s3, $a0, $zero
    .word 0xffb20020
    .word 0xffb10010
    .word 0xffb00000
    .word 0xffb40040
    jal        func_0011B6E0
    .word 0x8c540000
    sltiu      $v0, $v0, 0x2
    .word 0x14400003
    .word 0x3c021000
    .word 0x10000048
    daddu     $v0, $zero, $zero
    .word 0x3c031000
    .word 0x34423900
    .word 0x34633910
    .word 0x8c450000
    .word 0x3c041000
    .word 0x34843920
    .word 0x3c061000
    .word 0xae650000
    .word 0x34c63930
    .word 0x3c051000
    .word 0x3c0f1000
    .word 0x8c620000
    .word 0x34a53940
    .word 0x35ef3950
    .word 0x3c101000
    .word 0xae620004
    .word 0x36103960
    .word 0x3c0e1000
    .word 0x3c111000
    .word 0x8c820000
    .word 0x35ce3970
    .word 0x36313870
    .word 0x3c0c1000
    .word 0xae620008
    .word 0x358c3880
    .word 0x3c0b1000
    .word 0x3c0d1000
    .word 0x8cc20000
    .word 0x356b3800
    .word 0x35ad38d0
    .word 0x3c061000
    .word 0xae62000c
    .word 0x34c63890
    .word 0x3c071000
    .word 0x3c091000
    .word 0x8ca20000
    .word 0x34e73830
    .word 0x35293860
    .word 0x3c081000
    .word 0xae620010
    .word 0x35083820
    srl        $s2, $s4, 8
    .word 0x3c0a1000
    .word 0x8de40000
    .word 0x354a3850
    addiu      $v0, $zero, 0x1
    .word 0xae640014
    .word 0x8e030000
    .word 0xae630018
    .word 0x8dc40000
    .word 0xae64001c
    .word 0x8e230000
    .word 0xae630020
    .word 0x8d840000
    .word 0xae640024
    .word 0x8d630000
    .word 0xae630028
    .word 0x8da40000
    .word 0x8cc30000
    .word 0x8ce50000
    .word 0x8d260000
    .word 0x8d070000
    .word 0xa663002e
    .word 0xa664002c
    .word 0xa6650030
    .word 0xa6660032
    .word 0xa2670034
    .word 0xa2740035
    .word 0xa2720036
    .word 0x8d430000
    .word 0xa2630037
    .word 0xdfbf0050
    .word 0xdfb40040
    .word 0xdfb30030
    .word 0xdfb20020
    .word 0xdfb10010
    .word 0xdfb00000
    jr         $ra
    addiu     $sp, $sp, 0x60
}
