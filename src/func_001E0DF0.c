// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Teardown helper. If func_001D2910(0x20) succeeds, free two owned handles off
// the global context D_00275670 (fields +0x1D8 and +0x1E8) via func_001D21B0
// when non-null, then grab a node via func_001D2E00(0) and, if non-null and
// func_001D2910(5) reports clear, release it with func_001D21B0(node).
//
// Matched with mwcc 2.3.3 (991202 reaches 77.1%). Key (idiom-20): the +0x1E8
// release is `if (h) func_001D21B0(h);` immediately followed by
// `node = func_001D2E00(0)` -- giving func_001D2E00 its literal 0 argument lets
// mwcc emit the `beql h,zero / paddub a0,zero,zero` (a0=0 = that argument) it
// uses in the likely-branch delay slot. D_00275670 is small-data
// (sdatathreshold 8 -> %gp_rel). objdiff 100.0 vs build/expected.
extern int func_001D2910(int);
extern void func_001D21B0(int);
extern int func_001D2E00(int);
extern char *D_00275670;

void func_001E0DF0(void) {
    int a0;
    int s0;

    if (func_001D2910(0x20) != 0) {
        a0 = *(int *)(D_00275670 + 0x1D8);
        if (a0 != 0) {
            func_001D21B0(a0);
        }
        a0 = *(int *)(D_00275670 + 0x1E8);
        if (a0 != 0) {
            func_001D21B0(a0);
        }
        s0 = func_001D2E00(0);
        if (s0 != 0) {
            if (func_001D2910(5) == 0) {
                func_001D21B0(s0);
            }
        }
    }
}
