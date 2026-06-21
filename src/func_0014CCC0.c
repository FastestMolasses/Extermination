// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets actor state byte +0x04 = 3 after calling func_001FBD50 with a float
// constant 300.0f in $f12, id 0x875 in $a1, and 0 in $a2 (the actor pointer
// arg0 is passed through in $a0 and preserved across the call in $s0).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the residual is the clean-store delay-slot wall. 2.3.3 is
// byte-identical. Verified objdiff 100.0 vs build/expected/func_0014CCC0.o.
extern void func_001FBD50(char *, int, int, float);

void func_0014CCC0(char *arg0) {
    func_001FBD50(arg0, 0x875, 0, 300.0f);
    *(char *)(arg0 + 4) = 3;
}
