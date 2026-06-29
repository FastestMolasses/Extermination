// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Maps an item/object id to a small category code (1..0xC), default 9.

int func_001CBE10(int id) {
    switch (id) {
    case 0x20: return 5;
    case 0x21: return 4;
    case 0x27: return 4;
    case 0x28: return 8;
    case 0x29: return 8;
    case 0x2C: return 6;
    case 0x2E: return 5;
    case 0x2F: return 8;
    case 0x3A: return 7;
    case 0x3B: return 8;
    case 0x49: return 4;
    case 0x4A: return 7;
    case 0x4D: return 0xC;
    case 0x57: return 0xC;
    case 0x5B: return 8;
    case 0x5C: return 8;
    case 0x5D: return 8;
    case 0x60: return 5;
    case 0x66: return 8;
    case 0x69: return 4;
    case 0x6A: return 6;
    case 0x6C: return 4;
    case 0x6D: return 0xC;
    case 0x72: return 9;
    case 0x77: return 0xC;
    case 0x82: return 6;
    case 0x84: return 8;
    case 0x8B: return 9;
    case 0x91: return 6;
    case 0x92: return 6;
    case 0x93: return 8;
    case 0x94: return 8;
    case 0x9B: return 9;
    case 0xA1: return 4;
    case 0xA6: return 6;
    default: return 9;
    }
}
