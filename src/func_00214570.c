// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.61% NEARMISS).
// Fixes read from the original instructions:
//  - func_0020B0D0 is called as (arg0, D_00265B80) per its definition in src/
//    (int, char *); the old C passed (0, arg0).
//  - The state dispatch is a `switch` on byte 5 (case 0 falls into case 1, every
//    case ends in `break`), not an if/goto chain.
//  - D_002821B0 is one int array (elements 0, 1 and 0x24 are written); separate
//    externs made the scheduler reorder the three stores.
//  - The matching-entry loop reads the list as `arg0 + 0x50 + i` (address operand
//    order decides the register colouring), and case 3 decrements byte 6 in place.
//
// State machine on byte 5 of the object at arg0. State 0 resets counters, sets
// D_002821B0[0]/[1]/[0x24], and appends codes 1 or 0, 2, 3 to the list at +0x50
// (count at +0x18) from the flags D_00810C7C/7B/7D/7E. If D_008106B0 is set it
// looks up D_008106B1 - 0x17 in that list (index to +0x17), clears the flag and
// goes to state 3 with byte 6 = 0xF0; otherwise it advances to state 1. State 1
// on pad bit 0x20 calls func_0020CD60 and resets bytes 1..5; otherwise it runs a
// byte-6 repeat timer on pad bit 0x40, then the shared calls, advancing on a
// nonzero func_0020B210. State 2 steps back on a nonzero func_0020BC50. State 3
// counts byte 6 down and returns to state 1 at zero or on pad bits 0x5060.
extern void func_0020A7A0(unsigned long long v);
extern void func_0020AE40(void *a0, void *a1, int flags);
extern int func_0020B210(void *a0, void *a1, unsigned long long a2, int a3);
extern int func_0020BC50(void *a0, void *a1, unsigned long long a2, int a3);
extern void func_0020B0D0(int arg0, char *arg1);
extern void func_0020BBE0(char *a0, int a1);
extern void func_0020CD60(void);
extern void func_0020CD80(void);

extern char D_00265B80[];
extern char D_00265BF0[];
extern int D_002821B0[];
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_00810C60;
extern unsigned char D_00810C7B;
extern unsigned char D_00810C7C;
extern unsigned char D_00810C7D;
extern unsigned char D_00810C7E;
extern unsigned short D_00810E74;

void func_00214570(char *arg0) {
    int st;
    int idx;
    int i;

    st = *(unsigned char *)(arg0 + 5);

    switch (st) {
    case 0:
    *(unsigned char *)(arg0 + 0x17) = 0;
    *(unsigned char *)(arg0 + 0x19) = 0;
    *(unsigned char *)(arg0 + 0x18) = 0;
    *(unsigned char *)(arg0 + 0x1A) = 0;
    D_002821B0[0] = 4;
    D_002821B0[1] = 0;
    D_002821B0[0x24] = 3;
    *(short *)(arg0 + 0x1E) = 0x17;

    if (D_00810C7C != 0) {
        arg0[0x50 + (*(unsigned char *)(arg0 + 0x18))++] = 1;
    } else if (D_00810C7B != 0) {
        arg0[0x50 + (*(unsigned char *)(arg0 + 0x18))++] = 0;
    }
    if (D_00810C7D != 0) {
        arg0[0x50 + (*(unsigned char *)(arg0 + 0x18))++] = 2;
    }
    if (D_00810C7E != 0) {
        arg0[0x50 + (*(unsigned char *)(arg0 + 0x18))++] = 3;
    }

    if (D_008106B0 != 0) {
        *(unsigned char *)(arg0 + 0x1B) = D_008106B1 - 0x17;
        for (i = 0; i < *(unsigned char *)(arg0 + 0x18); i++) {
            if (*(unsigned char *)(arg0 + 0x50 + i) == *(unsigned char *)(arg0 + 0x1B)) {
                *(unsigned char *)(arg0 + 0x17) = i;
                D_002821B0[0x24] = 4;
                if ((D_008106B1 == 0x19) && (D_00810C60 == 1)) {
                    D_00810C60 = 2;
                }
                break;
            }
        }
        D_008106B0 = 0;
        *(unsigned char *)(arg0 + 5) = 3;
        *(unsigned char *)(arg0 + 6) = 0xF0;
        break;
    }

    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
    *(unsigned char *)(arg0 + 6) = 0;

    case 1:
    if (D_00810E74 & 0x20) {
        func_0020CD60();
        D_002821B0[1] = 2;
        *(unsigned char *)(arg0 + 1) = 3;
        *(unsigned char *)(arg0 + 2) = 0;
        *(unsigned char *)(arg0 + 3) = 0;
        *(unsigned char *)(arg0 + 4) = 0;
        *(unsigned char *)(arg0 + 5) = 0;
        break;
    }

    if (*(unsigned char *)(arg0 + 6) == 0) {
        if (D_00810E74 & 0x40) {
            func_0020CD80();
            *(unsigned char *)(arg0 + 6) = 0xF;
        }
    } else {
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) - 1;
    }

    func_0020A7A0(((long long)0x20042585 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2100);
    func_0020AE40(arg0, D_00265B80, 1);
    if (func_0020B210(arg0, D_00265BF0, ((long long)0x20042605 << 0x20) | ((long long)0xA132 << 0x10) | 0x1F80, 1) != 0) {
        *(short *)(arg0 + 0x1C) = 0;
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        func_0020BBE0(arg0, *(unsigned char *)(arg0 + 0x1A));
    }
    func_0020B0D0((int)arg0, D_00265B80);
    break;

    case 2:
    func_0020A7A0(((long long)0x20042585 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2100);
    func_0020AE40(arg0, D_00265B80, 1);
    if (func_0020BC50(arg0, D_00265BF0, ((long long)0x20042605 << 0x20) | ((long long)0xA132 << 0x10) | 0x1F80, 1) != 0) {
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) - 1;
    }
    func_0020B0D0((int)arg0, D_00265B80);
    break;

    case 3:
    func_0020A7A0(((long long)0x20042585 << 0x20) | ((long long)0x9D42 << 0x10) | 0x2100);
    func_0020AE40(arg0, D_00265B80, 1);
    func_0020B210(arg0, D_00265BF0, ((long long)0x20042605 << 0x20) | ((long long)0xA132 << 0x10) | 0x1F80, 0x401);
    func_0020B0D0((int)arg0, D_00265B80);
    if ((--*(unsigned char *)(arg0 + 6) == 0) || (D_00810E74 & 0x5060)) {
        if (D_00810E74 & 0x5060) {
            func_0020CD60();
        }
        D_008106B0 = 0;
        *(unsigned char *)(arg0 + 5) = 1;
        *(unsigned char *)(arg0 + 6) = 0;
        D_002821B0[0x24] = 3;
    }
    break;
    }
}
