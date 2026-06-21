// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Thin two-call wrapper: calls func_001CB9B0(arg2) and feeds its return value
// as the 4th argument to func_001CB6B0(arg0, arg1, 8, ret). Returns void.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the 991202 build leaves an
// 11%-residual on argument/return register shuffling here; 2.3.3 is
// byte-identical. Verified objdiff 100% vs build/expected/func_001CB900.o.
extern int func_001CB9B0(int);
extern void func_001CB6B0(int, int, int, int);

void func_001CB900(int arg0, int arg1, int arg2) {
    int v0;

    v0 = func_001CB9B0(arg2);
    func_001CB6B0(arg0, arg1, 8, v0);
}
