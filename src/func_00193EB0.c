// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.26% NEARMISS). Fixes read
// from the original instructions:
//  - the goto chain is a `switch` on arg1+0x230 whose cases end in `break` (2.3.3 emits the
//    case compares in reverse source order, so the cases are listed 0x10 first);
//  - the D_00810701 test was inverted: reaction 0xD is set when D_00810701 == 0, and the
//    position-band checks run only when it is nonzero;
//  - the arg1+0xA8 band had its two arms swapped: A8 <= 900 tests A4 <= 365 and fires K=0xC,
//    otherwise A4 <= 356 fires K=0xB (written as a negated `<=` test so the branch sense matches).
//
// Actor "hit reaction" event dispatcher. arg1+0x230 is an incoming event/state code; arg0 is the
// actor record (arg0+6 = reaction id, arg0+1 = a busy/lock flag cleared on every dispatch).
// Most codes (0x10, 0x2A/0xD, 0xC/0x29, 0x28, 0x12) just set a fixed reaction id and clear the
// lock flag. Codes {1, 0x21} instead forward to func_00191000(arg0, arg1) when arg2 != 2 and the
// actor isn't already busy/reacting (arg0+5 == 0 && arg0+6 == 0). Codes {8,9,7,6,0x2D,0x2C} are a
// region/area-gated combat trigger: for area 0x16 -> reaction 0xC; for area 0x13, reaction 0xD
// when D_00810701 == 0, otherwise (when the two hard-addressed gates D_008106B8/*0x70003B8D are
// clear) it tests the actor's world position bands (arg1+0xA0/0xA4/0xA8) against fixed
// thresholds and fires func_001B0C60(0x13, 0, K) with K in {0xD, 0xB, 0xC} for the matching band,
// setting reaction 7; for area 0xD (same gates) it checks D_00810702 in {4,6} or {5,7} against a
// Z threshold (159.0f) and fires func_001B0C60(0x13, 0, K) with K in {9, 0xA}, also setting
// reaction 7.

extern int func_00191000(char *a0, char *a1);
extern void func_001B0C60(int a0, int a1, int a2);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_008106B8;

void func_00193EB0(unsigned char *arg0, char *arg1, int arg2) {
    switch (*(int *)(arg1 + 0x230)) {
    case 0x10:
        arg0[6] = 9;
        arg0[1] = 0;
        break;
    case 0x2A:
    case 0xD:
        arg0[6] = 1;
        arg0[1] = 0;
        break;
    case 0xC:
    case 0x29:
        arg0[6] = 2;
        arg0[1] = 0;
        break;
    case 0x28:
        arg0[6] = 0xE;
        arg0[1] = 0;
        break;
    case 0x12:
        arg0[6] = 0xB;
        arg0[1] = 0;
        break;
    case 0x21:
    case 1:
        if (arg2 != 2 && arg0[5] == 0 && arg0[6] == 0) {
            func_00191000((char *)arg0, arg1);
        }
        break;
    case 0x2C:
    case 0x2D:
    case 6:
    case 7:
    case 9:
    case 8:
        if (D_00810700 == 0x16) {
            arg0[6] = 0xC;
            arg0[1] = 0;
            break;
        }
        if (D_00810700 == 0x13) {
            if (D_00810701 == 0) {
                arg0[6] = 0xD;
                arg0[1] = 0;
                break;
            }
            if (D_008106B8 != 0) break;
            if (*(unsigned char *)0x70003B8D != 0) break;
            if (*(float *)(arg1 + 0xA0) < 872.0f) {
                if (*(float *)(arg1 + 0xA4) <= 356.0f) {
                    func_001B0C60(0x13, 0, 0xD);
                    arg0[6] = 7;
                }
                break;
            }
            if (!(*(float *)(arg1 + 0xA8) <= 900.0f)) {
                if (*(float *)(arg1 + 0xA4) <= 356.0f) {
                    func_001B0C60(0x13, 0, 0xB);
                    arg0[6] = 7;
                }
                break;
            }
            if (*(float *)(arg1 + 0xA4) <= 365.0f) {
                func_001B0C60(0x13, 0, 0xC);
                arg0[6] = 7;
            }
            break;
        }
        if (D_00810700 != 0xD) break;
        if (D_008106B8 != 0) break;
        if (*(unsigned char *)0x70003B8D != 0) break;
        {
            int m = D_00810702;
            if (m == 4 || m == 6) {
                if (*(float *)(arg1 + 0xA4) <= 159.0f) {
                    func_001B0C60(0x13, 0, 9);
                    arg0[6] = 7;
                }
                break;
            }
            if (m == 5 || m == 7) {
                if (*(float *)(arg1 + 0xA4) <= 159.0f) {
                    func_001B0C60(0x13, 0, 0xA);
                    arg0[6] = 7;
                }
            }
        }
        break;
    }
}
