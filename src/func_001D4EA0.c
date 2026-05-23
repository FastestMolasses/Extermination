// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D4E20(void);

asm void func_001D4EA0(void) {
    j func_001D4E20
    paddub $a0, $zero, $zero
}
