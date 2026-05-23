// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001F4BF0(int, int, int, int);
extern void func_0021B9A0(int, int, int, int);

asm void func_001F4CC0(void) {
    addiu $sp, $sp, -0x30
    mtc1 $zero, $f12
    sq $ra, 0x20($sp)
    sq $s1, 0x10($sp)
    lui $v0, (0x47C35000 >> 16)
    paddub $s1, $a0, $zero
    ori $v0, $v0, (0x47C35000 & 0xFFFF)
    sq $s0, 0x0($sp)
    mtc1 $v0, $f13
    addiu $a0, $zero, 0x2
    jal func_0021B9A0
    paddub $s0, $a1, $zero
    lui $v0, (0x49742400 >> 16)
    ori $v0, $v0, (0x49742400 & 0xFFFF)
    mtc1 $zero, $f12
    mtc1 $v0, $f13
    jal func_0021B9A0
    addiu $a0, $zero, 0x3
    paddub $a0, $s1, $zero
    jal func_001F4BF0
    paddub $a1, $s0, $zero
    mtc1 $zero, $f12
    addiu $a0, $zero, 0x1
    jal func_0021B9A0
    mov.s $f13, $f12
    lq $ra, 0x20($sp)
    lq $s1, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x30
}
