// Simple nonleaf asm void
extern void func_00205A50(int, int, int, int);

asm void func_00205B00(void) {
    dsll32     $v0, $a3, 0
    dsrl32     $v0, $v0, 0
    dsll       $12, $v0, 20
    dsll32     $v0, $8, 0
    dsrl32     $v0, $v0, 0
    dsll       $8, $v0, 26
    dsll32     $v0, $9, 0
    dsll32     $9, $a1, 0
    dsrl32     $v0, $v0, 0
    dsll32     $a1, $a2, 0
    dsll       $a3, $v0, 30
    dsrl32     $a1, $a1, 0
    dsll32     $v0, $10, 0
    addiu      $sp, $sp, -0x10
    dsrl32     $v0, $v0, 0
    dsll32     $v1, $v0, 2
    sq         $ra, 0x0($sp)
    dsll32     $v0, $11, 0
    dsrl32     $v0, $v0, 0
    lwu        $a2, 0x20($sp)
    dsrl32     $9, $9, 0
    dsll       $a1, $a1, 14
    or         $a1, $9, $a1
    or         $a1, $12, $a1
    or         $a1, $8, $a1
    or         $a1, $a3, $a1
    lwu        $8, 0x10($sp)
    or         $v1, $v1, $a1
    dsll32     $v0, $v0, 3
    or         $9, $v0, $v1
    lwu        $a3, 0x18($sp)
    lwu        $v1, 0x28($sp)
    lwu        $v0, 0x30($sp)
    dsll32     $8, $8, 5
    or         $8, $8, $9
    dsll32     $a2, $a2, 23
    dsll32     $a3, $a3, 19
    or         $a3, $a3, $8
    or         $a2, $a2, $a3
    dsll32     $v1, $v1, 24
    addiu      $a1, $zero, 0x6
    or         $v1, $v1, $a2
    dsll32     $v0, $v0, 29
    jal        func_00205A50
    or        $a2, $v0, $v1
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
