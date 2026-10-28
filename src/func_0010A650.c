// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.61% NEARMISS).
// Fixes read from the original instructions:
//  - the work-area size multiplies width first: (w * 0x180 * h), not (h * 0x180 * w);
//    the operand order decided which of w/h got which register.
//  - ctx[0] (the previous width) is read into its own local BEFORE the global
//    width/height stores; the original loads it ahead of those stores, which a
//    direct ctx[0] read after them cannot do (possible aliasing).
//
// Recomputes the macroblock-aligned frame dimensions (16-pixel rounding; the
// height is rounded to 32 and doubled in one mode), publishes them to the
// globals, and when they differ from the size stored in ctx re-initialises the
// work-area heap, allocates three 64-byte-aligned frame buffers, rebuilds the
// nine plane descriptors and resizes each (luma full height, chroma half).

extern int D_00241BB0;
extern int D_002414AC;
extern int D_002414B4;
extern int D_0024146C;
extern int D_00241448;
extern int D_0024144C;
extern int D_002414C8;
extern int D_00241420;
extern int D_00241424;
extern int D_00241428;
extern int D_0024142C;
extern int D_0024140C;
extern int D_00241410;
extern int D_00241414;
extern int D_00241418;

extern int D_00241550[];
extern int D_002415B8[];
extern int D_00241620[];
extern int D_00241688[];
extern int D_002416F0[];
extern int D_00241758[];
extern int D_002417C0[];
extern int D_00241828[];
extern int D_00241890[];

extern int func_00109C68(void *heap);
extern int sub_work_area_size_is_too_small(void *heap, unsigned int size, unsigned int align);
extern int func_0010A998(int *a, int *b, int *c, int *d, int *e, int *f, int *g, int *h,
                         int *i, int j, int k, int l);
extern int func_0010A4D8(int *p, int w, int h);

void func_0010A650(int *ctx)
{
    char *st;
    void *heap;
    unsigned int size;
    int w;
    int h;
    int t;
    int *p2c;
    int cw;

    st = (char *)ctx[0x10];

    if (D_00241BB0 == 0) {
        D_00241448 = 1;
        D_0024144C = 1;
        D_002414C8 = 1;
        D_002414AC = 3;
        D_002414B4 = 1;
        D_0024146C = 5;
    }

    D_00241428 = (D_00241420 + 15) >> 4;
    p2c = &D_0024142C;
    if (D_00241BB0 != 0 && D_00241448 == 0)
        t = ((D_00241424 + 31) >> 5) << 1;
    else
        t = (D_00241424 + 15) >> 4;
    *p2c = t;

    w = D_00241428 << 4;
    h = D_0024142C << 4;
    cw = ctx[0];
    D_0024140C = w;
    D_00241410 = h;

    if (w == cw && h == ctx[1])
        return;

    ctx[1] = h;
    ctx[0] = w;
    D_00241414 = w >> 1;
    D_00241418 = h >> 1;

    heap = (void *)(st + 0x108);
    size = (unsigned int)(w * 0x180 * h) >> 8;
    func_00109C68(heap);
    *(int *)(st + 0xFC) = sub_work_area_size_is_too_small(heap, size, 0x40);
    *(int *)(st + 0x100) = sub_work_area_size_is_too_small(heap, size, 0x40);
    *(int *)(st + 0x104) = sub_work_area_size_is_too_small(heap, size, 0x40);

    func_0010A998(D_00241550, D_002415B8, D_00241620, D_00241688, D_002416F0,
                  D_00241758, D_002417C0, D_00241828, D_00241890,
                  *(int *)(st + 0xFC), *(int *)(st + 0x100), *(int *)(st + 0x104));

    func_0010A4D8(D_00241550, D_0024140C, D_00241410);
    func_0010A4D8(D_002415B8, D_0024140C, D_00241410);
    func_0010A4D8(D_00241620, D_0024140C, D_00241410);
    func_0010A4D8(D_00241688, D_0024140C, D_00241410 / 2);
    func_0010A4D8(D_002416F0, D_0024140C, D_00241410 / 2);
    func_0010A4D8(D_00241758, D_0024140C, D_00241410 / 2);
    func_0010A4D8(D_002417C0, D_0024140C, D_00241410 / 2);
    func_0010A4D8(D_00241828, D_0024140C, D_00241410 / 2);
    func_0010A4D8(D_00241890, D_0024140C, D_00241410 / 2);
}
