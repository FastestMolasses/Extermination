// SPAD: 0x700031D0 0x700031D8
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.72% NEARMISS).
// Fixes read from the original instructions (spelled like the byte-matched twin
// func_0019A6F0):
//  - func_001A6440 takes ONE int argument (its definition in src/); the old
//    two-argument extern passed the advanced vector pointer.
//  - The stack scratch is a 4-word vector whose fourth word is set to 1.0f (bit
//    pattern 0x3F800000).
//  - The three 3-element copy loops are indexed for-loops over D_70003190 as a
//    float array (+4 = the second vector, +8 = the third), not pointer walkers.
//  - 0x700031D4 is the named extern D_700031D4 (an object pointer); 0x700031D0 and
//    0x700031D8 are relocated externs (opted in with the SPAD line above).
//
// Copies a 3-vector pair (arg0 -> D_70003190[0..2], arg1 -> D_70003190[4..6] and
// the stack scratch), primes the w words at 0x700031AC/319C to 1.0f and clears
// 0x700031D4, then, gated on the low bits of arg2 (flags), runs up to three
// sub-tests and records the winning mode (1/2/4) at 0x700031D8.
// flags&1: func_001A6440(arg3&0xFFFF); on success copy D_70003190[8..10] to
// [4..6] -> mode 1. 0x7000324E is set to -1. flags&2: clear 0x70003254,
// func_001A0B10 -> mode 2. flags&4: func_0019D330 -> mode 4. On any success, copy
// the stack scratch back into D_70003190[4..6]; else clear 0x700031D0.

extern int func_001A6440(int);
extern int func_001A0B10(void);
extern int func_0019D330(void);
extern float D_70003190[];
extern char *D_700031D4;
extern int D_700031D0;
extern int D_700031D8;

int func_0019A570(float *arg0, float *arg1, int arg2, int arg3) {
    struct { float v[3]; int w; } sp40;
    int mode;
    int flags;
    int i;

    mode = 0;
    for (i = 0; i < 3; i++) {
        float f;
        D_70003190[i] = arg0[i];
        f = arg1[i];
        sp40.v[i] = f;
        D_70003190[i + 4] = f;
    }
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    flags = arg2 & 0xFF;
    sp40.w = 0x3F800000;
    D_700031D4 = 0;
    if ((flags & 1) && func_001A6440(arg3 & 0xFFFF) != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = D_70003190[i + 8];
        }
        mode = 1;
    }
    *(short *)0x7000324E = -1;
    if (flags & 2) {
        *(int *)0x70003254 = 0;
        if (func_001A0B10() != 0) {
            mode = 2;
        }
    }
    if ((flags & 4) && func_0019D330() != 0) {
        mode = 4;
    }
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = sp40.v[i];
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = mode;
    return mode;
}
