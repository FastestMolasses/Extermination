// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a CodeWarrior asm body;
// readable companion src/readable/func_001D6DD0.c was 40.61%). Compiler: mwcc 2.3.3.
// Fixes read from the original instructions:
// - The packed (x, y) doubleword is computed first, as its own local, spelled
//   x | (y << 32) (both sign-extended to 64 bits).
// - The channel cursor is reached through one slot pointer local
//   (&D_00275670->cur[ch]) and every cursor access goes through it; that gives the
//   original's slot register.
// - The body pointer (cursor + 0x10) is taken before the cursor advance, the zero
//   quadword is stored through it and it is the return value (as func_001D6E60 /
//   func_001D6C90 do).
// - Written with the DmaTag / VifLists types of the byte-matched siblings
//   func_001D7000 / func_001D6F60.
//
// Appends one GS register packet to DMA list ch of D_00275670 (the 0x40-byte layout
// of func_001D7000): an id 0x10 tag (addr 0, qwc 3), a zeroed quadword whose last
// word is 0x50000002, the GIF tag 0x1000000000008001 / 0xE (one A+D register), then
// the data word x | (y << 32) for GS register 0x18 (XYOFFSET_1). The cursor advances
// by 0x40. Returns the packet body (tag + 0x10).

typedef unsigned __int128 u128;
typedef struct {
    unsigned short qwc;
    unsigned char pad;
    unsigned char id;
    int addr;
    int w2;
    int w3;
} DmaTag;
typedef struct {
    char pad[0x10];
    DmaTag *cur[16];
} VifLists;
extern VifLists *D_00275670;

DmaTag *func_001D6DD0(int ch, int x, int y) {
    unsigned long packed = (long)x | ((long)y << 32);
    DmaTag **slot = &D_00275670->cur[ch];
    DmaTag *p;
    DmaTag *q;
    long *d;

    (*slot)->id = 0x10;
    (*slot)->addr = 0;
    (*slot)->qwc = 3;
    p = *slot;
    q = p + 1;
    *slot = p + 4;
    *(u128 *)q = 0;
    p[1].w3 = 0x50000002;
    d = (long *)(p + 2);
    d[0] = 0x8001L | ((long)0x10000000 << 32);
    d[1] = 0xE;
    d[2] = packed;
    d[3] = 0x18;
    return q;
}
