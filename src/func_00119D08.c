// COMPILER: eegcc
// CFLAGS: -O2
//
// Returns the u16 at +0x54 of the 0x78-byte slot a0 (a0 < 48) of D_0027E0C0,
// or -1.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

typedef struct {
    char pad0[0x2E];
    unsigned short state;
    unsigned short lo;
    char pad32[0x0E];
    unsigned short hi;
    char pad42[0x12];
    unsigned short id;
    char pad56[0x22];
} Slot78;
extern Slot78 D_0027E0C0[48];

int func_00119D08(unsigned int a0) {
    int r = -1;
    Slot78 *s = &D_0027E0C0[a0];
    if (a0 < 48) {
        r = s->id;
    }
    return r;
}
