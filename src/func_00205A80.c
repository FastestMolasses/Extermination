// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_00205A50(void);

asm void func_00205A80(void) {
    addiu $a1, $zero, 0x3F
    j func_00205A50
    paddub $a2, $zero, $zero
}
