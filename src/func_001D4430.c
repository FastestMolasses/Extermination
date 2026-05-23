// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D42E0(void);

asm void func_001D4430(void) {
    paddub $a2, $a1, $zero
    paddub $a1, $a0, $zero
    j func_001D42E0
    addiu $a0, $zero, 0x3
}
