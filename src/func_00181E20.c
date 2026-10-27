// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.12% NEARMISS).
// Fixes read from the original instructions:
// - The scratchpad float update is a compound `+= 25.0f`; the spelled-out
//   `x = x + 25.0f` colours the two float temporaries the other way round.
// - The result is one `if (hit && type == 0x1E) ret = 1; else ret = 0;`:
//   both failing tests share one "result 0" block, and the result is never
//   pre-set before the call (which forced it into a saved register).
//
// Writes the scratchpad vector at 0x700038A0 as (0, 0, 1 + fparg0, 1.0f).
// If arg1 is 0 it builds a matrix in 0x700036A0 (func_001029C0, then
// func_00102BB0 with the float at arg0+0x218, then func_00102918 with arg0+0xB0)
// and transforms the vector into 0x700038B0; otherwise it transforms it with
// the matrix at arg0+0xD0. It copies 0x700038B0 to 0x700038C0, adds 25.0f to
// that copy's second component, and calls func_0019A570 on the two points
// (mode 4). Returns 1 only if that call is non-zero and the byte at +0x1A of
// the record pointed to by 0x700031D0 is 0x1E; otherwise 0.

extern void func_001029C0(void *);
extern void func_00102BB0(void *, void *, float);
extern void func_00102918(void *, void *, float *);
extern void func_001026A0(void *, void *, void *);
extern void func_001031E0(void *, void *);
extern int func_0019A570(void *, void *, int, int);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_00181E20(char *arg0, int arg1, float fparg0) {
    int ret;
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = 1.0f + fparg0;
    *(int *)0x700038AC = 0x3F800000;
    if (arg1 == 0) {
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0x218));
        func_00102918(D_700036A0, D_700036A0, (float *)(arg0 + 0xB0));
        func_001026A0(D_700038B0, D_700036A0, D_700038A0);
    } else {
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
    }
    func_001031E0(D_700038C0, D_700038B0);
    *(float *)0x700038C4 += 25.0f;
    if (func_0019A570(D_700038B0, D_700038C0, 4, 0) != 0 && *(unsigned char *)(*(char **)0x700031D0 + 0x1A) == 0x1E) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
