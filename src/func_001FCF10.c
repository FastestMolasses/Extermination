// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001FCB90(void);

asm void func_001FCF10(void) {
    addiu $a0, $zero, 0x10E
    addiu $a1, $zero, 0xCC
    addiu $a2, $zero, 0x5
    j func_001FCB90
    paddub $a3, $zero, $zero
}
