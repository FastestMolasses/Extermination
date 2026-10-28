// SPAD: 0x700031D0 0x700031D8
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.56% NEARMISS).
// Fixes read from the original instructions:
//  - func_001A6440 and func_001A7280 take ONE int argument (their definitions in
//    src/); the old three-argument externs passed the advanced vector pointers.
//  - The stack scratch is a 4-word vector whose fourth word is set to 1.0f (bit
//    pattern 0x3F800000); the old separate local was dead and its store dropped.
//  - The three 3-element copy loops are indexed for-loops over D_70003190 as a
//    float array (+4 = the second vector, +8 = the third), not pointer walkers.
//  - 0x700031D4 is the named extern D_700031D4 (an object pointer); 0x700031D0 and
//    0x700031D8 are relocated externs (opted in with the SPAD line above).
//
// Takes a 5th integer argument in $t0 (arg4); all callers pass 0x40.
//
// Copies a 3-vector pair (arg1 -> D_70003190[0..2], arg2 -> D_70003190[4..6] and
// the stack scratch), primes global scale/blend words at 0x700031AC/319C and clears
// 0x700031D4, then, gated on the low bits of arg3 (flags), dispatches one of three
// sub-updates and records the winning mode (1/2/4) at 0x700031D8.
// flags&1 (and arg0[0]&1): if (arg0[2]&0x1F)==0 run func_001A6440, else the
// 0x40-path func_001A7280 (only when arg4&0xFFFF == 0x40; otherwise the result is
// the copy loop's final count, 3); on success copy D_70003190[8..10] to [4..6] ->
// mode 1. flags&2: store arg0[0x14] to 0x70003254, func_001A0B10 -> mode 2.
// flags&4: func_0019D330 -> mode 4. On any success, copy the stack scratch back
// into D_70003190[4..6]; else clear 0x700031D0.
extern int func_001A6440(int);
extern int func_001A7280(int);
extern int func_001A0B10(void);
extern int func_0019D330(void);
extern float D_70003190[];
extern char *D_700031D4;
extern int D_700031D0;
extern int D_700031D8;

int func_0019A6F0(char *arg0, float *arg1, float *arg2, int arg3, int arg4) {
    struct { float v[3]; int w; } sp50;
    int mode;
    int flags;
    int ok;
    int i;

    mode = 0;
    for (i = 0; i < 3; i++) {
        float f;
        D_70003190[i] = arg1[i];
        f = arg2[i];
        sp50.v[i] = f;
        D_70003190[i + 4] = f;
    }
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    flags = arg3 & 0xFF;
    sp50.w = 0x3F800000;
    D_700031D4 = 0;
    if ((flags & 1) && (*(unsigned char *)arg0 & 1)) {
        if (!(*(unsigned char *)(arg0 + 2) & 0x1F)) {
            ok = func_001A6440(arg4 & 0xFFFF);
            if (ok != 0 && (*(unsigned short *)(D_700031D4 + 0x52) & 2)) {
                ok = 0;
            }
        } else {
            /* With t0 != 0x40 the call is skipped and the result register still
             * holds the entry copy loop's count, 3 (nonzero). */
            ok = i;
            if ((arg4 & 0xFFFF) == 0x40) {
                ok = func_001A7280(0x40);
            }
            if (ok != 0 && !(*(unsigned short *)(arg0 + 0x52) & 2)) {
                ok = 0;
            }
        }
        if (ok != 0 && !(*(unsigned short *)(arg0 + 0x52) & 1)) {
            for (i = 0; i < 3; i++) {
                D_70003190[i + 4] = D_70003190[i + 8];
            }
            mode = 1;
        }
    }
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;
    if (flags & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_001A0B10() != 0) {
            mode = 2;
        }
    }
    if ((flags & 4) && func_0019D330() != 0) {
        mode = 4;
    }
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = sp50.v[i];
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = mode;
    return mode;
}
