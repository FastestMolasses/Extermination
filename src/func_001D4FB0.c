// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D4F30(void);

asm void func_001D4FB0(void) {
    paddub $a1, $a0, $zero
    j func_001D4F30
    paddub $a0, $zero, $zero
}
