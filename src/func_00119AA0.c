// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.26% NEARMISS).
// Fixes read from the original instructions:
// - The 0x30-entry slot table at D_0027CCC0 (stride 0x6A) is indexed as a struct
//   array (Slot[i].field), not walked through a char pointer; that restores the
//   loop body's store order.
// - The slot's +0x14 halfword is cleared through a separate byte-offset counter
//   (off += 0x6A) rather than the struct index, which gives the original its
//   second induction variable and its initialisation order.
// - The cursor at D_00281AC0 is a struct whose +0xC field points at 16-byte cells;
//   the final loop clears each cell's +9 byte and advances the cursor through it.
// - func_001157F0 returns int (its definition in src/).
//
// For an active entry a0 (< 0x30) of the 0x78-byte table D_0027E0C0 whose state
// table entry is 1: modes 0/1 reset the entry and pick sel = mode; modes 2/3 only
// proceed when the entry's +0x30 field already equals 1 (otherwise they arm it and
// return). Every slot owned by a0 with +0x1A == 1 is reset, its bit is set in the
// 64-bit mask at D_0027F740+0x28, and with sel == 1 func_001157F0(3, slot, 0, 0)
// is called. Finally the cursor is placed after the entry's +0xC base and 16 cells
// are cleared.

typedef struct {
    char pad00[9];
    char f09;
    char pad0A[6];
} Cell;
typedef struct {
    char pad00[0xC];
    Cell *cur;
} Cursor;
typedef struct {
    unsigned short f00;
    char pad02[4];
    unsigned short f06;
    unsigned short f08;
    char pad0A[0xA];
    unsigned short f14;
    unsigned short f16;
    char pad18[2];
    unsigned short f1A;
    char pad1C[0x1C];
    unsigned short f38;
    char pad3A[0x30];
} Slot;
extern Slot D_0027CCC0[];
extern char D_0027E0C0[];
extern char D_0027C6C0[];
extern char D_0027F740[];
extern Cursor D_00281AC0;
extern int func_001157F0(int a0, int a1, int a2, int a3);

void func_00119AA0(unsigned int a0, int a1) {
    char *e;
    char *g;
    long bit;
    unsigned int i;
    int off;
    int st;
    int sel;
    unsigned short cur;

    e = &D_0027E0C0[a0 * 0x78];
    if (a0 >= 0x30) {
        return;
    }
    if (*(unsigned short *)(e + 0x2E) != 1) {
        return;
    }
    st = *(int *)&D_0027C6C0[*(unsigned short *)(e + 0x24) * 0xC];
    if (st != 1) {
        return;
    }
    if (a1 < 0) {
        return;
    }
    if (a1 < 2) {
        *(int *)(e + 0x10) = st;
        *(int *)(e + 0x8) = 0x110;
        sel = a1;
        *(unsigned short *)(e + 0x30) = 0;
        *(unsigned short *)(e + 0x40) = 0;
        *(unsigned short *)(e + 0x36) = 0;
    } else {
        if (a1 >= 4) {
            return;
        }
        cur = *(unsigned short *)(e + 0x30);
        if (cur != st) {
            if (*(int *)(e + 0x10) != 0) {
                return;
            }
            if (*(unsigned short *)(e + 0x3E) != 0) {
                return;
            }
            *(unsigned short *)(e + 0x30) = st;
            *(unsigned short *)(e + 0x40) = 0;
            return;
        }
        *(unsigned short *)(e + 0x40) = cur;
        sel = a1 - 2;
        *(unsigned short *)(e + 0x30) = 0;
    }

    g = D_0027F740;
    bit = 1;
    for (i = 0, off = 0; i < 0x30; i++, off += 0x6A, bit <<= 1) {
        if (D_0027CCC0[i].f06 != a0) {
            continue;
        }
        if (D_0027CCC0[i].f1A != 1) {
            continue;
        }
        D_0027CCC0[i].f00 = 0;
        D_0027CCC0[i].f08 = 1;
        D_0027CCC0[i].f38 = 0x40;
        *(long *)(g + 0x28) |= bit;
        *(unsigned short *)((char *)&D_0027CCC0[0].f14 + off) = 0;
        D_0027CCC0[i].f16 = 0;
        if (sel == 1) {
            func_001157F0(3, i, 0, 0);
        }
    }

    D_00281AC0.cur = (Cell *)(*(int *)(e + 0xC) + 0x10);
    for (i = 0; i < 0x10; i++) {
        D_00281AC0.cur->f09 = 0;
        D_00281AC0.cur++;
    }
}
