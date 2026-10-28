// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.67% NEARMISS).
// Fixes read from the original instructions:
//  - The loop counter `i` is the first-declared local (it owns the lowest saved
//    register, then flags/tsin/tpos), not the last.
//  - The camera-origin source vector at 0x700031B0 and the probe vector at
//    0x700038B0 are relocated Vec4 externs accessed by member, not literal
//    addresses; that lets the compiler hoist the source loads above the stores,
//    which the original does.
//
// Builds the vector at 0x700038B0 from the vector at 0x700031B0 minus the
// x/z fields (+0x24/+0x2C) of the object at *0x700031D0 (scaled by 4 when the
// byte at arg0+0x316 is set), y = arg0's +0xB4 plus 20.5, w = 1, and passes it to
// func_0019BC40. Then walks the *0x700031E0 table entries; an entry with flag
// bit 0 set and a value below pi/5 in D_00282250 qualifies if func_0011DF78 of
// (y - its D_700030F0 value) is below 1.0. On the first such entry, if arg1 is
// non-zero, it writes arg0's +0x2E0/+0x2E4/+0x2E8 position and its +0x218 angle
// (func_001B1470 of 3*pi/2 plus func_0011E620 of the negated z and x fields).
// Returns 1 on a qualifying entry, 0 otherwise.

extern float func_0011DF78(float a);
extern float func_0011E620(float y, float x);
extern int func_0019BC40(void *p);
extern float func_001B1470(float a);

extern float D_00282250;
extern float D_700030F0;
extern unsigned short D_70003170;
typedef struct { float x, y, z, w; } Vec4;
extern Vec4 D_700038B0;
extern Vec4 D_700031B0;

int func_00178910(char *arg0, int arg1) {
    int i;
    unsigned short *flags;
    float *tsin;
    float *tpos;
    char *p;
    float d;

    if (*(unsigned char *)(arg0 + 0x316) != 0) {
        char *cam1 = *(char **)0x700031D0;
        D_700038B0.x = D_700031B0.x - (4.0f * *(float *)(cam1 + 0x24));
        D_700038B0.z = D_700031B0.z - (4.0f * *(float *)(cam1 + 0x2C));
    } else {
        char *cam2 = *(char **)0x700031D0;
        D_700038B0.x = D_700031B0.x - *(float *)(cam2 + 0x24);
        D_700038B0.z = D_700031B0.z - *(float *)(cam2 + 0x2C);
    }
    D_700038B0.y = 20.5f + *(float *)(arg0 + 0xB4);
    D_700038B0.w = 1.0f;
    func_0019BC40(&D_700038B0);

    i = 0;
    flags = &D_70003170;
    tsin = &D_00282250;
    tpos = &D_700030F0;
    goto guard;
    for (;;) {
        if ((*flags & 1) && *tsin < 0.62831855f) {
            d = func_0011DF78(D_700038B0.y - *tpos);
            *(float *)0x70003A20 = d;
            if (d < 1.0f) {
                if (arg1 != 0) {
                    p = *(char **)0x700031D0;
                    *(float *)(arg0 + 0x2E0) = D_700031B0.x + 1.5f * *(float *)(p + 0x24);
                    *(float *)(arg0 + 0x2E8) = D_700031B0.z + 1.5f * *(float *)(*(char **)0x700031D0 + 0x2C);
                    *(float *)(arg0 + 0x2E4) = (&D_700030F0)[i] - 20.5f;
                    p = *(char **)0x700031D0;
                    *(float *)0x70003A20 = func_0011E620(-*(float *)(p + 0x2C), *(float *)(p + 0x24));
                    *(float *)(arg0 + 0x218) = func_001B1470(4.712389f + *(float *)0x70003A20);
                }
                return 1;
            }
        }
        flags += 1;
        tsin += 1;
        tpos += 1;
        i++;
    guard:
        if (i >= *(int *)0x700031E0) {
            return 0;
        }
    }
}
