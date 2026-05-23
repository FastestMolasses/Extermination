// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D4A90(void);

asm void func_001D4B10(void) {
    paddub $a1, $a0, $zero
    j func_001D4A90
    paddub $a0, $zero, $zero
}
