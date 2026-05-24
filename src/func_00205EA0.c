// Hybrid: branches/j as .word, jal with extern decls
extern void func_00205A50(int, int, int, int);

asm void func_00205EA0(void) {
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsll32     $v1, $v0, 24
    dsll32     $v0, $a2, 0
    dsrl32     $v0, $v0, 0
    dsll32     $v0, $v0, 16
    or         $v1, $v1, $v0
    dsll32     $v0, $a1, 0
    dsrl32     $v0, $v0, 0
    dsll32     $v0, $v0, 0
    addiu      $a1, $zero, 0x50
    j         func_00205A50
    or        $a2, $v0, $v1
}
