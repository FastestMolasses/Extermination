// NEARMISS func_001CFFE0  (vram 0x001CFFE0, 0x414 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) (was 87.16%; matching wave 2026-10-27).
// Improvements: saved registers now match (sel/tbl declared first), the two scratchpad matrices are read
// through base-pointer locals g1/g2 (one address setup each, as in the byte-matched sibling func_001CFBE0),
// the selected D_00251260 row is indexed as a quadword array, each packet header goes through a cursor
// pointer, and packet 4's GIFtag is written through its tag pointer p.
// Remaining differences: caller-saved temporary colouring only (D_00275670 base / slot*4 / small tag constants
// in different t/a/v registers after the func_001CB9B0 call); instruction order and logic are identical.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: emits one actor's five GS/DMA display-list packets into the
//   per-slot write cursor held at D_00275670 + slot*4 + 0x10 (gp-rel pointer
//   to the current packet-buffer tail; each write advances it).
//   slot = display-list channel, idx = the actor's material/skin variant
//   (0..6), obj = the actor draw block (obj+0x8C = its kind), src = the
//   transform/colour block.
//   The (kind, idx) pair selects a texture/CLUT base `tbl` and a light-set
//   index `sel`: kind 1 uses one table, kinds 2/3/4 another (both 7-entry
//   jump tables; other kinds leave both unset, as in the original).
//   func_001CB9B0(kind) returns the shared per-kind resource pointer.
//   Packet 1 (0x10 bytes): tag byte 0x30, word = the func_001CB9B0 result,
//     half = 8.
//   Packet 2 (0x10 bytes): tag byte 0x50, word = tbl, half = 0.
//   Packet 3 (0x110 bytes): tag byte 0x10, half 0x10 -- a 0x100-byte GIF
//     block: GIFtag 0x01000101 / 0x6C0F006E, four quadwords from the
//     scratchpad matrix 0x70003A40, four from *(src+0x40), four from
//     0x70003AC0, one from D_00275670+0xA0, then two zero words, the float
//     at src+0x54, a zero word, and one quadword from D_00251260[sel].
//   Packet 4 (0xB0 bytes): tag byte 0x10, half 0xA -- GIFtag 0x6C090050
//     followed by 9 quadwords copied straight out of `obj`.
//   Packet 5 (0x80 bytes): tag byte 0x10, half 7 -- GIFtag 0x6C050059, the
//     four floats src+0x44/0x48/0x50/0x4C, four quadwords from src+0x00..0x30,
//     then a zeroed quadword overwritten with 0x14000000 / 0x11000000.

typedef int s128 __attribute__((mode(TI)));

extern char *D_00275670;
extern char D_00230800[];
extern char D_00231770[];
extern char D_00232540[];
extern char D_00233800[];
extern char D_0023D930[];
extern char D_00251260[];
extern char D_70003A40[];
extern char D_70003AC0[];

extern int func_001CB9B0(int kind);

void func_001CFFE0(int slot, unsigned int idx, char *obj, char *src)
{
    int sel;
    char *tbl;
    char *dl;
    char **c;
    char *q;
    char *p;
    char *dst;
    char *m;
    int kind;
    int n;
    int ret;
    int i;
    s128 *g1;
    s128 *g2;

    kind = *(int *)(obj + 0x8C);
    switch (kind) {
    case 1:
        switch (idx) {
        case 0:
            tbl = D_00230800;
            sel = 0;
            break;
        case 1:
            tbl = D_00231770;
            sel = 2;
            break;
        case 6:
            tbl = D_0023D930;
            sel = 2;
            break;
        case 2:
            tbl = D_00232540;
            sel = 4;
            break;
        case 3:
            tbl = D_00233800;
            sel = 2;
            break;
        case 5:
            sel = 6;
            tbl = D_00231770;
            break;
        case 4:
            sel = 7;
            tbl = D_00230800;
            break;
        }
        break;
    case 2:
    case 3:
    case 4:
        switch (idx) {
        case 0:
            sel = 1;
            tbl = D_00230800;
            break;
        case 1:
            sel = 3;
            tbl = D_00231770;
            break;
        case 6:
            sel = 3;
            tbl = D_0023D930;
            break;
        case 2:
            sel = 5;
            tbl = D_00232540;
            break;
        case 3:
            sel = 3;
            tbl = D_00233800;
            break;
        case 5:
            sel = 6;
            tbl = D_00231770;
            break;
        case 4:
            sel = 7;
            tbl = D_00230800;
            break;
        }
        break;
    }

    ret = func_001CB9B0(kind);
    n = slot * 4;

    dl = D_00275670;
    c = (char **)(dl + n + 0x10);
    (*c)[3] = 0x30;
    *(int *)(*c + 4) = ret;
    *(short *)*c = 8;
    *c = *c + 0x10;

    dl = D_00275670;
    c = (char **)(dl + n + 0x10);
    (*c)[3] = 0x50;
    *(int *)(*c + 4) = (int)tbl;
    *(short *)*c = 0;
    *c = *c + 0x10;

    dl = D_00275670;
    c = (char **)(dl + n + 0x10);
    (*c)[3] = 0x10;
    *(int *)(*c + 4) = 0;
    *(short *)*c = 0x10;
    q = *c;
    *c = q + 0x110;
    ((s128 *)q)[1] = 0;
    *(int *)(q + 0x18) = 0x01000101;
    *(int *)(q + 0x1C) = 0x6C0F006E;
    g1 = (s128 *)D_70003A40;
    ((s128 *)q)[2] = g1[0];
    ((s128 *)q)[3] = g1[1];
    ((s128 *)q)[4] = g1[2];
    ((s128 *)q)[5] = g1[3];
    m = *(char **)(src + 0x40);
    ((s128 *)q)[6] = ((s128 *)m)[0];
    ((s128 *)q)[7] = ((s128 *)m)[1];
    ((s128 *)q)[8] = ((s128 *)m)[2];
    ((s128 *)q)[9] = ((s128 *)m)[3];
    g2 = (s128 *)D_70003AC0;
    ((s128 *)q)[10] = g2[0];
    ((s128 *)q)[11] = g2[1];
    ((s128 *)q)[12] = g2[2];
    ((s128 *)q)[13] = g2[3];
    ((s128 *)q)[14] = ((s128 *)D_00275670)[10];
    *(int *)(q + 0xF0) = 0;
    *(int *)(q + 0xF4) = 0;
    *(float *)(q + 0xF8) = *(float *)(src + 0x54);
    *(int *)(q + 0xFC) = 0;
    ((s128 *)q)[16] = ((s128 *)D_00251260)[sel];

    dl = D_00275670;
    c = (char **)(dl + n + 0x10);
    (*c)[3] = 0x10;
    *(int *)(*c + 4) = 0;
    *(short *)*c = 0xA;
    q = *c;
    p = q + 0x10;
    dst = p + 0x10;
    *c = q + 0xB0;
    ((s128 *)p)[0] = 0;
    *(int *)(p + 0xC) = 0x6C090050;
    i = 9;
    do {
        i -= 1;
        *(s128 *)dst = *(s128 *)obj;
        dst += 0x10;
        obj += 0x10;
    } while (i != 0);

    dl = D_00275670;
    c = (char **)(dl + n + 0x10);
    (*c)[3] = 0x10;
    *(int *)(*c + 4) = 0;
    *(short *)*c = 7;
    q = *c;
    *c = q + 0x80;
    ((s128 *)q)[1] = 0;
    *(int *)(q + 0x1C) = 0x6C050059;
    *(float *)(q + 0x20) = *(float *)(src + 0x44);
    *(float *)(q + 0x24) = *(float *)(src + 0x48);
    *(float *)(q + 0x28) = *(float *)(src + 0x50);
    *(float *)(q + 0x2C) = *(float *)(src + 0x4C);
    ((s128 *)q)[3] = ((s128 *)src)[0];
    ((s128 *)q)[4] = ((s128 *)src)[1];
    ((s128 *)q)[5] = ((s128 *)src)[2];
    ((s128 *)q)[6] = ((s128 *)src)[3];
    ((s128 *)q)[7] = 0;
    *(int *)(q + 0x70) = 0x14000000;
    *(int *)(q + 0x74) = 0x11000000;
}
