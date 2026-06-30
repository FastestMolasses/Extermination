// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Builds and casts a downward floor/feeler probe. func_001031E0 initializes the
// scratch block D_700038A0 from the actor's transform at arg0+0xB0, then the
// probe's Y component (0x700038A4) is extended by 40.0. func_0019A570 casts it
// (origin=arg0+0xB0, &probe, count 4, flag 0). On a hit, the contact object
// pointer at 0x700031D0 is read; if its type byte (+0x1A) is 0x3C the contact
// point (object +0x34/+0x38/+0x3C) is copied into the actor at +0x2E0/+0x2E4/
// +0x2E8 and 1 is returned. Otherwise (no hit, or wrong type) returns 0.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 only reaches
// 86.63% (FP-half coloring + the swc1 delay-slot-fill). Idiom: writing the Y
// extension as `*(float*)0x700038A4 += 40.0f` (read-modify-write) makes mwcc
// keep the store ahead of the jal and fill the delay slot with the `0` arg
// (paddub a3) like CW, and picks the matching FP register half. The trailing
// `return 0;` is shared (single epilogue) rather than duplicated per arm.
extern void func_001031E0(void *dst, void *src);
extern int func_0019A570(void *a, void *b, int n, int m);
extern char D_700038A0[];

int func_0016BB80(char *arg0) {
    char *o;

    func_001031E0(&D_700038A0, arg0 + 0xB0);
    *(float *)0x700038A4 += 40.0f;
    if (func_0019A570(arg0 + 0xB0, &D_700038A0, 4, 0) != 0) {
        o = *(char **)0x700031D0;
        if (*(unsigned char *)(o + 0x1A) == 0x3C) {
            *(float *)(arg0 + 0x2E0) = *(float *)(o + 0x34);
            *(float *)(arg0 + 0x2E4) = *(float *)(*(char **)0x700031D0 + 0x38);
            *(float *)(arg0 + 0x2E8) = *(float *)(*(char **)0x700031D0 + 0x3C);
            return 1;
        }
    }
    return 0;
}
