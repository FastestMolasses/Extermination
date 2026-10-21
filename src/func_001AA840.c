// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// TU_STATIC: func_001AA7A0
//
// Byte-matched (objdiff 100%), decomp lane round 8 (was an 84.27% NEARMISS).
// Scans the D_00275B84 entries of the pointer list D_00275B7C and, for each live
// entry whose type bits (+2 & 0x1F) are 4, sub-type +3 is 0x29 and flag +0 is 1,
// runs the proximity test func_001AA7A0; the first hit ends the scan.
//
// Two source fixes, both read from the original instructions:
//  - func_001AA7A0 takes (a0, a1) and tests the entry passed in $a1; its own
//    first parameter is unused. The target sets $a1 = entry and leaves $a0 as it
//    came in, which is this function's own argument (its caller func_0018A1F0
//    passes arg0), so the call is func_001AA7A0(arg0, e). The old C passed the
//    entry as the only argument, in $a0.
//  - func_001AA7A0 is compiled as a static function of this file
//    (// TU_STATIC:, MATCHING_GUIDE "Decomp lane round 7"). The original compiler
//    saw that leaf earlier in the same translation unit, so the count and the
//    list cursor stay in caller-saved registers across the call with no saved
//    registers and a 16-byte frame (the former residual).
//    tools/decomp/tu_static.py removes the static copy after checking it equals
//    the original func_001AA7A0; the call links to that function's own unit.

static int func_001AA7A0(int a0, int a1);  /* same-TU callee, see TU_STATIC */
#include "src/func_001AA7A0.c"
extern char **D_00275B7C;
extern short D_00275B84;

void func_001AA840(unsigned char *arg0) {
    char **p;
    int count;
    char *e;

    count = D_00275B84;
    if (count != 0) {
        p = D_00275B7C;
        if (count != 0) {
            do {
                e = *p;
                count -= 1;
                p += 1;
                if ((e != 0) &&
                    ((*(unsigned char *)(e + 2) & 0x1F) == 4) &&
                    (*(unsigned char *)(e + 3) == 0x29) &&
                    (*(unsigned char *)(e + 0) == 1)) {
                    if (func_001AA7A0((int)arg0, (int)e) != 0) {
                        return;
                    }
                }
            } while (count != 0);
        }
    }
}
