// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D3BA0(void);

asm void func_001D3C30(void) {
    paddub $a1, $a0, $zero
    j func_001D3BA0
    paddub $a0, $zero, $zero
}
