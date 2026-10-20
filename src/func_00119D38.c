// COMPILER: eegcc
// CFLAGS: -O2
//
// For slot a0 (a0 < 48) of D_0027E0C0 whose u16 +0x2E is 1, returns u16 +0x30
// | (u16 +0x40 << 1); otherwise -1.
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

int func_00119D38(unsigned int a0) {
    int r = -1;
    Slot78 *s = &D_0027E0C0[a0];
    if (a0 < 48 && s->state == 1) {
        r = s->lo; r |= s->hi << 1;
    }
    return r;
}
