// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Actor AI top-level state dispatcher. arg0+0x48 selects a debug-tag variant
// (0/1) used only to color the OSD sub-state string (func_001FCBD0 tag
// 3 or 4); arg0+1 is the actual behavior-mode byte (0..4) dispatched below:
//   mode 0/1: tag the OSD (sub-state 9), run the "approach" evaluator
//     (func_002280F0/func_002283B0) and transition arg0+1/arg0+2 per its
//     result code (0..4).
//   mode 2: run sub__02d_02d_02d (a housekeeping pass), then dispatch on the
//     sub-state byte arg0+2 (0..4) into the melee/attack sub-machine, each
//     branch running its own evaluator (func_00229170/func_00227980/the
//     grapple-vs-strike selector) and transitioning arg0+1/arg0+2 again;
//     stamina meter D_00810CB2 is nudged +-4 on some transitions when the
//     actor's stance byte (arg0+0xA -> arg0[0xA]+0x6C) reads 1 or 2.
//   mode 4: dispatch on arg0+2 (0..3) straight into the four finisher/hit
//     reaction handlers.
extern int func_001FCBD0(int a, int b, int c, int d, int e);
extern void func_001FE8D0(void);
extern void func_00225A20(void);
extern void func_00225CF0(unsigned char *a0, int a1, int a2);
extern int func_002267A0(char *arg0);
extern int func_00226B90(char *arg0);
extern int func_00226D50(char *arg0);
extern int func_00226E90(char *arg0);
extern int func_00227010(unsigned char *arg0);
extern int func_00227160(unsigned char *arg0);
extern int func_00227980(unsigned char *arg0);
extern int func_002280F0(unsigned char *p);
extern int func_002283B0(char *arg0);
extern int func_00228A90(unsigned char *arg0);
extern int func_00229170(char *arg0);
extern int sub_BASCUS_97112_DS00_00_EX_DATA_00_4(unsigned char *arg0);
extern int sub__02d_02d_02d(unsigned char *arg0);
extern int sub_s_EX_DATA_02d(unsigned char *arg0);
extern short D_00810CB2;

int func_00227300(unsigned char *arg0) {
    int r;
    int approach;

    switch (*(int *)(arg0 + 0x48)) {
    case 0:
        func_001FCBD0(0x18, 0x20, 7, 3, 0x70808080);
        break;
    case 1:
        func_001FCBD0(0x18, 0x20, 7, 4, 0x70808080);
        break;
    }

    switch (arg0[1]) {
    case 0:
        func_001FCBD0(0x3C, 0x9B, 7, 9, 0x70808080);
        approach = func_002280F0(arg0);
        switch (approach) {
        case 0:
            break;
        case 1:
            arg0[1] = 1;
            func_00225CF0(arg0, 2, 7);
            break;
        case 2:
            arg0[1] = 4;
            arg0[2] = 0;
            break;
        case 3:
            arg0[1] = 4;
            arg0[2] = 1;
            break;
        case 4:
            arg0[1] = 4;
            arg0[2] = 2;
            break;
        }
        return 0;

    case 1:
        func_001FCBD0(0x3C, 0x9B, 7, 9, 0x70808080);
        approach = func_002283B0((char *)arg0);
        switch (approach) {
        case 0:
            break;
        case 1:
            arg0[1] = 2;
            func_00225CF0(arg0, 2, 7);
            break;
        case 2:
            arg0[1] = 4;
            arg0[2] = 2;
            break;
        case 3:
            arg0[1] = 4;
            arg0[2] = 3;
            break;
        }
        return 0;

    case 2:
        sub__02d_02d_02d(arg0);
        switch (arg0[2]) {
        case 0:
            r = func_00229170((char *)arg0);
            switch (r) {
            case 1:
                arg0[2] = 1;
                func_00225CF0(arg0, 3, 7);
                *(int *)(arg0 + 0x64) = 0;
                break;
            case 2:
                return 1;
            case 3:
                return 2;
            case 4:
                arg0[0x15] = 1;
                arg0[1] = 1;
                func_00225A20();
                func_00225CF0(arg0, 2, 7);
                func_001FE8D0();
                break;
            default:
                break;
            }
            return 0;

        case 1:
            r = func_00227980(arg0);
            switch (r) {
            case 0:
                break;
            case 1:
                arg0[2] = 0;
                func_00225CF0(arg0, 3, 7);
                break;
            case 2:
                return 2;
            case 3:
                arg0[2] = 2;
                if (arg0[0x14] == 1) {
                    D_00810CB2 -= 4;
                }
                func_00225CF0(arg0, 3, 7);
                break;
            case 4:
                arg0[0x15] = 1;
                arg0[1] = 1;
                func_00225A20();
                func_001FE8D0();
                func_00225CF0(arg0, 2, 7);
                break;
            }
            return 0;

        case 2: {
            int variant;

            if (arg0[0x14] == 1 && *(arg0 + arg0[0xA] + 0x6C) == 2) {
                func_001FCBD0(0x3C, 0x9B, 7, 0x22, 0x70808080);
                variant = sub_BASCUS_97112_DS00_00_EX_DATA_00_4(arg0);
            } else if (arg0[0x14] == 1) {
                func_001FCBD0(0x3C, 0x9B, 7, 0x10, 0x70808080);
                variant = func_00228A90(arg0);
            } else {
                func_001FCBD0(0x3C, 0x9B, 7, 0x16, 0x70808080);
                variant = sub_s_EX_DATA_02d(arg0);
            }

            if (arg0[0x14] == 1 && *(arg0 + arg0[0xA] + 0x6C) == 2) {
                switch (variant) {
                case 0:
                    break;
                case 1:
                    arg0[2] = 3;
                    D_00810CB2 += 4;
                    func_00225CF0(arg0, 3, 7);
                    break;
                case 2:
                    D_00810CB2 += 4;
                    arg0[2] = 4;
                    func_00225CF0(arg0, 3, 7);
                    break;
                }
            } else {
                switch (variant) {
                case 0:
                    break;
                case 1:
                    arg0[2] = 3;
                    func_00225CF0(arg0, 3, 7);
                    break;
                case 2:
                    if (arg0[0x14] == 1) {
                        D_00810CB2 += 4;
                    }
                    arg0[2] = 4;
                    func_00225CF0(arg0, 3, 7);
                    break;
                }
            }
            break;
        }

        case 3:
            return func_00227010(arg0);

        case 4:
            return func_00227160(arg0);

        default:
            break;
        }
        break;

    case 3:
        break;

    case 4:
        switch (arg0[2]) {
        case 0:
            return func_00226E90((char *)arg0);
        case 1:
            return func_00226B90((char *)arg0);
        case 2:
            return func_002267A0((char *)arg0);
        case 3:
            return func_00226D50((char *)arg0);
        }
        break;
    }

    return 0;
}
