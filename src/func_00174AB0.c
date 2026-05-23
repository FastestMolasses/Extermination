// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001749A0(void);

asm void func_00174AB0(void) {
    mtc1 $zero, $f12
    addiu $a2, $zero, 0x1
    j func_001749A0
    paddub $a1, $zero, $zero
}
