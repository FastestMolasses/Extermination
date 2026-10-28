// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 53.72% NEARMISS).
// Fixes read from the original instructions:
// - The mode test is a `switch (arg0)` over 0/1/2 with a default returning -1.
// - The 0x30-entry slot table at D_0027CCC0 (stride 0x6A) is indexed as a struct array
//   (Slot[i].field), as in the byte-matched siblings func_00119AA0 and others.
// - The scan start is a separate local copied into the loop counter.
// - The state block D_0027F740 is reached through a local pointer (g + 0x28).
// - func_001157F0 returns int (its definition in src/).
// The old "frame-stride + colouring wall" verdict was wrong: these source spellings give
// the original's 0x50 frame and registers.
//
// Allocate a voice slot for mode arg0 (0: slots 0..0x2F, 1: 0..0x17, 2: 0x18..0x2F; other
// modes return -1). A free slot (f00 == 0, f1A != 3) is claimed at once (f00 = 1,
// f1A = 3) and its index returned. Otherwise the in-use slot with the smallest f0A whose
// f1A is not 3 is stolen: func_001157F0(3, i, 0, 0) is called with i equal to the loop
// end (the original passes the counter, not the stolen index), the mask bit ORed into the
// 64-bit word at D_0027F740+0x28 is 1 << (limit - start) (the mask advanced once per
// scanned slot, not the stolen slot's bit), the slot is cleared (func_00121A28, 0x6A
// bytes), f06/f26/f24/f22 = 0xFFFF, f4E = 0x78, f00 = 1, f1A = 3, and its index returned.
// Returns -1 if no slot qualifies.

typedef struct {
    unsigned short f00;
    char pad02[4];
    unsigned short f06;
    char pad08[2];
    unsigned short f0A;
    char pad0C[0xE];
    unsigned short f1A;
    char pad1C[6];
    unsigned short f22;
    unsigned short f24;
    unsigned short f26;
    char pad28[0x26];
    unsigned short f4E;
    char pad50[0x1A];
} Slot;
extern Slot D_0027CCC0[];
extern char D_0027F740[];
extern int func_001157F0(int a0, int a1, int a2, int a3);
extern void func_00121A28(void *p, int a1, int a2);

int func_0011A2B0(int arg0) {
    int start, limit;
    int i;
    char *g;
    long mask;
    unsigned int best_val;
    int best_idx;

    best_val = 0xFFFFFFFF;
    best_idx = -1;
    switch (arg0) {
    case 0:
        start = 0;
        limit = 0x30;
        break;
    case 1:
        start = 0;
        limit = 0x18;
        break;
    case 2:
        start = 0x18;
        limit = 0x30;
        break;
    default:
        return -1;
    }

    for (mask = 1, i = start; i < limit; i++, mask <<= 1) {
        if (D_0027CCC0[i].f00 == 0 && D_0027CCC0[i].f1A != 3) {
            D_0027CCC0[i].f00 = 1;
            D_0027CCC0[i].f1A = 3;
            return i;
        }
        if (D_0027CCC0[i].f0A < best_val && D_0027CCC0[i].f1A != 3) {
            best_val = D_0027CCC0[i].f0A;
            best_idx = i;
        }
    }

    if (best_idx == -1) {
        return -1;
    }

    func_001157F0(3, i, 0, 0);
    g = D_0027F740;
    *(long *)(g + 0x28) |= mask;
    func_00121A28(&D_0027CCC0[best_idx], 0, 0x6A);
    D_0027CCC0[best_idx].f06 = 0xFFFF;
    D_0027CCC0[best_idx].f26 = 0xFFFF;
    D_0027CCC0[best_idx].f24 = 0xFFFF;
    D_0027CCC0[best_idx].f22 = 0xFFFF;
    D_0027CCC0[best_idx].f4E = 0x78;
    D_0027CCC0[best_idx].f00 = 1;
    D_0027CCC0[best_idx].f1A = 3;
    return best_idx;
}
