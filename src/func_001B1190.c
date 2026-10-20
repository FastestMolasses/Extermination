// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sets bit (id & 0x1F) of word (id & 0xFF) >> 5 in row D_00810700 of the
// bitset D_00810860 (8 words per row); id & 0xFF == 0 does nothing.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

extern unsigned char D_00810700;
extern int D_00810860[][8];
void func_001B1190(int id) {
    int n = id & 0xFF;
    if (n != 0) {
        D_00810860[D_00810700][n >> 5] |= 1 << (id & 0x1F);
    }
}
