// Hybrid: branches/j as .word, jal with extern decls
extern void func_0010CEE8(int, int, int, int);
extern void func_0010CEE8(int, int, int, int);

asm void func_0010CFD0(void) {
    addiu      $sp, $sp, -0x10
    addiu      $v0, $zero, 0xA
    .word 0x14820007
    sd        $ra, 0x0($sp)
    jal        func_0010CEE8
    addiu     $a0, $zero, 0xD
    ld         $ra, 0x0($sp)
    addiu      $a0, $zero, 0xA
    j         func_0010CEE8
    addiu     $sp, $sp, 0x10
    ld         $ra, 0x0($sp)
    j         func_0010CEE8
    addiu     $sp, $sp, 0x10
}
