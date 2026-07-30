// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: selects a script/command table for an entity and primes its
// cursor.  `e` is the entity; `q = *(e+0x24)` is its linked descriptor record;
// `base = e + 0x1F0` is the sub-object that owns the table pointer (+0x90) and
// the entry count (+0x98).
//   e[0x0D] (0..9) dispatches through jtbl_00273EF0:
//     2..8      -> a fixed table
//     0         -> table chosen by q[3] (12/1, 4, 5, 6, 7, 16); else return 0
//     1         -> table chosen by q[3] (0, 5, 7); q[3]==12 or 1 picks
//                  D_00268020 when the global mode byte D_008101E4 == 3,
//                  otherwise D_00267E30 / D_00267EB0 on bit 0 of q[0x0D];
//                  else return 0
//     9         -> if (q[2] & ~0xE0) == 0 use D_00267940, otherwise q[3]
//                  (0..18) dispatches through jtbl_00273EA0; 8 and 12..15
//                  are holes -> return 0
//     anything else (>= 10) -> return 0
// Then it walks the selected table (8-byte records) until the signed short at
// record+2 equals 8 (the terminator), stores index+1 to base+0x98 and
// returns 1.

extern short D_002677B0[];
extern short D_002677D0[];
extern short D_00267870[];
extern short D_002678A0[];
extern short D_002678D0[];
extern short D_002678F0[];
extern short D_00267910[];
extern short D_00267310[];
extern short D_00267470[];
extern short D_002673B0[];
extern short D_00267540[];
extern short D_00267660[];
extern short D_00267760[];
extern short D_00267E00[];
extern short D_00267F30[];
extern short D_00267F90[];
extern short D_00268020[];
extern short D_00267E30[];
extern short D_00267EB0[];
extern short D_00267940[];
extern short D_00267980[];
extern short D_002679C0[];
extern short D_00267AA0[];
extern short D_00267A30[];
extern short D_00267AF0[];
extern short D_00267B90[];
extern short D_00267C00[];
extern short D_00267C80[];
extern short D_00267CF0[];
extern short D_00267D60[];
extern short D_00267DA0[];
extern short D_00267D90[];
extern short D_00267DD0[];
extern unsigned char D_008101E4;

int func_0022B7A0(char *e) {
    char *q;
    char *base;
    short *r;
    int i;
    short *m;

    q = *(char **)(e + 0x24);
    base = e + 0x1F0;
    switch (*(unsigned char *)(e + 0xD)) {
    case 2:
        *(short **)(base + 0x90) = D_002677B0;
        break;
    case 3:
        *(short **)(base + 0x90) = D_002677D0;
        break;
    case 4:
        *(short **)(base + 0x90) = D_00267870;
        break;
    case 5:
        *(short **)(base + 0x90) = D_002678A0;
        break;
    case 6:
        *(short **)(base + 0x90) = D_002678D0;
        break;
    case 7:
        *(short **)(base + 0x90) = D_002678F0;
        break;
    case 8:
        *(short **)(base + 0x90) = D_00267910;
        break;
    case 0:
        switch (*(unsigned char *)(q + 3)) {
        case 12:
            *(short **)(base + 0x90) = D_00267310;
            break;
        case 1:
            *(short **)(base + 0x90) = D_00267310;
            break;
        case 4:
            *(short **)(base + 0x90) = D_00267470;
            break;
        case 5:
            *(short **)(base + 0x90) = D_002673B0;
            break;
        case 6:
            *(short **)(base + 0x90) = D_00267540;
            break;
        case 7:
            *(short **)(base + 0x90) = D_00267660;
            break;
        case 16:
            *(short **)(base + 0x90) = D_00267760;
            break;
        default:
            return 0;
        }
        break;
    case 1:
        switch (*(unsigned char *)(q + 3)) {
        case 0:
            *(short **)(base + 0x90) = D_00267E00;
            break;
        case 5:
            *(short **)(base + 0x90) = D_00267F30;
            break;
        case 7:
            *(short **)(base + 0x90) = D_00267F90;
            break;
        case 12:
        case 1:
            if (D_008101E4 == 3) {
                *(short **)(base + 0x90) = D_00268020;
            } else {
                if (*(unsigned char *)(q + 0xD) & 1) {
                    r = D_00267E30;
                } else {
                    r = D_00267EB0;
                }
                *(short **)(base + 0x90) = r;
            }
            break;
        default:
            return 0;
        }
        break;
    case 9:
        if ((*(unsigned char *)(q + 2) & ~0xE0) == 0) {
            *(short **)(base + 0x90) = D_00267940;
        } else {
            switch (*(unsigned char *)(q + 3)) {
            case 0:
                *(short **)(base + 0x90) = D_00267980;
                break;
            case 1:
                *(short **)(base + 0x90) = D_002679C0;
                break;
            case 4:
                *(short **)(base + 0x90) = D_00267AA0;
                break;
            case 5:
                *(short **)(base + 0x90) = D_00267A30;
                break;
            case 6:
                *(short **)(base + 0x90) = D_00267AF0;
                break;
            case 7:
                *(short **)(base + 0x90) = D_00267B90;
                break;
            case 3:
                *(short **)(base + 0x90) = D_00267940;
                break;
            case 2:
                *(short **)(base + 0x90) = D_00267C00;
                break;
            case 10:
                *(short **)(base + 0x90) = D_00267C80;
                break;
            case 11:
                *(short **)(base + 0x90) = D_00267CF0;
                break;
            case 9:
                *(short **)(base + 0x90) = D_00267D60;
                break;
            case 16:
                *(short **)(base + 0x90) = D_00267DA0;
                break;
            case 17:
                *(short **)(base + 0x90) = D_00267D90;
                break;
            case 18:
                *(short **)(base + 0x90) = D_00267DD0;
                break;
            default:
                return 0;
            }
        }
        break;
    default:
        return 0;
    }
    m = *(short **)(base + 0x90);
    i = 0;
loop:
    if (m[1] == 8) {
        *(int *)(base + 0x98) = i + 1;
        goto done;
    }
    m += 4;
    i++;
    goto loop;
done:
    return 1;
}
