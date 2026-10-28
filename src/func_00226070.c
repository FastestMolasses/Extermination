// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 69.23% NEARMISS).
// Fixes read from the original instructions:
//  - -sdatathreshold 4 with D_00810E74 as an 8-entry array: D_00275C58 is
//    gp-relative, the input-flag word stays absolute.
//  - The arg0+2 sub-state and every func_001FECB0 result are switches; the
//    result switches list -3/-2 first (func_001FE8D0 then falls into the -1
//    `return 0`), then -1, then 0, then the default (2.3.3 emits the case
//    compares in reverse source order).
//  - State 1: any func_002267A0 result other than 0/1/2 returns 0 (the old C
//    returned the result itself).
//  - States 3 and 4 just break to the shared `return 0`.
//  - State 2: the 0x48-selection float_to_int temporaries are block locals
//    declared c3..c0 (saved-register order).
//
// Larger weapon-swap FSM (state byte on arg0+1). State 0 runs two
// func_001FECB0 queries (sub-state on arg0+2) that set the flags at
// arg0+0x28 / arg0+0x2C from the result record at arg0+0x58, then moves to
// state 2. State 1 defers to func_002267A0. State 2 draws the selection,
// steps the choice at arg0+0x48 on the D_00810E74 input flags, latches the
// confirm/cancel buttons into arg0+0x17, and alternately re-queries the two
// flags (counter at arg0+0x24). Sibling of func_002267A0.

extern int float_to_int(float);
extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, char *, char *);
extern int func_001FECB0(int, int, char *, int, int);
extern void func_00207F80(int, int, int, int, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern void func_00226010(char *);
extern int func_002267A0(char *);

extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_00226070(char *arg0) {
    int v;

    switch (*(unsigned char *)(arg0 + 1)) {
    case 0:
        func_001FCBD0(0x3C, 0x9B, 7, 9, 0x70808080);
        if (D_00275C58 != 0) {
            func_00226010(arg0);
            return 0;
        }
        if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
            return 0;
        }
        switch (*(unsigned char *)(arg0 + 2)) {
        case 0:
            switch (func_001FECB0(0, 0, arg0 + 0x58, 0, 0)) {
            case -3:
            case -2:
                func_001FE8D0();
            case -1:
                return 0;
            case 0:
                *(unsigned char *)(arg0 + 2) = 1;
                if (*(int *)(arg0 + 0x58) == 2) {
                    *(int *)(arg0 + 0x28) = 1;
                } else {
                    *(int *)(arg0 + 0x28) = 0;
                }
                break;
            default:
                *(unsigned char *)(arg0 + 2) = 1;
                *(int *)(arg0 + 0x28) = 0;
                break;
            }
            break;
        case 1:
            switch (func_001FECB0(1, 0, arg0 + 0x58, 0, 0)) {
            case -3:
            case -2:
                func_001FE8D0();
            case -1:
                return 0;
            case 0:
                *(unsigned char *)(arg0 + 1) = 2;
                *(unsigned char *)(arg0 + 2) = 0;
                if (*(int *)(arg0 + 0x58) == 2) {
                    *(int *)(arg0 + 0x2C) = 1;
                } else {
                    *(int *)(arg0 + 0x2C) = 0;
                }
                break;
            default:
                *(unsigned char *)(arg0 + 1) = 2;
                *(int *)(arg0 + 0x2C) = 0;
                break;
            }
            break;
        }
        break;

    case 1:
        switch (func_002267A0(arg0)) {
        case 0:
            return 0;
        case 1:
            *(unsigned char *)(arg0 + 1) = 2;
            return 0;
        case 2:
            return 2;
        }
        break;

    case 2:
        if (*(int *)(arg0 + 0x28) != 0 || *(int *)(arg0 + 0x2C) != 0) {
            func_001FCBD0(0x3C, 0x9B, 7, 0xA, 0x70808080);
            if (*(int *)(arg0 + 0x28) != 0) {
                func_001FCBD0(0x64, 0xAF, 7, 3, 0x70808080);
                if (*(int *)(arg0 + 0x48) == 0) {
                    int c3;
                    int c2;
                    int c1;
                    int c0;
                    c0 = float_to_int(29952.0f);
                    c1 = float_to_int(33808.0f);
                    c2 = float_to_int(30144.0f);
                    c3 = float_to_int(33904.0f);
                    func_00207F80(1, c0, c1, c2, c3, 0x80CE6000);
                }
            }
            if (*(int *)(arg0 + 0x2C) != 0) {
                func_001FCBD0(0x64, 0xB9, 7, 4, 0x70808080);
                if (*(int *)(arg0 + 0x48) == 1) {
                    int c3;
                    int c2;
                    int c1;
                    int c0;
                    c0 = float_to_int(29952.0f);
                    c1 = float_to_int(33968.0f);
                    c2 = float_to_int(30144.0f);
                    c3 = float_to_int(34064.0f);
                    func_00207F80(1, c0, c1, c2, c3, 0x80CE6000);
                }
            }
        } else {
            func_001FCBD0(0x3C, 0x9B, 7, 0xB, 0x70808080);
        }

        if (*(int *)(arg0 + 0x28) != 0 && *(int *)(arg0 + 0x2C) != 0) {
            if ((D_00810E74[0] & 0x1000) && *(int *)(arg0 + 0x48) > 0) {
                func_0020CDA0();
                *(int *)(arg0 + 0x48) -= 1;
            }
            if ((D_00810E74[0] & 0x4000) && *(int *)(arg0 + 0x48) <= 0) {
                func_0020CDA0();
                *(int *)(arg0 + 0x48) += 1;
            }
        } else if (*(int *)(arg0 + 0x28) != 0) {
            *(int *)(arg0 + 0x48) = 0;
        } else if (*(int *)(arg0 + 0x2C) != 0) {
            *(int *)(arg0 + 0x48) = 1;
        }

        if (D_00810E74[0] & 0x40) {
            *(unsigned char *)(arg0 + 0x17) = 0x40;
        } else if (D_00810E74[0] & 0x20) {
            *(unsigned char *)(arg0 + 0x17) = 0x20;
        } else if (D_00810E74[0] & 0x10) {
            *(unsigned char *)(arg0 + 0x17) = 0x10;
        }

        if (D_00275C58 != 0) {
            func_00226010(arg0);
            return 0;
        }
        if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
            return 0;
        }

        if (*(unsigned char *)(arg0 + 0x17) & 0x20) {
            func_0020CD60();
            func_001FE8D0();
            *(unsigned char *)(arg0 + 0x17) = 0;
            return 2;
        }
        if (*(unsigned char *)(arg0 + 0x17) & 0x10) {
            func_0020CD60();
            func_001FE8D0();
            *(unsigned char *)(arg0 + 0x17) = 0;
            return 2;
        }
        if (*(unsigned char *)(arg0 + 0x17) & 0x40) {
            func_0020CD40();
            *(unsigned char *)(arg0 + 0x17) = 0;
            if (*(int *)(arg0 + 0x28) != 0 || *(int *)(arg0 + 0x2C) != 0) {
                func_001FE8D0();
                return 1;
            }
        }

        if (*(int *)(arg0 + 0x24) & 1) {
            v = func_001FECB0(1, 0, arg0 + 0x58, 0, 0);
            switch (v) {
            case -3:
            case -2:
                func_001FE8D0();
                break;
            case -1:
                break;
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
        } else {
            v = func_001FECB0(0, 0, arg0 + 0x58, 0, 0);
            switch (v) {
            case -3:
            case -2:
                func_001FE8D0();
                break;
            case -1:
                break;
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
        }
        if (v != -1) {
            *(int *)(arg0 + 0x24) += 1;
        }
        break;

    case 3:
        break;
    case 4:
        break;
    }
    return 0;
}
