// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001FBD50(void);

asm void func_00187DC0(void) {
    lui $v0, (0x43960000 >> 16)
    mtc1 $v0, $f12
    addiu $a1, $zero, 0x86
    j func_001FBD50
    paddub $a2, $zero, $zero
}
