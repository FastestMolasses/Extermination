// Hybrid: branches/j as .word, jal with extern decls
extern void func_00205A50(int, int, int, int);

asm void func_00205D00(void) {
    dsll32     $v0, $a3, 0
    dsll32     $a3, $a1, 0
    dsll32     $a1, $a2, 0
    dsrl32     $v0, $v0, 0
    dsrl32     $a1, $a1, 0
    dsll       $v1, $v0, 24
    dsll32     $v0, $8, 0
    dsrl32     $v0, $v0, 0
    dsrl32     $a3, $a3, 0
    dsll       $a1, $a1, 16
    or         $a1, $a3, $a1
    or         $v1, $v1, $a1
    dsll32     $v0, $v0, 0
    addiu      $a1, $zero, 0x4C
    j         func_00205A50
    or        $a2, $v0, $v1
}
