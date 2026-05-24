// Simple nonleaf asm void
extern void func_001281C0(int, int, int, int);
extern void func_00207E40(int, int, int, int);

asm void func_00211310(void) {
    addiu      $sp, $sp, -0x20
    sq         $ra, 0x10($sp)
    sq         $s0, 0x0($sp)
    lwc1       $f2, 0x8($a0)
    lui        $v0, (0x43800000 >> 16)
    mtc1       $v0, $f1
    lui        $v1, (0x44E00000 >> 16)
    lui        $v0, (0x41000000 >> 16)
    mtc1       $v0, $f0
    nop
    add.s      $f1, $f1, $f2
    lui        $v0, (0x41800000 >> 16)
    paddub     $s0, $a0, $zero
    sub.s      $f2, $f1, $f0
    mtc1       $v1, $f1
    mtc1       $v0, $f0
    nop
    add.s      $f1, $f1, $f2
    jal        func_001281C0
    mul.s     $f12, $f0, $f1
    lwc1       $f1, 0x0($s0)
    lui        $a2, (0x40000000 >> 16)
    lui        $v1, (0x44F20000 >> 16)
    paddub     $s0, $v0, $zero
    lui        $v0, (0x43240000 >> 16)
    mtc1       $v0, $f2
    nop
    sub.s      $f1, $f2, $f1
    lui        $v0, (0x41000000 >> 16)
    mtc1       $v0, $f0
    nop
    sub.s      $f2, $f1, $f0
    lui        $v0, (0x41800000 >> 16)
    mtc1       $a2, $f1
    mtc1       $v1, $f0
    nop
    div.s      $f1, $f2, $f1
    add.s      $f1, $f0, $f1
    mtc1       $v0, $f0
    jal        func_001281C0
    mul.s     $f12, $f0, $f1
    paddub     $a2, $v0, $zero
    lui        $v0, (0x80808080 >> 16)
    .word 0x34498080
    lui        $v0, (0x20042E85 >> 16)
    .word 0x34422e85
    dsll32     $v1, $v0, 0
    addiu      $a3, $zero, 0x10
    lui        $v0, (0x113221D0 >> 16)
    .word 0x344221d0
    addiu      $a0, $zero, 0x1
    paddub     $a1, $s0, $zero
    paddub     $8, $a3, $zero
    jal        func_00207E40
    or        $10, $v0, $v1
    lq         $ra, 0x10($sp)
    lq         $s0, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x20
}
