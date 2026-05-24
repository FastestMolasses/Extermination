// Simple nonleaf asm void
extern void func_00205A50(int, int, int, int);

asm void func_00205D40(void) {
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsll       $12, $v0, 4
    dsll32     $v0, $8, 0
    dsrl32     $v0, $v0, 0
    dsll       $8, $v0, 12
    dsll32     $v0, $9, 0
    dsll32     $9, $a1, 0
    dsrl32     $v0, $v0, 0
    dsll32     $a1, $a2, 0
    dsll       $a3, $v0, 14
    dsrl32     $a1, $a1, 0
    dsll32     $v0, $10, 0
    dsrl32     $v0, $v0, 0
    dsll       $v1, $v0, 15
    dsll32     $v0, $11, 0
    addiu      $sp, $sp, -0x10
    dsrl32     $v0, $v0, 0
    dsrl32     $9, $9, 0
    dsll       $a1, $a1, 1
    or         $a1, $9, $a1
    or         $a1, $12, $a1
    or         $a1, $8, $a1
    or         $a1, $a3, $a1
    or         $v1, $v1, $a1
    dsll       $v0, $v0, 16
    sq         $ra, 0x0($sp)
    or         $v1, $v0, $v1
    lwu        $v0, 0x10($sp)
    addiu      $a1, $zero, 0x47
    dsll       $v0, $v0, 17
    jal        func_00205A50
    or        $a2, $v0, $v1
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
