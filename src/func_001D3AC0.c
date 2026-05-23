// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D3A30(void);

asm void func_001D3AC0(void) {
    paddub $a1, $a0, $zero
    j func_001D3A30
    addiu $a0, $zero, 0x3
}
