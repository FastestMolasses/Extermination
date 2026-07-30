// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: allocate the next slot of a per-effect-type ring buffer and fill in
// its header.  `n` selects one of 7 effect presets via jtbl_0026E9C0 (ids 0..6);
// any other id returns immediately.  Each preset supplies four things: a 64-bit
// tag/mode word written at slot+0x50, a 4-float parameter vector copied to
// slot+0x40, a `count` whose *60 goes to slot+0x58, and the ring `limit`.
//   0: tag 0x20040F8555322078, params { 8, 8, 8, 64 }, count 3,  limit 0x20
//      (and only this preset also calls func_001EFD20(0x8000000E, src+0x30))
//   1: tag 0x200418851532218C, params { 48, 48, 48, 64 }, count 3,  limit 0x14
//   2: identical to 1 (a separate, duplicated code block in the original)
//   3: tag 0x2004108555322080, params { 255, 0, 0, 64 },  count 3,  limit 0x20
//   4: tag 0x20040F8555322078, params { 48, 32, 16, 80 }, count 20, limit 0x20
//   5: tag 0x20040F8555322078, params { 16, 16, 16, 64 }, count 3,  limit 0x20
//   6: same as 5 but count 2
// D_0081F950[n] is the per-preset ring index: it is bumped, then wrapped to 0
// once it reaches `limit`.  The slot address is
//   D_0028F700 + 0x4DBEC0 + n*3072 + index*96
// i.e. 32 slots of 96 bytes per preset lane, all inside one large table.  The
// caller's 4-quadword record `src` is copied into the slot with copy_qw4, and the
// float parameter vector is copied to slot+0x40 with func_00102948.
//

extern void func_001EFD20(unsigned int a, char *b);
extern void copy_qw4(char *dst, char *src);
extern void func_00102948(char *dst, char *src);

extern int  D_0081F950[];
extern char D_0028F700[];

void func_001F0460(int n, char *src)
{
    int limit;
    int count;
    long long tag;
    char *p;
    float buf[4];

    switch (n) {
    case 0:
        tag = 0x20040F8555322078LL;
        buf[0] = 8.0f;
        buf[1] = 8.0f;
        buf[2] = 8.0f;
        buf[3] = 64.0f;
        count = 3;
        limit = 0x20;
        func_001EFD20(0x8000000E, src + 0x30);
        break;
    case 1:
        tag = 0x200418851532218CLL;
        buf[0] = 48.0f;
        buf[1] = 48.0f;
        buf[2] = 48.0f;
        buf[3] = 64.0f;
        count = 3;
        limit = 0x14;
        break;
    case 2:
        tag = 0x200418851532218CLL;
        buf[0] = 48.0f;
        buf[1] = 48.0f;
        buf[2] = 48.0f;
        buf[3] = 64.0f;
        count = 3;
        limit = 0x14;
        break;
    case 3:
        tag = 0x2004108555322080LL;
        count = 3;
        limit = 0x20;
        buf[0] = 255.0f;
        buf[1] = 0.0f;
        buf[2] = 0.0f;
        buf[3] = 64.0f;
        break;
    case 4:
        tag = 0x20040F8555322078LL;
        buf[0] = 48.0f;
        buf[1] = 32.0f;
        buf[2] = 16.0f;
        buf[3] = 80.0f;
        count = 0x14;
        limit = 0x20;
        break;
    case 5:
        tag = 0x20040F8555322078LL;
        buf[0] = 16.0f;
        buf[1] = 16.0f;
        buf[2] = 16.0f;
        buf[3] = 64.0f;
        count = 3;
        limit = 0x20;
        break;
    case 6:
        tag = 0x20040F8555322078LL;
        buf[0] = 16.0f;
        buf[1] = 16.0f;
        buf[2] = 16.0f;
        buf[3] = 64.0f;
        count = 2;
        limit = 0x20;
        break;
    default:
        return;
    }

    D_0081F950[n] = D_0081F950[n] + 1;
    if (D_0081F950[n] >= limit) {
        D_0081F950[n] = 0;
    }

    p = (char *)D_0028F700 + ((n * 3) << 10) + 0x4DBEC0;
    p = p + D_0081F950[n] * 96;
    *(long long *)(p + 0x50) = tag;
    *(int *)(p + 0x58) = count * 60;
    copy_qw4(p, src);
    func_00102948(p + 0x40, (char *)buf);
}
