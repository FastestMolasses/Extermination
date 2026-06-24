// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Initializes the 8-float scratch matrix at 0x700038A0 to one of two row
// layouts selected by the byte flag at p+0x2F1 (the only difference is
// element [4]: -10.0 when the flag is zero, +10.0 otherwise). Then runs the
// scratch block through func_001026A0 twice (building D_700038C0/D_700038D0
// from p+0xD0) and tests it with func_0019AFE0(...,7); if the result has bit
// 1 or 2 set, calls func_00178910(p, 1) and returns its value, else 0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 89.6% (it spills the trailing 1.0 store to a fresh register at the
// branch merge, a regalloc divergence). Keeping the [7]=1.0 store inside both
// arms holds the 1.0 register live across the merge. objdiff 100.0 vs expected.
extern void func_001026A0(void *dst, void *src, void *m);
extern int func_00178910(void *p, int flag);
extern int func_0019AFE0(void *p, void *a, void *b, int n);
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

int func_001784E0(char *p) {
    float *m = (float *)0x700038A0;
    if (*(unsigned char *)(p + 0x2F1) == 0) {
        m[0] = 0.0f;
        m[1] = 20.0f;
        m[2] = -9.0f;
        m[3] = 1.0f;
        m[4] = -10.0f;
        m[5] = 20.0f;
        m[6] = -9.0f;
        m[7] = 1.0f;
    } else {
        m[0] = 0.0f;
        m[1] = 20.0f;
        m[2] = -9.0f;
        m[3] = 1.0f;
        m[4] = 10.0f;
        m[5] = 20.0f;
        m[6] = -9.0f;
        m[7] = 1.0f;
    }
    func_001026A0(D_700038C0, p + 0xD0, D_700038A0);
    func_001026A0(D_700038D0, p + 0xD0, D_700038B0);
    if (func_0019AFE0(p, D_700038C0, D_700038D0, 7) & 6) {
        return func_00178910(p, 1);
    }
    return 0;
}
