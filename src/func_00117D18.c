// COMPILER: eegcc
// CFLAGS: -O2
// SDK: GS/packet writer. If state->mode != 1, emit a few bytes into the
// global packet D_00281AC0, then advance state->cursor by 2.
struct gd {
    char pad0c[0xC];
    unsigned char *fC;  // 0xC
    char pad14[0x4];
    int f14;            // 0x14
};
extern struct gd D_00281AC0;

struct st {
    char pad8[0x8];
    int cursor;         // 0x8
    char pad34[0x34 - 0xC];
    unsigned short mode; // 0x34
};

int func_00117D18(struct st *a0) {
    if (a0->mode != 1) {
        unsigned char *src = (unsigned char *)(a0->cursor + D_00281AC0.f14);
        D_00281AC0.fC[2] = src[1];
        D_00281AC0.fC[0xA] = 0x40;
        D_00281AC0.fC[0xB] = 0x40;
    }
    a0->cursor += 2;
    return a0->cursor;
}
