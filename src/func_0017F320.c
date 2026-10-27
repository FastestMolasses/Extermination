// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.55% NEARMISS).
// Fixes read from the original instructions:
//  - The epilogue is `if (flags) return 0; return 1;`, not a `(flags == 0) ? 1 : 0`
//    expression. With the early-return spelling mwcc 2.3.3 hoists each following
//    constant load (the next probe's direction, and the final return value 1) into
//    the guard's delay slot without leaving a duplicate copy on the fall-through
//    path, as the original does. The earlier "scheduling wall" verdict was wrong.
//  - No scratchpad symbolization (idiom-32) is needed: the literal 0x700038xx
//    stores match as written.
//
// Runs four directional cone/ray probes off the entity's transform (arg0+0xD0) and
// returns whether NONE of them was blocked. Each probe fills two scratch vectors at
// 0x700038A0/0xB0 (cone params as float bit-patterns -- direction +/-3.0f, length
// 20.0f/19.0f, and constants 0/1.0f/6.5f/1.0f), transforms them through
// func_001026A0 into 0x700038C0/0xD0, then casts via func_0019AFE0(self, c0, d0, 6);
// a nonzero result ORs a hit bit into `flags` (probes 1&3 set bit 0, probes 2&4 set
// bit 1). The four probes are +3.0f/-3.0f direction at length 20.0f, then +3.0f/-3.0f
// at length 19.0f. Returns 1 when no probe hit (flags==0), else 0. The scratch writes
// use raw absolute 0x700038xx addresses while the call arguments use the &D_700038xx
// symbols, mirroring the original's mix of literal and relocated addresses.
extern void func_001026A0(float *dst, char *src, float *m);
extern int func_0019AFE0(int self, float *a, float *b, int n);
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;

int func_0017F320(int arg0) {
    int flags;

    *(int *)0x700038A0 = 0x40400000;
    *(int *)0x700038B0 = 0x40400000;
    *(int *)0x700038A4 = 0x41A00000;
    *(int *)0x700038B4 = 0x41A00000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    flags = 0;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 1;
    }

    *(int *)0x700038A0 = 0xC0400000;
    *(int *)0x700038B0 = 0xC0400000;
    *(int *)0x700038A4 = 0x41A00000;
    *(int *)0x700038B4 = 0x41A00000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 2;
    }

    *(int *)0x700038A0 = 0x40400000;
    *(int *)0x700038B0 = 0x40400000;
    *(int *)0x700038A4 = 0x41980000;
    *(int *)0x700038B4 = 0x41980000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 1;
    }

    *(int *)0x700038A0 = 0xC0400000;
    *(int *)0x700038B0 = 0xC0400000;
    *(int *)0x700038A4 = 0x41980000;
    *(int *)0x700038B4 = 0x41980000;
    *(int *)0x700038A8 = 0;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40D00000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038C0, (char *)(arg0 + 0xD0), &D_700038A0);
    func_001026A0(&D_700038D0, (char *)(arg0 + 0xD0), &D_700038B0);
    if (func_0019AFE0(arg0, &D_700038C0, &D_700038D0, 6) != 0) {
        flags |= 2;
    }

    if (flags) return 0;
    return 1;
}
