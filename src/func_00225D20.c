// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B93

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.30% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad byte 0x70003B93 (read by the result==2 arms of states 1
//    and 2) is a relocated extern (idiom-32, opted in with // SPAD:), not a
//    literal address. As a literal, mwcc CSE'd its 0x7000 upper half and
//    speculated it into a branch delay slot the original leaves empty; the
//    "compiler wall" verdict was wrong.
//
// Actor per-frame state-machine update, dispatched on the state byte at
// arg0+0x15 (switch 1: 0..3). State 0: if the busy flag arg0+0x19 is set,
// plays SFX via func_001FCBD0 twice then dispatches a nested switch on the
// sub-state byte arg0+9 (switch 2: 0/1) which drives D_00810E74 bit tests
// (0x40/0x20/0x10/0x2000/0x8000) via func_0020CD40/func_0020CD60/
// func_0020CDA0, clearing/toggling arg0+9 and arg0+0x19, returning early on
// a 0x40 abort. If arg0+0x19 is clear, advances state to 1 and kicks off
// func_00225A20 + func_00225CF0(arg0,1,7). State 1: calls func_00226070 and
// switches on the result (switch 3): 1 advances state and calls
// func_00225CF0, clearing arg0+0x20; 2 sets/clears arg0+0x19 and either
// backs up or forces state 3 based on the byte at 0x70003B93. State 2:
// calls func_00227300 and switches on the result (switch 4): 1 backs up
// state and calls func_00225CF0+func_00225A20; 2 mirrors state 1's case 2;
// 3 returns 2. State 3: resets arg0+0x15 to 0 and returns 1. All paths
// otherwise fall through to a shared "return 0" tail.
extern int func_001FCBD0(int a, int b, int c, int d, int e);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern void func_002256E0(void);
extern void func_00225700(void);
extern void func_00225A20(void);
extern void func_00225CF0(char *p, int a, int b);
extern int func_00226070(void);
extern int func_00227300(void);
extern unsigned short D_00810E74;
extern unsigned char D_70003B93;

int func_00225D20(char *arg0) {
    unsigned char st;
    unsigned char s9;
    int r;

    st = *(unsigned char *)(arg0 + 0x15);
    switch (st) {
    case 0:
        if (*(unsigned char *)(arg0 + 0x19) != 0) {
            func_001FCBD0(0x3C, 0x9B, 7, 0x21, 0x70808080);
            func_001FCBD0(0x8C, 0xC3, 7, 2, 0x70808080);
            s9 = *(unsigned char *)(arg0 + 9);
            switch (s9) {
            case 0:
                func_002256E0();
                if (D_00810E74 & 0x40) {
                    func_0020CD40();
                    return 2;
                }
                break;
            case 1:
                func_00225700();
                if (D_00810E74 & 0x40) {
                    *(unsigned char *)(arg0 + 0x19) = 0;
                    func_0020CD40();
                    return 0;
                }
                break;
            }
            if (D_00810E74 & 0x20) {
                func_0020CD60();
                *(unsigned char *)(arg0 + 0x19) = 0;
            } else if (D_00810E74 & 0x10) {
                func_0020CD60();
                *(unsigned char *)(arg0 + 0x19) = 0;
            } else {
                if ((D_00810E74 & 0x2000) && *(unsigned char *)(arg0 + 9) == 0) {
                    func_0020CDA0();
                    *(unsigned char *)(arg0 + 9) = 1;
                }
                if ((D_00810E74 & 0x8000) && *(unsigned char *)(arg0 + 9) != 0) {
                    func_0020CDA0();
                    *(unsigned char *)(arg0 + 9) = 0;
                }
            }
        } else {
            *(unsigned char *)(arg0 + 0x15) = st + 1;
            func_00225A20();
            func_00225CF0(arg0, 1, 7);
        }
        break;
    case 1:
        r = func_00226070();
        switch (r) {
        case 1:
            *(unsigned char *)(arg0 + 0x15) = *(unsigned char *)(arg0 + 0x15) + 1;
            func_00225CF0(arg0, 1, 7);
            *(int *)(arg0 + 0x20) = 0;
            break;
        case 2:
            if (D_70003B93 != 0) {
                *(unsigned char *)(arg0 + 0x19) = 1;
                *(unsigned char *)(arg0 + 0x15) = *(unsigned char *)(arg0 + 0x15) - 1;
            } else {
                *(unsigned char *)(arg0 + 0x15) = 3;
            }
            break;
        }
        break;
    case 2:
        r = func_00227300();
        switch (r) {
        case 1:
            *(unsigned char *)(arg0 + 0x15) = *(unsigned char *)(arg0 + 0x15) - 1;
            func_00225CF0(arg0, 1, 7);
            func_00225A20();
            break;
        case 2:
            if (D_70003B93 != 0) {
                *(unsigned char *)(arg0 + 0x19) = 1;
                *(unsigned char *)(arg0 + 0x15) = 0;
            } else {
                *(unsigned char *)(arg0 + 0x15) = 3;
            }
            break;
        case 3:
            return 2;
        }
        break;
    case 3:
        *(unsigned char *)(arg0 + 0x15) = 0;
        return 1;
    }
    return 0;
}
