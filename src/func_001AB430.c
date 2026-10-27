// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 82.16% NEARMISS).
// Fixes read from the original instructions:
//  - D_00275C78 is a gp-relative byte: -sdatathreshold 4 (was 0) with the far
//    .data globals over-declared as arrays so they keep absolute %hi/%lo
//    addressing (same scheme as func_001AC3B0).
//  - func_001AEA50 takes no arguments (its definition in src/); the old
//    extern passed the loop pointer.
//  - The three settings bytes D_00810118 / D_00810119 / D_0081011C are
//    volatile (as their twins func_001AF150 / func_001AF1C0 treat the mirror
//    block): this keeps the source order 0, 1, 0 instead of mwcc grouping the
//    two zero stores, and lets the last store sink past the loop setup.
//  - The loop counter is declared before the pointer (register order).
//
// Reset routine: calls func_0010C648, clears two scratchpad words
// (0x70003B90 byte, 0x70003B64 word), the short D_00810E80, the bytes
// D_00821058 and D_00275C78, zeroes the 0xC-byte settings block at
// D_00810118 and then sets its fields to 0 / 1 / 0, zeroes the 0xAF-word
// table D_0028A490, stores 0xB00000 into D_0028A734, and finally calls
// func_001AEA50 and func_001FC9B0.

extern void func_0010C648(void);
extern void func_00121A28(unsigned char *, int, int);
extern void func_001AEA50(void);
extern void func_001FC9B0(void);
extern unsigned char D_00275C78;
extern int D_0028A490[];
extern int D_0028A734[4];
extern volatile unsigned char D_00810118[16];
extern volatile unsigned char D_00810119[16];
extern volatile unsigned char D_0081011C[16];
extern short D_00810E80[16];
extern unsigned char D_00821058[16];

void func_001AB430(void) {
    int i;
    int *p;

    func_0010C648();
    *(unsigned char *)0x70003B90 = 0;
    *(int *)0x70003B64 = 0;
    D_00810E80[0] = 0;
    D_00821058[0] = 0;
    D_00275C78 = 0;
    func_00121A28((unsigned char *)D_00810118, 0, 0xC);
    D_00810118[0] = 0;
    D_00810119[0] = 1;
    D_0081011C[0] = 0;
    p = D_0028A490;
    i = 0;
    do {
        i += 1;
        *p = 0;
        p += 1;
    } while (i < 0xAF);
    D_0028A734[0] = 0xB00000;
    func_001AEA50();
    func_001FC9B0();
}
