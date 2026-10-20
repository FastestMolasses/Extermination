// COMPILER: eegcc
// CFLAGS: -O2
//
// Returns word a0 (a0 < 48) of the table at D_002817C0 + 0xC0, or 0.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

struct T { int a[0x30]; int b[0x30]; int c[0x10]; };
extern struct T D_002817C0;

int func_0011A730(int a0) {
    int v1 = 0;
    if ((unsigned int)a0 < 0x30U) {
        v1 = D_002817C0.b[a0];
    }
    return v1;
}
