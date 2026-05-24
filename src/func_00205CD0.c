// Hybrid: branches/j as .word, jal with extern decls
extern void func_00205A50(int, int, int, int);

asm void func_00205CD0(void) {
    dsll32     $v1, $a2, 0
    dsll32     $a1, $a1, 0
    dsrl32     $v1, $v1, 0
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsrl32     $a1, $a1, 0
    dsll       $v1, $v1, 16
    or         $v1, $a1, $v1
    dsll32     $v0, $v0, 0
    addiu      $a1, $zero, 0x5
    j         func_00205A50
    or        $a2, $v0, $v1
}
