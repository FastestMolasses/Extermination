// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001CA5E0(void);

asm void func_001CA6E0(void) {
    j func_001CA5E0
    paddub $a2, $zero, $zero
}
