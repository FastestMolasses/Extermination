// NEARMISS func_0010A650  (vram 0x0010A650, 0x344 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.61% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc list-scheduler / GPR-coloring wall on the last ~9 instructions. Whole tail (heap alloc, func_0010A998 12-arg call with 8-byte-stride stack args, all 9 func_0010A4D8 calls) is byte-identical, and the 6-store init block + the D_0024142C address materialization now match exactly. Residual: ee-...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
    D_0024140C = w;
    D_00241410 = h;

    if (w == ctx[0] && h == ctx[1])
        return;

    ctx[1] = h;
    ctx[0] = w;
    D_00241414 = w >> 1;
    D_00241418 = h >> 1;

    heap = (void *)(st + 0x108);
    size = (unsigned int)(h * 0x180 * w) >> 8;
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
