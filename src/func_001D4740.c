// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D4650(void);

asm void func_001D4740(void) {
    paddub $a1, $a0, $zero
    j func_001D4650
    paddub $a0, $zero, $zero
}
