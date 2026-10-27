// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.14% NEARMISS).
// Fixes read from the original instructions:
//  - func_001FEC20 takes SIX arguments (its other callers, e.g. the matched
//    func_00228530 and func_002283B0, pass 0 then the literal 1 before
//    &D_00821500); the 5-argument extern explained the "t1 instead of t0" residual.
//    mwcc shares that literal 1 with the state-1 dispatch compare constant.
//  - -sdatathreshold 8 (as the sibling func_002283B0): D_00275C58 is reached
//    gp-relative; the two table addresses are incomplete arrays so they stay
//    absolute.
//  - The +0x60 completion test is a switch (case 1 / default), not an if: that
//    gives the original's branch-to-case layout in both state 0 and state 2.
//  - The retry cap is `> 5` with the counter field bumped in place (`+= 1`),
//    which gives the original's branch-likely increment.
//  - State 1 handles the r == 0 arm first and returns 1 after it; state 2's
//    result is `func_00228320(p) != 0 ? 1 : 3`.
//  - func_001FE9A0 takes pointer arguments (as declared in the sibling
//    func_002283B0); the int casts reordered the argument setup.
//
// Per-object stream/handshake step, gated on func_001FE9A0 readiness (a0 = 1,
// the +0x50/+0x54 channel pair); returns 0 when the gate is not ready.
// Dispatch on the state byte at +2:
//   state 0: poll func_001FECB0 (status into +0x58, result into +0x60). On a
//            finished poll a status other than 2 returns 4; with status 2 a
//            result of 1 advances to state 1, otherwise the retry counter at
//            +0x20 is bumped and, past 5 retries, cleared, func_001FE8D0 is
//            called and 2 is returned. A poll result of -1 is "still busy";
//            any other result returns 4.
//   state 1: step func_001FEC20 with the table D_002738E0 / D_00821500. A
//            negative result is busy; a positive one returns 1; zero returns 1
//            when the byte at +0x14 is 2, else advances to state 2.
//   state 2: like state 0 (also asking for +0x5C); a result of 1 returns 1 or 3
//            depending on func_00228320(p).
// Shared tail: return D_00275C58 ? 4 : 0 (the busy flag).

extern int func_001FE9A0(int a0, char *a1, char *a2);
extern int func_001FECB0(int a0, int a1, void *a2, void *a3, void *a4);
extern int func_001FEC20(int a0, int a1, void *a2, int a3, int a4, void *a5);
extern void func_001FE8D0(void);
extern int func_00228320(char *p);
extern int D_002738E0[];
extern int D_00275C58;
extern int D_00821500[];

int func_002280F0(char *p) {
    int r;
    int s;

    if (func_001FE9A0(1, p + 0x50, p + 0x54) == 0) {
        return 0;
    }
    switch (*(unsigned char *)(p + 2)) {
    case 0:
        r = func_001FECB0(*(int *)(p + 0x48), *(int *)(p + 0x4C), p + 0x58, 0, p + 0x60);
        if (r == 0) {
            s = *(int *)(p + 0x58);
            if (s != 2) {
                return 4;
            }
            if (s == 2) {
                switch (*(int *)(p + 0x60)) {
                case 1:
                    *(unsigned char *)(p + 2) = 1;
                    break;
                default:
                    if (*(int *)(p + 0x20) > 5) {
                        *(int *)(p + 0x20) = 0;
                        func_001FE8D0();
                        return 2;
                    }
                    *(int *)(p + 0x20) += 1;
                    break;
                }
            }
            break;
        }
        if (r == -1) {
            break;
        }
        return 4;
    case 1:
        r = func_001FEC20(*(int *)(p + 0x48), *(int *)(p + 0x4C), D_002738E0, 0, 1, D_00821500);
        if (r < 0) {
            break;
        }
        if (r == 0) {
            if (*(unsigned char *)(p + 0x14) == 2) {
                return 1;
            }
            *(unsigned char *)(p + 2) = 2;
            break;
        }
        return 1;
    case 2:
        r = func_001FECB0(*(int *)(p + 0x48), *(int *)(p + 0x4C), p + 0x58, p + 0x5C, p + 0x60);
        if (r == 0) {
            s = *(int *)(p + 0x58);
            if (s != 2) {
                return 4;
            }
            if (s == 2) {
                switch (*(int *)(p + 0x60)) {
                case 1:
                    return func_00228320(p) != 0 ? 1 : 3;
                default:
                    if (*(int *)(p + 0x20) > 5) {
                        *(int *)(p + 0x20) = 0;
                        func_001FE8D0();
                        return 2;
                    }
                    *(int *)(p + 0x20) += 1;
                    break;
                }
            }
            break;
        }
        if (r == -1) {
            break;
        }
        return 4;
    }
    return D_00275C58 != 0 ? 4 : 0;
}
