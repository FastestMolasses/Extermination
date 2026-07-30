// NEARMISS func_00119AA0  (vram 0x00119AA0, 0x1F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.26% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler wall. Body/structure/regalloc fully recovered (128 instrs both sides, identical frame 0xA0, identical callee-save set, identical control flow, identical loop IVs incl. the strength-reduced 0x6A offset IV and the 64-bit `bit <<= 1` mask IV). Two residual clusters, both pure po...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_0027E0C0[];
extern char D_0027C6C0[];
extern char D_0027CCC0[];
extern char D_0027CCD4[];
extern char D_0027F740[];
extern char D_00281AC0[];
extern void func_001157F0(int a, int b, int c, int d);

void func_00119AA0(unsigned int a0, int a1) {
    char *e;
    char *q;
    char *g;
    char *r;
    long bit;
    int off;
    unsigned int i;
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

    q = D_0027CCC0;
    g = D_0027F740;
    bit = 1;
    for (i = 0, off = 0; i < 0x30; i++, q += 0x6A, off += 0x6A, bit <<= 1) {
        if (*(unsigned short *)(q + 6) != a0) {
            continue;
        }
        if (*(unsigned short *)(q + 0x1A) != 1) {
            continue;
        }
        *(unsigned short *)(q + 0) = 0;
        *(unsigned short *)(q + 8) = 1;
        *(unsigned short *)(q + 0x38) = 0x40;
        *(long *)(g + 0x28) |= bit;
        *(unsigned short *)(D_0027CCD4 + off) = 0;
        *(unsigned short *)(q + 0x16) = 0;
        if (sel == 1) {
            func_001157F0(3, i, 0, 0);
        }
    }

    r = D_00281AC0;
    *(int *)(r + 0xC) = *(int *)(e + 0xC) + 0x10;
    for (i = 0; i < 0x10; i++) {
        *(char *)(*(int *)(D_00281AC0 + 0xC) + 9) = 0;
        *(int *)(D_00281AC0 + 0xC) = *(int *)(D_00281AC0 + 0xC) + 0x10;
    }
}
