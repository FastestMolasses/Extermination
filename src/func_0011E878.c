// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_00120F40(int, int, int, int);
extern void func_00121A28(int, int, int, int);

asm void func_0011E878(void) {
    mult       $zero, $a1, $a2
    addiu      $sp, $sp, -0x20
    .word 0xffb00000
    .word 0xffbf0010
    mflo       $a1
    jal        func_00120F40
    nop
    daddu      $s0, $v0, $zero
    .word 0x56000003
    .word 0x8e03fffc
    .word 0x10000020
    daddu     $v0, $zero, $zero
    .word 0x3c02ffff
    .word 0x3442fffc
    and        $v1, $v1, $v0
    addiu      $a2, $v1, -0x4
    sltiu      $v0, $a2, 0x25
    .word 0x10400015
    sltiu     $v0, $a2, 0x14
    .word 0x1440000e
    daddu     $a0, $s0, $zero
    .word 0xae000000
    addiu      $a0, $s0, 0x8
    sltiu      $v0, $a2, 0x1C
    .word 0x14400009
    .word 0xae000004
    sltiu      $v0, $a2, 0x24
    .word 0xae000008
    .word 0xae00000c
    .word 0x14400004
    addiu     $a0, $s0, 0x10
    .word 0xae000010
    addiu      $a0, $s0, 0x18
    .word 0xae000014
    .word 0xac800000
    addiu      $a0, $a0, 0x4
    .word 0xac800004
    .word 0x10000004
    .word 0xac800000
    daddu      $a0, $s0, $zero
    jal        func_00121A28
    daddu     $a1, $zero, $zero
    daddu      $v0, $s0, $zero
    .word 0xdfbf0010
    .word 0xdfb00000
    jr         $ra
    addiu     $sp, $sp, 0x20
}
