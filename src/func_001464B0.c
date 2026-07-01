// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// AI approach-decision gate: builds a 45-degree cone transform at the scratch
// block D_700038A0 (func_001B2B10 identity+seed, func_001028B8 VU0 add with
// self+0xB0) and tests it against the target via func_001B2BF0 (bit0/bit2
// result flags). bit0 set: either bit2 clear -> distance-vs-20 gate on
// D_700038D0 (accept 0 / reject 3), or bit2 set -> if not within 15.0f of the
// stored distance and the global actor's type byte (*0x700031D0 + 0x1A) != 0x46,
// rebuild the scratch block with a 15.5f-wide cone and re-test via
// func_0019AD00 (reject 2 on failure), then fall into the shared "commit"
// tail: field 0x5C <= 10.0f gates a 45-degree func_001B1560 check against the
// player (D_00810360) before calling func_00146740 to finalize. bit0 clear
// path: bit2 clear -> reject 3; bit2 set -> same commit tail as above.
extern void func_001028B8(void *a, void *b, void *c);
extern int func_00146740(char *a, char *b);
extern int func_0019AD00(char *a, void *b, int n);
extern int func_001B1560(char *a, void *b, float c);
extern void func_001B2B10(void *a, void *b, void *c);
extern int func_001B2BF0(char *a, void *b, void *c, float d);
extern char D_00810360[];
extern char D_700038A0[];
extern char D_700038D0[];

int func_001464B0(char *arg0, char *arg1) {
    int v0;

    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x40400000;
    *(int *)0x700038A8 = 0x42060000;
    *(int *)0x700038AC = 0x3F800000;
    func_001B2B10(arg0, D_700038A0, D_700038A0);
    func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
    v0 = func_001B2BF0(arg0, D_700038A0, D_700038D0, 3.14159274f * 0.25f);
    if (!(v0 & 1)) {
        if (!(v0 & 4)) {
            return 3;
        }
        if (*(float *)(arg1 + 0x5C) <= 10.0f) {
            if (func_001B1560(arg0, D_00810360, 3.14159274f * 0.25f) != 0) {
                return 3;
            }
        }
        return func_00146740(arg0, arg1);
    }
    if (v0 & 4) {
        if (!(15.0f + *(float *)(arg0 + 0xB4) < *(float *)0x700038D0)
                && *(unsigned char *)(*(char **)0x700031D0 + 0x1A) != 0x46) {
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0x41780000;
            *(int *)0x700038A8 = 0x42060000;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(arg0, D_700038A0, D_700038A0);
            func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
            if (func_0019AD00(arg0, D_700038A0, 7) == 0) {
                return 2;
            }
        }
        if (*(float *)(arg1 + 0x5C) <= 10.0f) {
            if (func_001B1560(arg0, D_00810360, 3.14159274f * 0.25f) != 0) {
                return 3;
            }
        }
        return func_00146740(arg0, arg1);
    }
    if (*(float *)(arg0 + 0xB4) - 20.0f >= *(float *)0x700038D0) {
        return 3;
    }
    v0 = 0;
    return v0;
}
