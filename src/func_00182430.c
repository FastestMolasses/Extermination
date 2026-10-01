// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player footstep sounds (FINDINGS "FOOTSTEP SURFACE TABLE"; port
// em_player_floor.c). The surface type +0x23A picks a block of sound ids
// (0x10, 0x21, 0x32, 0x43, ... 0xED in steps of 0x11; surface 0x5B picks
// 0xBA or 0xCB by the +0x23C flag; unknown surfaces use surface 0's block),
// the gait (2 = +5, 3 = +10) a sub-base, and func_00179B90 a random variant
// 0..4. The step plays at distance 300 (func_001FBD50), followed by the gear
// layer 0x138 + a variant.
extern int func_00179B90(void);
extern void func_001FBD50(unsigned char *e, int sound, int b, float dist);

void func_00182430(unsigned char *e, unsigned char gait) {
    unsigned char k;
    int id;

    switch (e[0x23A]) {
    case 0:
        k = gait;
        if (k == 3) {
            id = 0x1A;
        } else if (k == 2) {
            id = 0x15;
        } else {
            id = 0x10;
        }
        break;
    case 1:
        k = gait;
        if (k == 3) {
            id = 0x2B;
        } else if (k == 2) {
            id = 0x26;
        } else {
            id = 0x21;
        }
        break;
    case 2:
        k = gait;
        if (k == 3) {
            id = 0x3C;
        } else if (k == 2) {
            id = 0x37;
        } else {
            id = 0x32;
        }
        break;
    case 3:
        k = gait;
        if (k == 3) {
            id = 0x4D;
        } else if (k == 2) {
            id = 0x48;
        } else {
            id = 0x43;
        }
        break;
    case 4:
        k = gait;
        if (k == 3) {
            id = 0x5E;
        } else if (k == 2) {
            id = 0x59;
        } else {
            id = 0x54;
        }
        break;
    case 5:
        k = gait;
        if (k == 3) {
            id = 0x6F;
        } else if (k == 2) {
            id = 0x6A;
        } else {
            id = 0x65;
        }
        break;
    case 0x5A:
        k = gait;
        if (k == 3) {
            id = 0x80;
        } else if (k == 2) {
            id = 0x7B;
        } else {
            id = 0x76;
        }
        break;
    case 8:
        k = gait;
        if (k == 3) {
            id = 0x91;
        } else if (k == 2) {
            id = 0x8C;
        } else {
            id = 0x87;
        }
        break;
    case 0x5C:
        k = gait;
        if (k == 3) {
            id = 0xA2;
        } else if (k == 2) {
            id = 0x9D;
        } else {
            id = 0x98;
        }
        break;
    case 6:
    case 7:
        k = gait;
        if (k == 3) {
            id = 0xB3;
        } else if (k == 2) {
            id = 0xAE;
        } else {
            id = 0xA9;
        }
        break;
    case 0x5B:
        k = gait;
        if (e[0x23C] == 1) {
            if (k == 3) {
                id = 0xC4;
            } else if (k == 2) {
                id = 0xBF;
            } else {
                id = 0xBA;
            }
        } else {
            if (k == 3) {
                id = 0xD5;
            } else if (k == 2) {
                id = 0xD0;
            } else {
                id = 0xCB;
            }
        }
        break;
    case 0xD:
        k = gait;
        if (k == 3) {
            id = 0xE6;
        } else if (k == 2) {
            id = 0xE1;
        } else {
            id = 0xDC;
        }
        break;
    case 0xE:
        k = gait;
        if (k == 3) {
            id = 0xF7;
        } else if (k == 2) {
            id = 0xF2;
        } else {
            id = 0xED;
        }
        break;
    default:
        k = gait;
        if (k == 3) {
            id = 0x1A;
        } else if (k == 2) {
            id = 0x15;
        } else {
            id = 0x10;
        }
        break;
    }
    func_001FBD50(e, id + func_00179B90(), 0, 300.0f);
    func_001FBD50(e, func_00179B90() + 0x138, 0, 300.0f);
}
