// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Arms two sibling objects then conditionally fires a one-shot effect. Writes
// the 1.0f constant (0x3F800000, stored as a raw word) into field +0xC of both
// arg1 and arg2, runs func_0019B2C0(arg1, arg2, 6) and bails (return 0) if it
// fails. Then queries a scalar via func_0019A310(&sp4C); bails on failure.
// Finally, only when the queried value is strictly greater than the float
// parameter, invokes func_001028B8(arg0+0xB0, arg0+0xB0, &D_700031C0) and
// returns the (non-zero) func_0019B2C0 result; otherwise returns 0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 76.3% (it hoists the arg-setup out of the jal delay slot and moves the
// mov.s out of the call slot). Key idiom: the func_0019B2C0 result is assigned
// inside the `if` condition -- `if ((v0 = func_0019B2C0(...)) == 0)` -- so mwcc
// branches on the raw result in $v0 and fills the bnez delay slot with the
// `paddub s0, v0, zero` saved-register copy, matching CW exactly. objdiff 100.0
// vs build/expected/func_001B3390.o.
extern void func_001028B8(int a, int b, void *p);
extern int func_0019A310(float *p);
extern int func_0019B2C0(char *a, char *b, int n);
extern char D_700031C0;

int func_001B3390(int arg0, char *arg1, char *arg2, float fparg0) {
    float sp4C;
    int v0;

    *(int *)(arg2 + 0xC) = 0x3F800000;
    *(int *)(arg1 + 0xC) = 0x3F800000;
    if ((v0 = func_0019B2C0(arg1, arg2, 6)) == 0) {
        return 0;
    }
    if (func_0019A310(&sp4C) == 0) {
        return 0;
    }
    if (sp4C <= fparg0) {
        return 0;
    }
    func_001028B8(arg0 + 0xB0, arg0 + 0xB0, &D_700031C0);
    return v0;
}
