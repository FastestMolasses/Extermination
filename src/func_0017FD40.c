// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// 2.3.1 codegen: a dead a1 = 0xEB after the unconditional branch and its quadword
// load (taken-path const pre-hoisted into the bnez delay slot), and the third arg 0
// emitted as a 128-bit register clear. mwcc 2.3.3 reproduces both; 991202 drops the dead insn.
// Condition written `== 0` (0xE9 case as the if-body) so mwcc keeps the bnez
// branch sense and the 0xEB const in the taken slot, matching the target.
extern void func_001749A0(unsigned char *, int, int);

void func_0017FD40(unsigned char *a0) {
    if (a0[0x2F1] == 0) {
        func_001749A0(a0, 0xE9, 0);
    } else {
        func_001749A0(a0, 0xEB, 0);
    }
}
