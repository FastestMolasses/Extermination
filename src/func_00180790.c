// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Runs func_00180420(), copies the matrix at arg0+0x290 into the scratch block
// D_700038A0 (func_001031E0), and subtracts 4.5 from the scratch float at
// 0x700038A4. Then func_0019AB20 tests arg0 / D_700038A0 / arg0+0x280 (n=6);
// on a nonzero return it bails out returning 2. Otherwise it rewrites the
// scratch float to (*(D_00275B40+4))->[0xC4] - 6.0 and returns whether
// func_00180300(arg0, scratch, arg0[0xD]) was nonzero (1) or zero (0).
//
// D_00275B40 is a gp-relative pointer cell (needs -sdatathreshold 4).
extern void func_001031E0(void *dst, char *src);
extern int func_00180300(char *p, void *m, unsigned char b);
extern void func_00180420(void);
extern int func_0019AB20(char *p, void *m, char *q, int n);
extern int D_00275B40;
extern char D_700038A0[];

int func_00180790(char *arg0) {
    func_00180420();
    func_001031E0(D_700038A0, arg0 + 0x290);
    *(float *)0x700038A4 = *(float *)0x700038A4 - 4.5f;
    if (func_0019AB20(arg0, D_700038A0, arg0 + 0x280, 6) != 0) {
        return 2;
    }
    *(float *)0x700038A4 = *(float *)(*(int *)(D_00275B40 + 4) + 0xC4) - 6.0f;
    return (func_00180300(arg0, D_700038A0, *(unsigned char *)(arg0 + 0xD)) != 0) ? 1 : 0;
}
