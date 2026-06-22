// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Two-stage gate on an object (arg0). First runs func_001FE9A0(1, &arg0[0x50],
// &arg0[0x54]); if it returns 0, bail returning 0. Otherwise runs the predicate
// func_001FED40(arg0->ptr[0x48], arg0->ptr[0x4C]); if that returns 0 the result
// becomes 1 (the "completed/ok" flag, held in s0). Finally if the global
// D_00275C58 is set, override the return with 2; else return the flag (0 or 1).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the lone
// residual under 991202 is wall #13 (clean-store delay-slot nop fill). The 2.3.3
// build leaves that nop, so this readable C is byte-identical. Verified objdiff
// 100.0% vs build/expected/func_00228070.o.
extern int func_001FE9A0(int a, char *b, char *c);
extern int func_001FED40(int a, int b);
extern int D_00275C58;

int func_00228070(char *arg0) {
    int v;

    v = 0;
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    if (func_001FED40(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C)) == 0) {
        v = 1;
    }
    if (D_00275C58 != 0) {
        return 2;
    }
    return v;
}
