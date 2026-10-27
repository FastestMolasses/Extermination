// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.22% NEARMISS).
// Fixes read from the original instructions:
//  - The `mode == 1` arm indexes the slot table twice by re-reading the 16-bit
//    slot index from the object at each use (the original computes the
//    element address twice and loads `buf` twice); caching it in a local let
//    ee-gcc share the address computation.
//  - The slot table is a plain (non-volatile) extern; the earlier volatile
//    qualifier was a workaround and is not needed.
//
// Commits the object's pending slot/owner change (mode 1: swap the current and
// requested slot values and point the shared context at that slot's buffer;
// mode 2: point the context at the object's own buffer and apply the per-bit
// mask writes), then pushes the new value to every active node owned by the
// object, refreshing its outputs through func_001179E0 / func_001157F0.
// Returns -1 when nothing was pending, else 0.

typedef struct {
    /* 0x00 */ signed char cur;
    /* 0x01 */ signed char req;
} Pair; /* 0x02 */

typedef struct {
    /* 0x00 */ int unk0;
    /* 0x04 */ unsigned char *buf;
    /* 0x08 */ int unk8;
} Slot; /* 0x0C */

typedef struct {
    /* 0x00 */ int unk0;
    /* 0x04 */ int unk4;
    /* 0x08 */ unsigned char *cur;
    /* 0x0C */ unsigned char *dst;
    /* 0x10 */ unsigned char *base;
} Ctx;

typedef struct {
    /* 0x00 */ unsigned short state;
    /* 0x02 */ unsigned short unk02;
    /* 0x04 */ unsigned short slot;
    /* 0x06 */ unsigned short owner;
    /* 0x08 */ unsigned char unk08[0x12];
    /* 0x1A */ unsigned short kind;
    /* 0x1C */ unsigned char unk1C[0x0C];
    /* 0x28 */ unsigned short out28;
    /* 0x2A */ unsigned char unk2A[0x06];
    /* 0x30 */ unsigned short out30;
    /* 0x32 */ unsigned char unk32[0x38];
} Node; /* 0x6A */

extern Pair D_002819C0[];
extern Slot D_0027C6C0[];
extern Ctx D_00281AC0;
extern Node D_0027CCC0[];

extern int func_001179E0(int idx, unsigned char *p);
extern void func_001157F0(int a, int b, int c, int d);

int func_00116DB8(unsigned char *p)
{
    int i;
    int mode;

    mode = 0;
    if (*(unsigned short *)(p + 0x2E) == 0 && *(unsigned short *)(p + 0x32) == 1) {
        int j = *(unsigned short *)(p + 0x24);
        if (D_002819C0[j].req == 1) {
            int t;
            D_002819C0[j].req = 0;
            *(int *)(p + 0x68) = 0xFFFF;
            t = *(int *)(p + 0x6C);
            *(int *)(p + 0x6C) = D_002819C0[j].cur;
            D_002819C0[j].cur = t;
            mode = 1;
        }
    } else if (*(unsigned short *)(p + 0x2E) == 1 &&
               *(unsigned short *)(p + 0x32) == 0 && *(int *)(p + 0x64) == 1) {
        *(int *)(p + 0x64) = 0;
        mode = 2;
    }

    switch (mode) {
    case 1: {
        D_00281AC0.base = D_0027C6C0[*(unsigned short *)(p + 0x24)].buf;
        D_00281AC0.cur = D_0027C6C0[*(unsigned short *)(p + 0x24)].buf + *(int *)(D_00281AC0.base + 0x20);
        *D_00281AC0.cur = p[0x6C];
        break;
    }
    case 2: {
        unsigned char *b = *(unsigned char **)(p + 0xC);
        D_00281AC0.dst = b + 0x10;
        D_00281AC0.cur = b;
        if (*(int *)(p + 0x68) == 0xFFFF) {
            *b = p[0x6C];
        }
        if (*(int *)(p + 0x70) != 0) {
            int mask = 1;
            for (i = 0; i < 16; i++) {
                if (*(int *)(p + 0x70) & mask) {
                    D_00281AC0.dst[0xE] = p[0x74];
                }
                D_00281AC0.dst += 0x10;
                mask <<= 1;
            }
            D_00281AC0.dst -= 0x100;
        }
        break;
    }
    default:
        return -1;
    }

    for (i = 0; i < 0x30; i++) {
        int v;

        if (D_0027CCC0[i].state != 1) {
            continue;
        }
        if (D_0027CCC0[i].kind == 3) {
            continue;
        }
        if (D_0027CCC0[i].owner != *(int *)(p + 0x18)) {
            continue;
        }
        D_0027CCC0[i].out30 = *D_00281AC0.cur;
        if (D_0027CCC0[i].kind == 1) {
            D_00281AC0.dst += D_0027CCC0[i].slot << 4;
            D_0027CCC0[i].out28 = D_00281AC0.dst[0xE];
            D_00281AC0.dst -= D_0027CCC0[i].slot << 4;
        }
        v = func_001179E0(i, p);
        func_001157F0(1, i, v >> 16, v & 0xFFFF);
    }
    return 0;
}
