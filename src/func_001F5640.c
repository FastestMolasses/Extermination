// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Lookup table: maps the packed 16-bit key (D_00810700<<8)+D_00810701 to a
// pointer-to-global, returning NULL for unmapped keys. mwcc lowers this sparse
// switch to a linear beq compare-chain (no jump table), so the case ordering is
// load-bearing: pointer-returning cases first (ascending key), all NULL-returning
// keys last sharing the default's `paddub v0,zero,zero` exit. 0x600/0x601 must be
// two separate return statements (the target emits two identical D_0025C400 blocks).
extern char D_0025AD80[];
extern char D_0025B0D0[];
extern char D_0025B330[];
extern char D_0025B4F0[];
extern char D_0025B590[];
extern char D_0025B770[];
extern char D_0025BCF0[];
extern char D_0025BDE0[];
extern char D_0025BF20[];
extern char D_0025C130[];
extern char D_0025C400[];
extern char D_0025C4D0[];
extern char D_0025C820[];
extern char D_0025C8F0[];
extern char D_0025C9C0[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;

char *func_001F5640(void) {
    switch ((D_00810700 << 8) + D_00810701) {
    case 0x0:
        return D_0025AD80;
    case 0x1:
        return D_0025B0D0;
    case 0x700:
        return D_0025B330;
    case 0x703:
        return D_0025B4F0;
    case 0xB00:
        return D_0025B590;
    case 0xD00:
        return D_0025B770;
    case 0xE00:
        return D_0025BCF0;
    case 0xF00:
        return D_0025BDE0;
    case 0xF01:
        return D_0025BF20;
    case 0x1100:
        return D_0025C130;
    case 0x600:
        return D_0025C400;
    case 0x601:
        return D_0025C400;
    case 0x1300:
        return D_0025C4D0;
    case 0x1400:
        return D_0025C820;
    case 0x1500:
        return D_0025C8F0;
    case 0x802:
        return D_0025C9C0;
    case 0x2:
    case 0x100:
    case 0x101:
    case 0x200:
    case 0x201:
    case 0x202:
    case 0x300:
    case 0x301:
    case 0x400:
    case 0x401:
    case 0x500:
    case 0x701:
    case 0x702:
    case 0x704:
    case 0x800:
    case 0x801:
    case 0x803:
    case 0x804:
    case 0x805:
    case 0x806:
    case 0x900:
    case 0xA00:
    case 0xC00:
    case 0x1000:
    case 0x1001:
    case 0x1200:
    case 0x1301:
    case 0x1600:
    default:
        return 0;
    }
}
