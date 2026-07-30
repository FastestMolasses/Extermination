// NEARMISS func_00116DB8  (vram 0x00116DB8, 0x2D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.22% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc CSE wall (local, 4 instrs) — everything except one 4-instruction cluster in the `mode == 1` arm is instruction-for-instruction identical (control flow, all delay slots, both branch-likely idioms, both loops, IV selection, epilogue). Expected evaluates `D_0027C6C0[j].buf` TWICE without CSE: ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
extern volatile Slot D_0027C6C0[];
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
        int j = *(unsigned short *)(p + 0x24);
        D_00281AC0.base = D_0027C6C0[j].buf;
        D_00281AC0.cur = D_0027C6C0[j].buf + *(int *)(D_00281AC0.base + 0x20);
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
