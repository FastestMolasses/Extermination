// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// 2.3.1 codegen: dead `addiu a1,zero,0xEA` after the b+lq (taken-path const
// pre-hoisted into the bnez delay slot), and the third arg 0 emitted as
// `paddub a2,zero,zero`. mwcc 2.3.3 reproduces both; 991202 drops the dead insn.
// Condition written `== 0` (0xE8 case as the if-body) so mwcc keeps the bnez
// branch sense and the 0xEA const in the taken slot, matching the target.
extern void func_001749A0(unsigned char *, int, int);

void func_0017FD00(unsigned char *a0) {
    if (a0[0x2F1] == 0) {
        func_001749A0(a0, 0xE8, 0);
    } else {
        func_001749A0(a0, 0xEA, 0);
    }
}
