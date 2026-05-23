// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D3F60(void);

asm void func_001D40D0(void) {
    paddub $a1, $a0, $zero
    j func_001D3F60
    addiu $a0, $zero, 0x3
}
