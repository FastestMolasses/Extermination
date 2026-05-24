// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00122B58(int, int, int, int);

asm void func_00102600(void) {
    addiu $sp, $sp, -0x10
    lui   $a0, %hi(D_0026B260)
    sd    $ra, 0x0($sp)
    jal   func_00122B58
    addiu $a0, $a0, %lo(D_0026B260)
    ld    $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr    $ra
    addiu $sp, $sp, 0x10
}
