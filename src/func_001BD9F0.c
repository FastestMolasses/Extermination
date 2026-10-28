// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.64% NEARMISS).
// Fixes read from the original instructions:
// - The outer state test is ONE switch on +4 (cases 0, 1, and 2/3 sharing the
//   func_001AFC10 call), not two early-return ifs ahead of a switch.
// - Every case ends in `break` (no per-case `return`), which gives the
//   branch-likely dispatch with the shared argument copies in its slots.
// - -sdatathreshold 4: D_00275610 is a gp-relative word (as in its siblings
//   func_001BC960 / func_001BDFC0); the 0x0081xxxx globals stay absolute
//   (unsized extern arrays).
// - Local declaration order: the +0x1F0 sub-block pointer is declared before
//   the owner pointer read from +0x18 (lowest saved register first).
//
// State machine on +4: 0 initialises (func_001B0FD0, saves +0x2E into +0x34,
// sets +0x2E from whether +3 is 3, installs the D_00275610 handler table at
// +0x30, func_001C6380, sets byte 0 to 1); 1 runs a sub-state machine on +5
// driving func_00158590 / func_001BC860 / func_001BA1F0 / func_001BC150 /
// func_001AEBA0, then calls func_001B17A0 and the callback at +0x4C; 2 and 3
// call func_001AFC10.

extern void func_00158590(char *self, int a1, int mode);
extern void func_001AEBA0(int);
extern void func_001AFC10(char *self);
extern int func_001B0FD0(char *self);
extern void func_001B17A0(char *);
extern int func_001BA1F0(char *);
extern void func_001BC150(char *);
extern int func_001BC860(char *, char *);
extern void func_001C6380(char *);
extern int D_00275610;
extern float D_008105E0[];
extern float D_008105E4[];
extern float D_008105E8[];
extern unsigned char D_008106B8[];
extern unsigned char D_00810774[];

void func_001BD9F0(char *arg0) {
    char *sub;
    char *owner;
    unsigned char st;
    unsigned char st2;
    short t;

    st = *(unsigned char *)(arg0 + 4);
    owner = *(char **)(arg0 + 0x18);
    sub = arg0 + 0x1F0;
    switch (st) {
    case 0:
        func_001B0FD0(arg0);
        *(short *)(arg0 + 0x34) = *(unsigned char *)(arg0 + 0x2E);
        if (*(unsigned char *)(arg0 + 3) == 3) {
            *(short *)(arg0 + 0x2E) = 1;
        } else {
            *(short *)(arg0 + 0x2E) = 0;
        }
        *(char **)(arg0 + 0x30) = (char *)&D_00275610;
        func_001C6380(arg0);
        *arg0 = 1;
        break;
    case 1:
        st2 = *(unsigned char *)(arg0 + 5);
        switch (st2) {
        case 0:
            if (*(unsigned short *)(arg0 + 0x2E) == 1) {
                if (D_00810774[0] == 1) {
                    *arg0 = 2;
                    func_00158590(arg0, 0, 1);
                } else {
                    *arg0 = 1;
                    func_00158590(arg0, 1, 1);
                }
            } else {
                func_00158590(arg0, 1, 1);
            }
            if (func_001BC860(arg0, sub) != 0) {
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 1:
            func_00158590(arg0, 1, 1);
            if (func_001BA1F0(arg0) != 0) {
                if (*(unsigned char *)(arg0 + 3) == 4) {
                    owner = *(char **)(owner + 0x18);
                }
                D_008105E0[0] = *(float *)(owner + 0xB0);
                D_008105E4[0] = 16.0f + *(float *)(owner + 0xB4);
                D_008105E8[0] = *(float *)(owner + 0xB8);
                *(unsigned char *)(owner + 0xB) = 1;
                *(short *)(arg0 + 0x28) = 0x3C;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 2:
            func_00158590(arg0, 1, 1);
            t = *(short *)(arg0 + 0x28) - 1;
            *(short *)(arg0 + 0x28) = t;
            if (t == 0) {
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_001BC150(arg0);
            }
            break;
        case 3:
            func_00158590(arg0, 1, 1);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            func_001AEBA0(4);
            break;
        case 4:
            func_00158590(arg0, 1, 1);
            if (D_008106B8[0] == 0) {
                *(unsigned char *)(arg0 + 5) = 0;
                *arg0 = 1;
                *(unsigned char *)(arg0 + 0xB) = 0;
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
