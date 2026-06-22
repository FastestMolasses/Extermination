// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// If the object's type byte at +3 is not 0x2C, OR a per-object bit into a
// shared status byte (table base D_00810841 indexed by the global selector
// D_00810700; bit = 1 << object id byte at +0x2E), then spawn/queue an
// effect via func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000). Always returns 1.
extern void func_001FB9F0(int, int, int, int);
extern unsigned char D_00810700;
extern unsigned char D_00810841;

int func_001580C0(unsigned char *arg0) {
    unsigned char *p;
    if (arg0[3] != 0x2C) {
        p = &D_00810841 + D_00810700;
        *p |= 1 << arg0[0x2E];
        func_001FB9F0(0x3EE, 0x1000, 0x1000, 0x1000);
    }
    return 1;
}
