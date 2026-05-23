// Multi-call non-leaf — asm void with extern decls for every callee.
extern void func_001157F0(int, int, int, int);

asm void func_0011A608(void) {
    dsrl $v0, $a0, 24
    lui $v1, (0xFFFFFF >> 16)
    ori $v1, $v1, (0xFFFFFF & 0xFFFF)
    and $a0, $a0, $v1
    sll $a3, $a1, 16
    addiu $sp, $sp, -0x10
    or $a3, $a3, $a2
    and $v0, $v0, $v1
    dsll32 $a1, $a0, 0
    dsra32 $a1, $a1, 0
    sd $ra, 0x0($sp)
    dsll32 $a2, $v0, 0
    dsra32 $a2, $a2, 0
    jal func_001157F0
    addiu $a0, $zero, 0x40
    ld $ra, 0x0($sp)
    daddu $v0, $zero, $zero
    jr $ra
    addiu $sp, $sp, 0x10
}
