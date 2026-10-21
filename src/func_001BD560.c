// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// TU_STATIC: func_001BD270 func_001BD460
//
// Byte-matched (objdiff 100%), decomp lane round 8 (was a 99.08% NEARMISS).
// Two source fixes, both read from the original instructions:
//  - The four mover polls take only the mover: func_001BD180(sub),
//    func_001BD270(sub), func_001BD370(sub), func_001BD460(sub). The target sets
//    only $a0 before each call; the old C passed a second argument 1, which was
//    a register leftover, not an argument.
//  - func_001BD270 and func_001BD460 are compiled as static functions of this
//    file (// TU_STATIC:, MATCHING_GUIDE "Decomp lane round 7"). The original
//    compiler saw them earlier in the same translation unit and kept values in
//    caller-saved registers across those calls. tools/decomp/tu_static.py
//    removes the static copies after checking each equals the original callee;
//    the calls link to the callees' own units.
// The callees are compiled with -sdatathreshold 8 (D_00275B40 is gp-relative),
// so this file uses the same threshold; the four 0x0081xxxx bytes, which the
// original addresses absolutely, are declared as arrays so they stay out of
// small data (the size only keeps them there, it is not the object's size).
//
// SEMANTICS: top-level per-frame update for the door/lift object arg0, dispatched
// on the phase byte at arg0+4 (0 = enter, 1 = run, 2 and 3 = func_001AFC10, other =
// nothing). arg0+0x1F0 is the embedded mover sub-object.
// Phase 0 primes the object: func_001B0FD0(arg0, 1), arg0+0 = 1, the byte counter at
// arg0+0x2E is snapshotted into the short at arg0+0x34 and cleared, the mover's two
// accumulators (+0x10, +0x14) are zeroed, and the scale triple at arg0+0x80..0x88 is
// set from the flag word at arg0+0x56: bit 0x40 -> 1.5, bit 0x80 -> 2.0, else 1.0.
// Phase 1 runs a sub-state machine on arg0+5. The variant byte at arg0+3 selects
// which one: 0xB uses jtbl_0026E220 (8 states, the "keyed" variant with the
// D_0081076C key gate and the D_0081083E toggle) and anything else uses
// jtbl_0026E200 (7 states, toggling D_00810836[0] or [1] depending on whether the
// level id D_00810700 is 4 or 7). Both variants: state 0 waits for arg0+0xB == 2 and
// opens (func_001BBD20), states 1/3 poll the mover (func_001BD180/270 keyed,
// func_001BD370/460 plain), state 2 waits for arg0+0xB == 4 and closes, state 4
// polls func_001BA1F0 then commits with func_001BC150, state 5 hands off to
// func_001AEBA0(4, 1). The keyed variant additionally parks in state 7 until the key
// byte D_0081076C reads 0xFF. The scratchpad word at 0x70003258 gates the audio
// streamer while the keyed variant is moving.
// Every phase-1 path then runs func_001C6380(arg0), the virtual update at arg0+0x4C,
// and — unless arg0+0xB == 3 — trips arg0+1 and calls func_001B1D20(arg0) when the
// owner at arg0+0x114 is nearer than 18.0 (its distance is at +0x80).
//
static int func_001BD270(char *arg0);         /* same-TU callee, see TU_STATIC */
#include "src/func_001BD270.c"
static int func_001BD460(unsigned char *a0);  /* same-TU callee, see TU_STATIC */
#include "src/func_001BD460.c"
extern void func_001AEBA0(int a, int b);
extern void func_001AFC10(char *p);
extern void func_001B0FD0(char *p, int a);
extern void func_001B1D20(char *p);
extern void func_001BA1A0(char *p, char *q);
extern int func_001BA1F0(char *p, int a);
extern void func_001BBD20(char *p, int a);
extern void func_001BC150(char *p);
extern int func_001BD180(char *p);
extern int func_001BD370(char *p);
extern void func_001C6380(char *p);
extern char D_0024E6E0[];
extern unsigned char D_00810700[16];
extern unsigned char D_0081076C[16];
extern unsigned char D_00810836[16];
extern unsigned char D_0081083E[16];

void func_001BD560(char *arg0) {
    char *sub;
    unsigned char st;
    short flags;

    sub = arg0 + 0x1F0;
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        func_001B0FD0(arg0, 1);
        *(char *)(arg0 + 0) = 1;
        *(short *)(arg0 + 0x34) = *(unsigned char *)(arg0 + 0x2E);
        *(short *)(arg0 + 0x2E) = 0;
        *(int *)(sub + 0x10) = 0;
        *(int *)(sub + 0x14) = 0;
        flags = *(short *)(arg0 + 0x56);
        if (flags & 0x40) {
            *(float *)(arg0 + 0x80) = 1.5f;
            *(float *)(arg0 + 0x84) = 1.5f;
            *(float *)(arg0 + 0x88) = 1.5f;
        } else if (flags & 0x80) {
            *(float *)(arg0 + 0x80) = 2.0f;
            *(float *)(arg0 + 0x84) = 2.0f;
            *(float *)(arg0 + 0x88) = 2.0f;
        } else {
            *(float *)(arg0 + 0x80) = 1.0f;
            *(float *)(arg0 + 0x84) = 1.0f;
            *(float *)(arg0 + 0x88) = 1.0f;
        }
        break;
    case 1:
        if (*(unsigned char *)(arg0 + 3) == 0xB) {
            st = *(unsigned char *)(arg0 + 5);
            switch (st) {
            case 0:
                *(volatile int *)0x70003258 = 1;
                if (*(unsigned char *)(arg0 + 0xB) == 2) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    func_001BBD20(arg0, 0);
                    *(volatile int *)0x70003258 = 0;
                }
                break;
            case 1:
                if (func_001BD180(sub) != 0) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    *(unsigned char *)(arg0 + 0xB) = 3;
                }
                break;
            case 2:
                *(volatile int *)0x70003258 = 0;
                if (*(unsigned char *)(arg0 + 0xB) == 4) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    func_001BBD20(arg0, 1);
                    *(volatile int *)0x70003258 = 1;
                }
                break;
            case 3:
                if (func_001BD270(sub) != 0) {
                    *(unsigned char *)(arg0 + 0xB) = 0;
                    *(int *)(sub + 0x10) = 0;
                    *(int *)(sub + 0x14) = 0;
                    if (D_0081076C[0] == 0) {
                        D_0081076C[0] = 1;
                        *(unsigned char *)(arg0 + 5) = 7;
                    } else {
                        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                        func_001BA1A0(sub, D_0024E6E0);
                    }
                }
                break;
            case 4:
                if (func_001BA1F0(arg0, 1) != 0) {
                    func_001BC150(arg0);
                    D_0081083E[0] = 1 - D_0081083E[0];
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                }
                break;
            case 5:
                *(unsigned char *)(arg0 + 5) = st + 1;
                func_001AEBA0(4, 1);
                break;
            case 6:
                break;
            case 7:
                if (D_0081076C[0] == 0xFF) {
                    func_001BC150(arg0);
                    D_0081083E[0] = 1 - D_0081083E[0];
                    *(unsigned char *)(arg0 + 5) = 5;
                }
                break;
            }
        } else {
            st = *(unsigned char *)(arg0 + 5);
            switch (st) {
            case 0:
                if (*(unsigned char *)(arg0 + 0xB) == 2) {
                    *(unsigned char *)(arg0 + 5) = *(volatile unsigned char *)(arg0 + 5) + 1;
                    func_001BBD20(arg0, 0);
                }
                break;
            case 1:
                if (func_001BD370(sub) != 0) {
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    *(unsigned char *)(arg0 + 0xB) = 3;
                }
                break;
            case 2:
                if (*(unsigned char *)(arg0 + 0xB) == 4) {
                    *(unsigned char *)(arg0 + 5) = st + 1;
                    func_001BBD20(arg0, 1);
                }
                break;
            case 3:
                if (func_001BD460((unsigned char *)sub) != 0) {
                    *(unsigned char *)(arg0 + 0xB) = 0;
                    *(int *)(sub + 0x10) = 0;
                    *(int *)(sub + 0x14) = 0;
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    func_001BA1A0(sub, D_0024E6E0);
                }
                break;
            case 4:
                if (func_001BA1F0(arg0, 1) != 0) {
                    func_001BC150(arg0);
                    if (D_00810700[0] == 4 || D_00810700[0] == 7) {
                        D_00810836[0] = 1 - D_00810836[0];
                    } else {
                        D_00810836[1] = 1 - D_00810836[1];
                    }
                    *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                }
                break;
            case 5:
                *(unsigned char *)(arg0 + 5) = st + 1;
                func_001AEBA0(4, 1);
                break;
            case 6:
                break;
            }
        }
        func_001C6380(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        if (*(unsigned char *)(arg0 + 0xB) != 3 &&
            *(float *)(*(char **)(arg0 + 0x114) + 0x80) < 18.0f) {
            *(char *)(arg0 + 1) = 1;
            func_001B1D20(arg0);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
