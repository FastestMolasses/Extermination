// Hybrid: branches/j as .word, jal with extern decls
extern void func_0021BA70(int, int, int, int);

asm void func_0021BA80(void) {
    dsll32     $v0, $a1, 0
    dsra32     $v0, $v0, 0
    dsll       $v1, $v0, 8
    dsll32     $v0, $a2, 0
    dsll32     $a0, $a0, 0
    dsra32     $v0, $v0, 0
    dsra32     $a0, $a0, 0
    dsll       $v0, $v0, 16
    or         $v1, $a0, $v1
    j         func_0021BA70
    or        $a0, $v0, $v1
}
