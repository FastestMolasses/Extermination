// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 62.50% NEARMISS).
// Fixes read from the original instructions:
//  - The arg0+7 sub-state is a switch (case 0, case 1; any other value
//    returns 0), and every func_001FECB0 result is a switch instead of an
//    if-chain with a separate -1 test.
//  - Sub-state 0 query: -2/-3/0 share one arm (result record == 2: call
//    func_001FE8D0, set the flag, return 1), -1 returns 0, default clears the
//    flag; all non-returning paths then set the sub-state to 1.
//  - Sub-state 1 query: -2/-3 (record == 2: func_001FE8D0, set the flag,
//    return 1) and 0 (flag = record == 2) are separate arms, -1 returns 0,
//    default clears the flag; the rest then reset the sub-state to 0.
//  - -sdatathreshold 4, as in its byte-matched caller func_00226070
//    (D_00275C58 gp-relative, D_00810E74 absolute).
//
// Weapon-swap confirm step (state 1 of func_00226070). Draws prompt 0xF or
// 0xE depending on the choice at arg0+0x48; the D_00810E74 input flags 0x40
// (returns 1) and 0x20/0x10 (returns 2) end it. Otherwise bumps the counter
// at arg0+0x10 and, once func_001FE9A0 is ready, alternately runs the two
// func_001FECB0 queries (sub-state on arg0+7, order depending on arg0+0x48)
// that set the flags at arg0+0x28 / arg0+0x2C from the result record at
// arg0+0x58. Returns 0 while still running.

extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_00226010(char *);
extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_002267A0(char *arg0) {
    if (*(int *)(arg0 + 0x48) != 0) {
        func_001FCBD0(0x3C, 0x9B, 7, 0xF, 0x70808080);
    } else {
        func_001FCBD0(0x3C, 0x9B, 7, 0xE, 0x70808080);
    }

    if (D_00810E74[0] & 0x40) {
        func_0020CD40();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x20) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }
    if (D_00810E74[0] & 0x10) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }

    *(int *)(arg0 + 0x10) = *(int *)(arg0 + 0x10) + 1;
    if (D_00275C58 != 0) {
        func_00226010(arg0);
        return 0;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }

    if (*(int *)(arg0 + 0x48) != 0) {
        switch (*(unsigned char *)(arg0 + 7)) {
        case 0:
            switch (func_001FECB0(1, 0, arg0 + 0x58, 0, 0)) {
            case -2:
            case -3:
            case 0:
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x2C) = 1;
                    return 1;
                }
                break;
            case -1:
                return 0;
            default:
                *(int *)(arg0 + 0x2C) = 0;
                break;
            }
            *(unsigned char *)(arg0 + 7) = 1;
            return 0;
        case 1:
            switch (func_001FECB0(0, 0, arg0 + 0x58, 0, 0)) {
            case -2:
            case -3:
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x28) = 1;
                    return 1;
                }
                break;
            case -1:
                return 0;
            case 0:
                if (*(int *)(arg0 + 0x58) == 2) {
                    *(int *)(arg0 + 0x28) = 1;
                } else {
                    *(int *)(arg0 + 0x28) = 0;
                }
                break;
            default:
                *(int *)(arg0 + 0x28) = 0;
                break;
            }
            *(unsigned char *)(arg0 + 7) = 0;
            return 0;
        }
    } else {
        switch (*(unsigned char *)(arg0 + 7)) {
        case 0:
            switch (func_001FECB0(0, 0, arg0 + 0x58, 0, 0)) {
            case -2:
            case -3:
            case 0:
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x28) = 1;
                    return 1;
                }
                break;
            case -1:
                return 0;
            default:
                *(int *)(arg0 + 0x28) = 0;
                break;
            }
            *(unsigned char *)(arg0 + 7) = 1;
            return 0;
        case 1:
            switch (func_001FECB0(1, 0, arg0 + 0x58, 0, 0)) {
            case -2:
            case -3:
                if (*(int *)(arg0 + 0x58) == 2) {
                    func_001FE8D0();
                    *(int *)(arg0 + 0x2C) = 1;
                    return 1;
                }
                break;
            case -1:
                return 0;
            case 0:
                if (*(int *)(arg0 + 0x58) == 2) {
                    *(int *)(arg0 + 0x2C) = 1;
                } else {
                    *(int *)(arg0 + 0x2C) = 0;
                }
                break;
            default:
                *(int *)(arg0 + 0x2C) = 0;
                break;
            }
            *(unsigned char *)(arg0 + 7) = 0;
            return 0;
        }
    }
    return 0;
}
