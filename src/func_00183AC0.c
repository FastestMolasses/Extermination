// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
int func_00183AC0(unsigned char *e) {
    if ((e[2] & ~0xE0) == 2) {
        switch (e[3]) {
        case 0x06:
            return e[0x9F] == 0 ? 1 : 0;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x0F:
        case 0x0D:
        case 0x0E:
            return 0;
        default:
            return 1;
        }
    }
    return 0;
}
