// Simple nonleaf asm void
extern void func_00205A50(int, int, int, int);

asm void func_00205BC0(void) {
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsll       $12, $v0, 4
    dsll32     $v0, $8, 0
    dsrl32     $v0, $v0, 0
    dsll       $8, $v0, 5
    dsll32     $v0, $9, 0
    dsll32     $9, $a1, 0
    dsrl32     $v0, $v0, 0
    dsll32     $a1, $a2, 0
    dsll       $a3, $v0, 6
    dsrl32     $a1, $a1, 0
    dsll32     $v0, $10, 0
    dsrl32     $v0, $v0, 0
    dsll       $v1, $v0, 7
    dsll32     $v0, $11, 0
    addiu      $sp, $sp, -0x10
    dsrl32     $v0, $v0, 0
    dsrl32     $9, $9, 0
    dsll       $a1, $a1, 3
    or         $a1, $9, $a1
    or         $a1, $12, $a1
    or         $a1, $8, $a1
    or         $a1, $a3, $a1
    or         $v1, $v1, $a1
    dsll       $v0, $v0, 8
    sq         $ra, 0x0($sp)
    or         $a2, $v0, $v1
    lwu        $v1, 0x10($sp)
    lwu        $v0, 0x18($sp)
    paddub     $a1, $zero, $zero
    dsll       $v1, $v1, 9
    or         $v1, $v1, $a2
    dsll       $v0, $v0, 10
    jal        func_00205A50
    or        $a2, $v0, $v1
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
