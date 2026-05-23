// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001157F0(int, int, int, int);

asm void func_0011A658(void) {
    lui $v0, (0xFFFFFF >> 16)
    ori $v0, $v0, (0xFFFFFF & 0xFFFF)
    dsrl $a2, $a0, 24
    and $a0, $a0, $v0
    daddu $a3, $a1, $zero
    addiu $sp, $sp, -0x10
    and $a2, $a2, $v0
    dsll32 $a1, $a0, 0
    dsra32 $a1, $a1, 0
    sd $ra, 0x0($sp)
    dsll32 $a2, $a2, 0
    dsra32 $a2, $a2, 0
    jal func_001157F0
    addiu $a0, $zero, 0x41
    ld $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
