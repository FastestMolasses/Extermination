// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00100268(int, int, int, int);

asm void func_00100550(void) {
    addiu      $sp, $sp, -0x20
    .word 0xffb00000
    .word 0xffbf0010
    jal        func_00100268
    daddu     $s0, $a0, $zero
    .word 0x84430006
    addiu      $v0, $zero, 0x1
    .word 0x14620011
    .word 0xde040000
    .word 0x3c021200
    .word 0x3c031200
    .word 0x3c061200
    .word 0xfc440000
    .word 0x34630070
    .word 0x34c60080
    .word 0x3c041200
    .word 0xde050010
    .word 0x348400c0
    .word 0xfc650000
    .word 0xde020018
    .word 0xfcc20000
    .word 0xde030020
    .word 0xfc830000
    .word 0x10000014
    .word 0xdfbf0010
    .word 0x3c031200
    .word 0x3c021200
    .word 0x3c061200
    .word 0xfc640000
    .word 0x34420020
    .word 0x34c60090
    .word 0x3c051200
    .word 0xde030008
    .word 0x34a500a0
    .word 0x3c041200
    .word 0xfc430000
    .word 0x348400e0
    .word 0xde020010
    .word 0xfcc20000
    .word 0xde030018
    .word 0xfca30000
    .word 0xde020020
    .word 0xfc820000
    .word 0xdfbf0010
    .word 0xdfb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
