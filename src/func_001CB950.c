// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001CB5F0(int, int, int, int);

asm void func_001CB950(void) {
    addiu $sp, $sp, -0x20
    sq $ra, 0x10($sp)
    sq $s0, 0x0($sp)
    paddub $s0, $a2, $zero
    jal func_001CB5F0
    addiu $a2, $zero, 0x3
    lui $v1, (0x50000002 >> 16)
    sq $zero, 0x0($v0)
    ori $v1, $v1, (0x50000002 & 0xFFFF)
    sw $v1, 0xC($v0)
    lui $v1, (0x10000000 >> 16)
    dsll32 $a0, $v1, 0
    ori $v1, $zero, 0x8001
    or $v1, $v1, $a0
    sd $v1, 0x10($v0)
    addiu $v1, $zero, 0xE
    sd $v1, 0x18($v0)
    addiu $v1, $zero, 0x6
    sd $s0, 0x20($v0)
    sd $v1, 0x28($v0)
    lq $ra, 0x10($sp)
    lq $s0, 0x0($sp)
    jr $ra
    addiu $sp, $sp, 0x20
}
