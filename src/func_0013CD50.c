// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038B0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.05% NEARMISS).
// Fixes read from the original instructions:
//  - Local declaration order: the loop index first, then the three per-slot
//    scan pointers, then hi before lo (first declared gets the lowest saved
//    register); the "register permutation" was this, not a compiler wall.
//  - The scan loop advances the pointers before the index.
//  - The mask switch has an explicit case 3 returning 3; any other mask
//    value returns 0 (the old default returned 3).
//  - The second scan re-reads the table count in its condition (no local
//    count copy, so mwcc keeps the unrotated loop), walks a block-local
//    pointer, and writes the +-5.0 limits inline.
//  - The scratchpad word 0x700038B0 is a relocated extern (idiom-32, opted in
//    with // SPAD:): its two clears go through D_700038B0, which stops mwcc
//    hoisting the 0x7000 upper half into the scan loop's exit branch slots.
//
// func_0019AFE0(self, self+0xB0, pos, 6) gates it (0 -> 0; bit 1 with
// *0x700031D4 clear, or bit 0 -> 3). After func_0019BC40(pos) fills the
// parallel tables D_700030F0[] (values), D_70003170[] (flags) and
// D_00282250[] (angles), it scans from pos[1] (+-1000 defaults) for the
// bounding entries below (lo, flag bit 0 clear) and above (hi, flag bit 0
// set) whose |angle| (func_0011DF78) is <= pi/3, rebuilds the
// scratch transform at 0x700038B0 from self+0xC4, rescans to build a 2-bit mask
// of entries outside the [lo-5, hi+5] band, and returns 1 (near hi) / 2 (near
// lo) / 3 (blocked) from the 25-unit distances.

extern int func_0019AFE0(void *p, void *a, void *b, int n);
extern void func_0019BC40(void *p);
extern float func_0011DF78(float a);
extern float func_001B1470(float a);
extern void func_001029C0(void *a);
extern void func_00102C58(void *a, void *b, void *c);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);

extern float D_00282250[];
extern float D_700030F0[];
extern unsigned short D_70003170[];
extern float D_700031B0;
extern float D_700036A0;
extern float D_700038B0;

int func_0013CD50(char *arg0, char *arg1) {
    int i;
    float *tpos;
    unsigned short *flags;
    float *tsin;
    float hi;
    float lo;
    int mask;
    int r;

    r = func_0019AFE0(arg0, arg0 + 0xB0, arg1, 6);
    if (r == 0) {
        return 0;
    }
    if ((r & 2) && *(int *)0x700031D4 == 0) {
        return 3;
    }
    if (r & 1) {
        return 3;
    }
    func_0019BC40(arg1);
    if (*(int *)0x700031E0 == 0) {
        return 3;
    }

    lo = *(float *)(arg1 + 4) - 1000.0f;
    tpos = D_700030F0;
    flags = D_70003170;
    hi = 1000.0f + *(float *)(arg1 + 4);
    tsin = D_00282250;
    i = 0;

    for (; i < *(int *)0x700031E0; tpos += 1, flags += 1, tsin += 1, i++) {
        if (!(*(float *)(arg1 + 4) < *tpos)) {
            if (!(*flags & 1) && func_0011DF78(*tsin) <= 1.0471976f) {
                lo = *tpos;
            }
            continue;
        }
        if ((D_70003170[i] & 1) && func_0011DF78(D_00282250[i]) <= 1.0471976f) {
            hi = D_700030F0[i];
        }
        break;
    }

    D_700038B0 = 0.0f;
    *(float *)0x700038B4 = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0x3F800000;
    func_001029C0(&D_700036A0);
    func_00102C58(&D_700036A0, &D_700036A0, &D_700038B0);
    D_700038B0 = 0.0f;
    *(float *)0x700038B4 = 0.0f;
    *(int *)0x700038B8 = 0x3DCCCCCD;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
    func_001028B8(&D_700038B0, &D_700038B0, &D_700031B0);
    func_0019BC40(&D_700038B0);

    mask = 0;
    if (*(int *)0x700031E0 != 0) {
        float *p = D_700030F0;
        for (i = 0; i < *(int *)0x700031E0; p += 1, i++) {
            if (*p <= *(float *)(arg1 + 4)) {
                if (!(*p <= lo - 5.0f)) {
                    mask |= 1;
                }
            } else if (*p < 5.0f + hi) {
                mask |= 2;
            }
        }
    }

    switch (mask) {
    case 0:
        if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= 25.0f && func_0011DF78(*(float *)(arg1 + 4) - lo) <= 25.0f) {
            if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= func_0011DF78(*(float *)(arg1 + 4) - lo)) {
                return 1;
            }
            return 2;
        }
        if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= 25.0f) {
            return 1;
        }
        if (func_0011DF78(*(float *)(arg1 + 4) - lo) <= 25.0f) {
            return 2;
        }
        return 1;
    case 1:
        if (func_0011DF78(hi - *(float *)(arg1 + 4)) <= 25.0f) {
            return 1;
        }
        return 3;
    case 2:
        if (func_0011DF78(*(float *)(arg1 + 4) - lo) <= 25.0f) {
            return 2;
        }
        return 3;
    case 3:
        return 3;
    }
    return 0;
}
