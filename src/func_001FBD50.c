// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Thin wrapper: resolves a pair of int outputs from func_001FBF50 (passing the
// caller's float param in f12 plus the constant 4096.0f in f13), then forwards
// them to func_001FB9F0. arg0 is passed through to func_001FBF50; arg1 (saved in
// s0 across the call) becomes func_001FB9F0's first arg, arg2 is the int param to
// func_001FBF50. Returns func_001FB9F0's result on success, -1 if func_001FBF50
// fails (returns 0).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the 991202
// build only reaches 68% (clean-store / delay-slot divergence). 2.3.3 is byte-
// identical. Verified objdiff 100% vs build/expected/func_001FBD50.o.
extern int func_001FBF50(void *a0, int *a1, int *a2, int a3, float f12, float f13);
extern int func_001FB9F0(int a0, int a1, int a2, int a3);

int func_001FBD50(void *a0, int a1, int a2, float f12) {
    int local1;
    int local2;

    if (func_001FBF50(a0, &local1, &local2, a2, f12, 4096.0f) != 0) {
        return func_001FB9F0(a1, 0x1000, local1, local2);
    }
    return -1;
}
