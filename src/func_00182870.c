// CFLAGS: -O4,p -sdatathreshold 0
//
// Selects a sound/voice id from a per-entity state byte at p[0x23A] and an
// arg1 variant flag, then plays it via func_001FBD50(p, id, 0, 300.0f). Each
// state maps to a high/low id pair chosen by arg1; states 0x5A/0x5B/0x5C are
// played only when arg1==0 (else early return). State 0x5B further branches on
// the sub-flag p[0x23C].
extern void func_001FBD50(char *p, int id, int b, float f);

void func_00182870(char *arg0, int arg1) {
    int id;

    switch (*(unsigned char *)(arg0 + 0x23A)) {
    case 0:
        if (arg1 == 0) { id = 0x1F; } else { id = 0x20; }
        break;
    case 1:
        if (arg1 == 0) { id = 0x30; } else { id = 0x31; }
        break;
    case 2:
        if (arg1 == 0) { id = 0x41; } else { id = 0x42; }
        break;
    case 3:
        if (arg1 == 0) { id = 0x52; } else { id = 0x53; }
        break;
    case 4:
        if (arg1 == 0) { id = 0x63; } else { id = 0x64; }
        break;
    case 5:
        if (arg1 == 0) { id = 0x74; } else { id = 0x75; }
        break;
    case 0x5A:
        if (arg1) return;
        id = 0x85;
        break;
    case 8:
        if (arg1 == 0) { id = 0x96; } else { id = 0x97; }
        break;
    case 0x5C:
        if (arg1) return;
        id = 0xA7;
        break;
    case 6:
    case 7:
        if (arg1 == 0) { id = 0xB8; } else { id = 0xB9; }
        break;
    case 0x5B:
        if (*(unsigned char *)(arg0 + 0x23C) == 1) {
            if (arg1) return;
            id = 0xC9;
        } else {
            if (arg1) return;
            id = 0xDA;
        }
        break;
    case 0xD:
        if (arg1 == 0) { id = 0xEB; } else { id = 0xEC; }
        break;
    case 0xE:
        if (arg1 == 0) { id = 0xFC; } else { id = 0xFD; }
        break;
    default:
        if (arg1 == 0) { id = 0x1F; } else { id = 0x20; }
        break;
    }
    func_001FBD50(arg0, id, 0, 300.0f);
}
