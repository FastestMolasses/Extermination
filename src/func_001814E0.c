// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.35% NEARMISS).
// Fixes read from the original instructions:
// - Wrong body: the 23F==3 retry runs whenever the first pass did not return 1
//   (probe failed OR the copied byte is neither 0x34 nor 0x1E), not only when
//   the first probe failed.
// - The rotation scratch at 0x700038A0 is the int array D_700038A0[4] (the
//   aggregate stores keep the 23F==3 test's branch slot empty like the original).
// - func_001026A0 takes three void pointers (its definition in src/); passing
//   the arguments uncast restores the original argument-setup order.
//
// Builds a rotation scratch at D_700038A0 (angle picked by arg1==0 / actor byte
// 0x23F==3 / else), applies it via func_001026A0+func_001031E0, advances the
// D_700038C4 float by 25.0, and if func_0019A570 succeeds copies actor byte 0xD
// from *(0x700031D0)+0x1A, returning 1 when that byte is 0x34 or 0x1E. Otherwise,
// for an actor whose byte 0x23F is 3, it retries once with the default angle and
// returns 2 on the same test; else 0.
extern void func_001026A0(void *, void *, void *);
extern int func_001031E0(float *, float *);
extern int func_0019A570(float *, float *, int, int);
extern int D_700038A0[];
extern char D_700038B0;
extern char D_700038C0;

int func_001814E0(char *arg0, int arg1) {
    unsigned char v;

    if (arg1 == 0) {
        D_700038A0[0] = 0;
        D_700038A0[1] = 0;
        D_700038A0[2] = 0x408CCCCD;
        D_700038A0[3] = 0x3F800000;
    } else if (*(unsigned char *)(arg0 + 0x23F) == 3) {
        D_700038A0[0] = 0;
        D_700038A0[1] = 0;
        D_700038A0[2] = 0x41326666;
        D_700038A0[3] = 0x3F800000;
    } else {
        D_700038A0[0] = 0;
        D_700038A0[1] = 0;
        D_700038A0[2] = 0x40D4CCCD;
        D_700038A0[3] = 0x3F800000;
    }
    func_001026A0(&D_700038B0, arg0 + 0xD0, D_700038A0);
    func_001031E0((float *)&D_700038C0, (float *)&D_700038B0);
    { float _c = 25.0f; *(float *)0x700038C4 = *(float *)0x700038C4 + _c; }
    if (func_0019A570((float *)&D_700038B0, (float *)&D_700038C0, 4, 0) != 0) {
        *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
        v = *(unsigned char *)(arg0 + 0xD);
        if (v == 0x34 || v == 0x1E) {
            return 1;
        }
    }
    if (*(unsigned char *)(arg0 + 0x23F) == 3) {
        D_700038A0[0] = 0;
        D_700038A0[1] = 0;
        D_700038A0[2] = 0x40D4CCCD;
        D_700038A0[3] = 0x3F800000;
        func_001026A0(&D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001031E0((float *)&D_700038C0, (float *)&D_700038B0);
        { float _c = 25.0f; *(float *)0x700038C4 = *(float *)0x700038C4 + _c; }
        if (func_0019A570((float *)&D_700038B0, (float *)&D_700038C0, 4, 0) != 0) {
            *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
            v = *(unsigned char *)(arg0 + 0xD);
            if (v == 0x34 || v == 0x1E) {
                return 2;
            }
        }
    }
    return 0;
}
