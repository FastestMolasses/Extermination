// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D3E40(void);

asm void func_001D3F50(void) {
    paddub $a1, $a0, $zero
    j func_001D3E40
    paddub $a0, $zero, $zero
}
