// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.00% NEARMISS).
// Fixes read from the original instructions:
//  - The per-stage bit test is spelled with pointer arithmetic on the byte
//    table, *(D_00810841 + D_00810700[0]) & (1 << p[0x2E]), as in the matched
//    twin func_00158430; the subscript form permuted the test's registers.
//  - Every case ends in `break`, not `return` (as in the matched twin
//    func_00158D30). With `return` mwcc removed the else branch's first
//    instruction after copying it into the branch slot; the original keeps it.
//
// State dispatch on the byte at p+4. State 0: func_001B0FD0(p). State 1:
// func_001C6380(p); the callback at p+0x4C runs when func_001B17A0(p) is
// non-zero; then the four scratchpad words at 0x700038B0..0x700038BC get
// (0, 0x80, 0, 0x80) when bit p[0x2E] is set in the stage mask byte
// D_00810841[D_00810700[0]], else (0x80, 0, 0, 0x80); the words at
// 0x700038C0..0x700038CC = 0x3F800000 (1.0f); and
// func_001F4A10(p + 0xD0, D_700038B0, D_700038C0). States 2 and 3:
// func_001AFC10(p).

extern void func_001AFC10(char *p);
extern int func_001B0FD0(char *p);
extern int func_001B17A0(char *p);
extern void func_001C6380(char *p);
extern void func_001F4A10(void *a, void *b, void *c);
extern unsigned char D_00810700[];
extern unsigned char D_00810841[];
extern char D_700038B0[];
extern char D_700038C0[];

void func_00158BD0(char *p) {
    int t;

    t = *(unsigned char *)(p + 4);
    switch (t) {
    case 0:
        func_001B0FD0(p);
        break;
    case 1:
        func_001C6380(p);
        if (func_001B17A0(p) != 0) {
            (*(void (**)(char *))(p + 0x4C))(p);
        }
        if (*(unsigned char *)(D_00810841 + D_00810700[0]) & (1 << *(unsigned short *)(p + 0x2E))) {
            *(int *)0x700038B0 = 0;
            *(int *)0x700038B4 = 0x80;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x80;
        } else {
            *(int *)0x700038B0 = 0x80;
            *(int *)0x700038B4 = 0;
            *(int *)0x700038B8 = 0;
            *(int *)0x700038BC = 0x80;
        }
        *(int *)0x700038C0 = 0x3F800000;
        *(int *)0x700038C4 = 0x3F800000;
        *(int *)0x700038C8 = 0x3F800000;
        *(int *)0x700038CC = 0x3F800000;
        func_001F4A10(p + 0xD0, D_700038B0, D_700038C0);
        break;
    case 2:
    case 3:
        func_001AFC10(p);
        break;
    }
}
