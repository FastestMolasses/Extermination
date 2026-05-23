// Tail-call / arg-shuffle wrapper — `asm void` with extern decls.
extern void func_001D7FA0(void);

asm void func_001D80E0(void) {
    lui $v0, (0xBF800000 >> 16)
    mtc1 $v0, $f13
    lui $v0, (0x3F19999A >> 16)
    ori $v0, $v0, (0x3F19999A & 0xFFFF)
    mtc1 $v0, $f12
    nop
    j func_001D7FA0
    paddub $a2, $zero, $zero
}
