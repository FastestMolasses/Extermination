// Simple nonleaf asm void
extern void func_001AF5C0(int, int, int, int);
extern void func_001AF690(int, int, int, int);
extern void func_001AF710(int, int, int, int);
extern void func_001AF8E0(int, int, int, int);
extern void func_001D0660(int, int, int, int);

asm void func_001AFCA0(void) {
    addiu      $sp, $sp, -0x10
    sq         $ra, 0x0($sp)
    jal        func_001AF5C0
    nop
    jal        func_001AF690
    nop
    jal        func_001AF710
    nop
    jal        func_001AF8E0
    nop
    jal        func_001D0660
    nop
    lui        $at, (0x700031F4 >> 16)
    sw         $zero, (0x700031F4 & 0xFFFF)($at)
    lq         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x10
}
