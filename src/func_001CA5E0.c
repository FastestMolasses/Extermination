// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001CA5F0(void);

asm void func_001CA5E0(void) {
    sw $a1, 0x44($a0)
    j func_001CA5F0
    paddub $a1, $a2, $zero
}
