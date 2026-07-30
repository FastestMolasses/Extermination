// NEARMISS func_001CFFE0  (vram 0x001CFFE0, 0x414 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 87.16% (mwcc233; mwcc 2.3.1 = 76.1%). Decode is complete and verified instruction-by-instruction: every packet field offset/width/constant, both jump tables, all 14 case bodies, the 9-iteration quadword loop and the whole tail block line up 1:1 with the target -- there is NO logic residual. Progr...
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
    char *dl;
    char *q;
    char *p;
    char *dst;
    char *m;
    int kind;
    int sel;
    int soff;
    int n;
    int ret;
    int i;
    char *tbl;

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
    soff = sel * 16;

    dl = D_00275670;
    *(char *)(*(int *)(dl + n + 0x10) + 3) = 0x30;
    *(int *)(*(int *)(dl + n + 0x10) + 4) = ret;
    *(short *)(*(int *)(dl + n + 0x10)) = 8;
    *(int *)(dl + n + 0x10) = *(int *)(dl + n + 0x10) + 0x10;

    dl = D_00275670;
    *(char *)(*(int *)(dl + n + 0x10) + 3) = 0x50;
    *(int *)(*(int *)(dl + n + 0x10) + 4) = (int)tbl;
    *(short *)(*(int *)(dl + n + 0x10)) = 0;
    *(int *)(dl + n + 0x10) = *(int *)(dl + n + 0x10) + 0x10;

    dl = D_00275670;
    *(char *)(*(int *)(dl + n + 0x10) + 3) = 0x10;
    *(int *)(*(int *)(dl + n + 0x10) + 4) = 0;
    *(short *)(*(int *)(dl + n + 0x10)) = 0x10;
    q = *(char **)(dl + n + 0x10);
    *(char **)(dl + n + 0x10) = q + 0x110;
    ((s128 *)q)[1] = 0;
    *(int *)(q + 0x18) = 0x01000101;
    *(int *)(q + 0x1C) = 0x6C0F006E;
    ((s128 *)q)[2] = ((s128 *)D_70003A40)[0];
    ((s128 *)q)[3] = ((s128 *)D_70003A40)[1];
    ((s128 *)q)[4] = ((s128 *)D_70003A40)[2];
    ((s128 *)q)[5] = ((s128 *)D_70003A40)[3];
    m = *(char **)(src + 0x40);
    ((s128 *)q)[6] = ((s128 *)m)[0];
    ((s128 *)q)[7] = ((s128 *)m)[1];
    ((s128 *)q)[8] = ((s128 *)m)[2];
    ((s128 *)q)[9] = ((s128 *)m)[3];
    ((s128 *)q)[10] = ((s128 *)D_70003AC0)[0];
    ((s128 *)q)[11] = ((s128 *)D_70003AC0)[1];
    ((s128 *)q)[12] = ((s128 *)D_70003AC0)[2];
    ((s128 *)q)[13] = ((s128 *)D_70003AC0)[3];
    ((s128 *)q)[14] = ((s128 *)D_00275670)[10];
    *(int *)(q + 0xF0) = 0;
    *(int *)(q + 0xF4) = 0;
    *(float *)(q + 0xF8) = *(float *)(src + 0x54);
    *(int *)(q + 0xFC) = 0;
    ((s128 *)q)[16] = ((s128 *)(D_00251260 + soff))[0];

    dl = D_00275670;
    *(char *)(*(int *)(dl + n + 0x10) + 3) = 0x10;
    *(int *)(*(int *)(dl + n + 0x10) + 4) = 0;
    *(short *)(*(int *)(dl + n + 0x10)) = 0xA;
    q = *(char **)(dl + n + 0x10);
    p = q + 0x10;
    dst = p + 0x10;
    *(char **)(dl + n + 0x10) = q + 0xB0;
    ((s128 *)q)[1] = 0;
    *(int *)(q + 0x1C) = 0x6C090050;
    i = 9;
    do {
        i -= 1;
        *(s128 *)dst = *(s128 *)obj;
        dst += 0x10;
        obj += 0x10;
    } while (i != 0);

    dl = D_00275670;
    *(char *)(*(int *)(dl + n + 0x10) + 3) = 0x10;
    *(int *)(*(int *)(dl + n + 0x10) + 4) = 0;
    *(short *)(*(int *)(dl + n + 0x10)) = 7;
    q = *(char **)(dl + n + 0x10);
    *(char **)(dl + n + 0x10) = q + 0x80;
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
