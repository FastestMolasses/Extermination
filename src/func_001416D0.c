// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031B0 0x700031B4 0x700031B8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 80.44% NEARMISS).
// Fixes read from the original instructions:
//  - func_001B2B10 takes three arguments (its definition in src/): the caller's
//    arg0 is passed through in the first argument register.
//  - The func_001B2BF0 angle limit is PI/4 (0.785398f), not PI.
//  - The three vector components at 0x700031B0/B4/B8 are relocated scratchpad
//    externs (idiom-32); as named objects mwcc knows they do not alias the
//    stack arrays, which lets their loads interleave with the other copy.
//  - The two 4-float buffers are separate stack arrays, and all non-zero paths
//    share one `return 1` after an if / else-if / else chain.
//
// Seeds a scratch vector at 0x700038A0 (0, 3.0f, 13.0f, 1.0f), runs
// func_001B2B10(self, v, v) then func_001028B8(v, v, self+0xB0), then
// func_001B2BF0(self, &D_700038A0, &D_700038D0, PI/4) which returns a bitmask
// in `flags`. When flags & 4 it builds two 4-float vectors on the stack: the
// global entity (*0x700031D0)+0x24/0x28/0x2C and the vector at
// 0x700031B0/B4/B8, each with w = 1.0f, and stores the byte result of
// func_001B3670(self, a, b) at arg1+0x83. Otherwise it sets arg1+0x83 = 2 and
// stores func_001B37D0(self, 10.0f, 20.0f) at arg1+0x44 -- except that when
// flags & 1 is set it first gates on (self->0xB4 - 20.0f) < *0x700038D0,
// returning 0 in that case. Returns 1 otherwise.
extern void func_001B2B10(char *, int *, int *);
extern void func_001028B8(int *, int *, char *);
extern int func_001B2BF0(char *, int *, int *, float);
extern char func_001B3670(char *, float *, float *);
extern float func_001B37D0(char *, float, float);
extern float D_700031B0;
extern float D_700031B4;
extern float D_700031B8;
extern int D_700038A0;
extern int D_700038D0;

int func_001416D0(char *arg0, char *arg1) {
    float a[4];
    float b[4];
    int flags;
    char *p;

    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x40400000;
    *(int *)0x700038A8 = 0x41500000;
    *(int *)0x700038AC = 0x3F800000;
    func_001B2B10(arg0, &D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
    flags = func_001B2BF0(arg0, &D_700038A0, &D_700038D0, 0.785398185f);
    if (!(flags & 1)) {
        if (flags & 4) {
            p = *(char **)0x700031D0;
            a[0] = *(float *)(p + 0x24);
            a[1] = *(float *)(p + 0x28);
            a[2] = *(float *)(p + 0x2C);
            b[0] = D_700031B0;
            b[1] = D_700031B4;
            b[2] = D_700031B8;
            a[3] = 1.0f;
            b[3] = 1.0f;
            *(char *)(arg1 + 0x83) = func_001B3670(arg0, a, b);
        } else {
            *(char *)(arg1 + 0x83) = 2;
            *(float *)(arg1 + 0x44) = func_001B37D0(arg0, 10.0f, 20.0f);
        }
    } else if (flags & 4) {
        p = *(char **)0x700031D0;
        a[0] = *(float *)(p + 0x24);
        a[1] = *(float *)(p + 0x28);
        a[2] = *(float *)(p + 0x2C);
        b[0] = D_700031B0;
        b[1] = D_700031B4;
        b[2] = D_700031B8;
        a[3] = 1.0f;
        b[3] = 1.0f;
        *(char *)(arg1 + 0x83) = func_001B3670(arg0, a, b);
    } else {
        if ((*(float *)(arg0 + 0xB4) - 20.0f) < *(float *)0x700038D0) {
            return 0;
        }
        *(char *)(arg1 + 0x83) = 2;
        *(float *)(arg1 + 0x44) = func_001B37D0(arg0, 10.0f, 20.0f);
    }
    return 1;
}
