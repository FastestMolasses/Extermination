// Tail-call wrapper (uses $t0/$t1 in original; rewritten with
// numeric register names because mwcc inline asm rejects $tN).
extern void func_00207F80(void);

asm void func_002256E0(void) {
    lui $v0, (0x80CE6000 >> 16)
    addiu $a0, $zero, 0x1
    addiu $a1, $zero, 0x77E0
    addiu $a3, $zero, 0x78A0
    ori $a2, $zero, 0x8550
    ori $8, $zero, 0x85B0
    j func_00207F80
    ori $9, $v0, (0x80CE6000 & 0xFFFF)
}
