// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// TU_STATIC: func_001B64F0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 88.03% NEARMISS).
// Fixes read from the original instructions:
// - func_001B64F0 sat earlier in the same translation unit; compiled as a
//   static same-TU callee (TU_STATIC) the compiler knows it leaves t7 alone,
//   so the walking pointer stays in a caller-saved register across the call
//   (no saved register, 16-byte frame). The callee takes (char *, int), per
//   its definition in src/; its own unit's flags differ only in the
//   threshold, and the static copy equals the original at threshold 8.
// - The pointer post-increment belongs in the store (`*p++`): that lets the
//   bit-table byte index load ahead of the global store and puts the
//   increment in the call's delay slot.
// - The bit mask is spelled inline in the test and in the set (no mask
//   local), which gives the original's register choice.
// - Both guards are early returns; the nested-if shape left an extra empty
//   block before the epilogue.
//
// Marks entry D_00810700[0] in the byte bit-table at D_00810B40 (bit
// D_00810701[0]); if the bit was not yet set, sets it, records the table
// cursor in D_00275BE4, and for each nonzero word of the zero-terminated list
// p stores it in D_00275BE8 and runs the entity-array sweep func_001B64F0 on
// it for index D_00810700[0]. Only runs while D_00810788[0] == 0xFF.

extern unsigned char D_00810788[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_00810B40[];
extern unsigned char *D_00275BE4;
extern int D_00275BE8;

static void func_001B64F0(char *p, int idx);
#include "src/func_001B64F0.c"

void func_001B65C0(int *p) {
    unsigned char *q;

    if (D_00810788[0] != 0xFF) {
        return;
    }
    D_00275BE4 = D_00810B40;
    D_00275BE4 = D_00275BE4 + D_00810700[0];
    q = D_00275BE4;
    if ((*q & (1 << D_00810701[0])) != 0) {
        return;
    }
    *q = *q | (1 << D_00810701[0]);
    do {
        D_00275BE8 = *p++;
        func_001B64F0((char *)D_00275BE8, D_00810700[0]);
    } while (*p != 0);
}
