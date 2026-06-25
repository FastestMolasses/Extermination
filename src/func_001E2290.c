// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Returns 1 if arg0 is one of a fixed set of key/scan codes, else 0.
int func_001E2290(int arg0) {
    switch (arg0) {
    case 0x3B:
    case 0x3D:
    case 0x3E:
    case 0x3F:
    case 0x40:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x54:
    case 0x55:
    case 0x58:
    case 0x59:
    case 0x5A:
    case 0x61:
    case 0x68:
    case 0x6A:
        return 1;
    default:
        return 0;
    }
}
