// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D66A0(void);

asm void func_00208AB0(void) {
    paddub $v0, $a1, $zero
    paddub $a1, $a0, $zero
    paddub $a3, $a2, $zero
    addiu $a0, $zero, 0x1
    j func_001D66A0
    paddub $a2, $v0, $zero
}
