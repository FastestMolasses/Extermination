// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001FB9F0(void);

asm void func_0020CD60(void) {
    addiu $a1, $zero, 0x1000
    addiu $a0, $zero, 0x1
    paddub $a2, $a1, $zero
    j func_001FB9F0
    paddub $a3, $a1, $zero
}
