// Hybrid: branches/j as .word, jal with extern decls
extern void func_00205A50(int, int, int, int);

asm void func_00205A90(void) {
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsll       $12, $v0, 5
    dsll32     $v0, $8, 0
    dsrl32     $v0, $v0, 0
    dsll       $8, $v0, 6
    dsll32     $v0, $9, 0
    dsll32     $9, $a1, 0
    dsll32     $a1, $a2, 0
    dsrl32     $v0, $v0, 0
    dsrl32     $a1, $a1, 0
    dsll       $a3, $v0, 9
    dsll32     $v0, $10, 0
    dsrl32     $v0, $v0, 0
    dsll       $v1, $v0, 19
    dsll32     $v0, $11, 0
    dsrl32     $v0, $v0, 0
    dsrl32     $9, $9, 0
    dsll       $a1, $a1, 2
    or         $a1, $9, $a1
    or         $a1, $12, $a1
    or         $a1, $8, $a1
    or         $a1, $a3, $a1
    or         $v1, $v1, $a1
    dsll32     $v0, $v0, 0
    addiu      $a1, $zero, 0x14
    j         func_00205A50
    or        $a2, $v0, $v1
}
