// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.95% NEARMISS).
// Fixes read from the original instructions:
//  - The trailing func_00109C58(heap) is a plain call statement, not
//    `return func_00109C58(heap);`: only the statement form lets ee-gcc emit the
//    sibling jump (same rule as func_00106CB0 / func_00107E88). The success path
//    therefore falls off the end and the caller sees func_00109C58's value in v0.
//    The too-small path is an early return of 0.
//  - func_00109A90 returns int (its byte-matched definition does); declaring it
//    void changed which register held the first descriptor address.
//  - The nine handler-table stores run in ascending address order, and the three
//    -1 stores to +0x80/+0x94/+0x98 come after the D_002412F4 store, with +0x9C
//    last.
//  - Externs given their real prototypes (func_00109A90 is still called with ctx,
//    which the original passes even though the definition ignores it).
//
// SEMANTICS: library initialiser (Sony SDK "lowmem" region).
//
// Takes a caller-supplied work buffer, aligns it up to 4 bytes, and requires
// at least 0x118 bytes of header plus the remainder for the heap.  If the
// buffer is too small it prints D_0026B5C0 ("The size of work area is too
// small") through func_0010A3A8 and returns 0.
//
// Layout: `work`        = aligned buffer base (the 0x108-byte context header),
//         `work + 0x108`= heap object initialised by func_00109C40,
//         and `ctx`     = the caller's descriptor, whose 0x40 field points at
//         the aligned buffer.  D_002412F4 becomes the global "current context".
//
// The nine D_002413xx globals are the default handler table, pointing at the
// nine consecutive 0x68-byte descriptors starting at D_00241550.
//

extern void func_00109C40(unsigned int, unsigned int, unsigned int);
extern int sub_work_area_size_is_too_small(unsigned int, int, int);
extern void func_0010A248(void);
extern void func_00109A50(void *);
extern int func_00109A90(void *);
extern int func_00109C58(unsigned int);
extern void func_0010A3A8(char *);

extern void func_0010AD88();
extern void func_0010AD98();

extern char D_0026B5C0[];

extern void *D_002412F4;
extern void *D_00241300;
extern void *D_00241304;
extern void *D_00241308;
extern void *D_0024130C;
extern void *D_00241310;
extern void *D_00241314;
extern void *D_00241318;
extern void *D_0024131C;
extern void *D_00241320;

extern char D_00241550[];
extern char D_002415B8[];
extern char D_00241620[];
extern char D_00241688[];
extern char D_002416F0[];
extern char D_00241758[];
extern char D_002417C0[];
extern char D_00241828[];
extern char D_00241890[];

int sub_The_size_of_work_area_is_too_sma(void *ctx, unsigned int buf,
                                         unsigned int size) {
    unsigned int work;
    unsigned int heap;
    int alloc;

    work = ((buf + 3) >> 2) << 2;
    size -= work - buf;
    if (size < 0x118) {
        func_0010A3A8(D_0026B5C0);
        return 0;
    }

    heap = work + 0x108;
    *(unsigned int *)((char *)ctx + 0x40) = work;
    func_00109C40(heap, work + 0x118, size - 0x118);

    *(int *)((char *)ctx + 0x00) = 0;
    *(int *)((char *)ctx + 0x04) = 0;
    *(int *)((char *)ctx + 0x08) = 0;
    *(long *)((char *)ctx + 0x10) = -1;
    *(long *)((char *)ctx + 0x18) = -1;
    *(long *)((char *)ctx + 0x20) = 0;
    *(long *)((char *)ctx + 0x28) = -1;
    *(long *)((char *)ctx + 0x30) = -1;
    *(long *)((char *)ctx + 0x38) = 0;

    *(int *)(work + 0xB4) = 0;
    *(int *)(work + 0xB8) = 0;
    *(int *)(work + 0xBC) = 0;
    *(int *)(work + 0xC0) = 0;
    *(int *)(work + 0xC4) = 0;
    *(int *)(work + 0xC8) = 0;
    *(int *)(work + 0xCC) = 0;
    *(int *)(work + 0xD0) = 0;
    *(int *)(work + 0xD4) = 0;
    *(int *)(work + 0xD8) = 0;
    *(int *)(work + 0xDC) = 0;
    *(int *)(work + 0xE0) = 0;
    *(int *)(work + 0xE4) = 0;
    *(int *)(work + 0xE8) = 0;
    *(int *)(work + 0xF8) = 0;
    *(int *)(work + 0x0C) = 0;
    *(int *)(work + 0x14) = 0;
    *(int *)(work + 0x2C) = 0;
    *(int *)(work + 0x34) = 0;
    *(int *)(work + 0x3C) = 0;
    *(long *)(work + 0xF0) = -1;
    *(void (**)())(work + 0x1C) = func_0010AD88;
    *(void (**)())(work + 0x24) = func_0010AD98;

    alloc = sub_work_area_size_is_too_small(heap, 0x600, 8);
    *(int *)(work + 0x48) = 0;
    *(int *)(work + 0xFC) = 0;
    *(int *)(work + 0x100) = 0;
    *(int *)(work + 0x104) = 0;
    *(int *)(work + 0x70) = 0;
    *(long *)(work + 0x78) = 0;
    *(long *)(work + 0x88) = 0;
    *(int *)(work + 0x90) = 0;
    *(int *)(work + 0xAC) = 0;
    *(int *)(work + 0x44) = alloc;
    *(int *)(work + 0xB0) = 1;
    D_002412F4 = ctx;
    *(int *)(work + 0x80) = -1;
    *(int *)(work + 0x94) = -1;
    *(int *)(work + 0x98) = -1;
    *(int *)(work + 0x9C) = -1;

    func_0010A248();
    func_00109A50(ctx);
    func_00109A90(ctx);

    D_00241300 = D_00241550;
    D_00241304 = D_002415B8;
    D_00241308 = D_00241620;
    D_0024130C = D_00241688;
    D_00241310 = D_002416F0;
    D_00241314 = D_00241758;
    D_00241318 = D_002417C0;
    D_0024131C = D_00241828;
    D_00241320 = D_00241890;

    func_00109C58(heap);
}
