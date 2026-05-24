// Hybrid: branches/j as .word, jal with extern decls
extern void func_00108660(int, int, int, int);

asm void func_00108790(void) {
    daddu      $a3, $a0, $zero
    sll        $a1, $a1, 3
    ld         $v0, 0x18($a3)
    lw         $a2, 0x8($a3)
    daddu      $v0, $a1, $v0
    lw         $v1, 0x24($a3)
    dsll       $a0, $v0, 29
    dsra32     $a0, $a0, 0
    sd         $zero, 0x0($a3)
    addu       $a2, $a2, $a0
    sw         $zero, 0x10($a3)
    sltu       $v1, $a2, $v1
    sd         $v0, 0x18($a3)
    .word 0x14600004
    sw        $a2, 0xC($a3)
    lw         $v0, 0x28($a3)
    subu       $v0, $a2, $v0
    sw         $v0, 0xC($a3)
    daddu      $a0, $a3, $zero
    j         func_00108660
    daddu     $a1, $zero, $zero
}
