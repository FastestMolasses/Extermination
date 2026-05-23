// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D4B80(void);

asm void func_001D4C20(void) {
    paddub $a1, $a0, $zero
    j func_001D4B80
    paddub $a0, $zero, $zero
}
