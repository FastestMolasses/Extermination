// Multi-call non-leaf — asm void with extern decls for every callee.
extern void float_to_int(int, int, int, int);
extern void func_001C6160(int, int, int, int);

asm void func_001C6190(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    jal func_001C6160
    paddub $s0, $a0, $zero
    lwc1 $f12, 0x3C($s0)
    jal float_to_int
    paddub $s0, $v0, $zero
    subu $v0, $s0, $v0
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
