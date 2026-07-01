// NEARMISS func_001DF600  (vram 0x001DF600, 0x3C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Deep register-allocation / stack-layout divergence in a large (964-byte, 0x10F0-frame) function building a 16x16 lighting grid then emitting GS draw packets. Logic fully recovered and verified field-by-field against target asm (grid indexing, packet header bit-packing at body+0x1C/0x20/0x28, lo/h...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

typedef unsigned __int128 u128;
extern int float_to_int(float f);
extern void func_00102948(void *a0, void *a1);
extern float func_0011DF78(float a);
extern void func_001D1F20(int a0);
extern void func_001D1FF0(int a0, int a1);
extern void func_001D2040(int a0, int a1);
extern void func_001D6B60(int a0, int a1, int a2, int a3, void *a4);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int t0p, int t1p);
extern void func_001D7080(int a0, int a1, float fa0);
extern int D_0026E870[4];
extern int *D_00275670;
extern int D_0027568C;

int func_001DF600(int arg0, int arg1, long long arg2, float fparg0) {
    char grid[0x1000];
    struct { int a, b, c, d; } lo, hi;
    int *rec;
    char *cur;
    char *body;
    char *row_base;
    char *cell;
    int handle;
    int i, j;
    int row, col;
    float fi, fj;
    float dy, dx, shade;

    rec = (int *)((char *)D_00275670 + arg0 * 4);
    handle = rec[4];

    func_001D6B60(arg0, D_0027568C, 8, 8, D_0026E870);
    func_001D6BA0(arg0, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(arg0, 3);
    func_001D2040(arg0, 0);
    func_001D7080(arg0, (int)((arg2 << 32) >> 32), 1.0f);

    row_base = grid;
    i = 0;
    cell = row_base;
    do {
        fi = (float) i / 15.0f;
        dy = func_0011DF78((2.0f * fi) - 1.0f);
        dy = dy * dy;
        j = 0;
        cell = row_base;
        do {
            fj = (float) j / 15.0f;
            dx = func_0011DF78((2.0f * fj) - 1.0f);
            dx = dx * dx;
            j += 1;
            shade = 1.0f - ((dx + dy) * fparg0);
            *(float *)(cell + 0) = 0.5f + ((fj - 0.5f) * shade);
            *(float *)(cell + 4) = 0.5f + ((fi - 0.5f) * shade);
            *(int *)(cell + 8) = 0x3F800000;
            cell += 0x10;
        } while (j < 0x10);
        i += 1;
        row_base += 0x100;
    } while (i < 0x10);

    row = 0;
    row_base = grid;
    do {
        rec = (int *)((char *)D_00275670 + arg0 * 4);
        cur = *(char **)((char *)rec + 0x10);
        cur[3] = 0x10;
        *(int *)(cur + 4) = 0;
        *(short *)(cur + 0) = 0x42;
        body = cur;
        *(char **)((char *)rec + 0x10) = body + 0x430;
        *(u128 *)(body + 0x10) = 0;
        *(int *)(body + 0x1C) = 0x50000041;
        *(long long *)(body + 0x20) = ((long long)(int)(arg1 | 0x14) << 47) | 0x8010 | ((long long)(int)0x40004000 << 32);
        *(long long *)(body + 0x28) = 0x4242;

        lo.a = 0x7000;
        hi.a = 0x7000;
        lo.b = (((unsigned)(row * 0xE0) / 15) + (((unsigned)(row * 0xE0)) >> 31) + 0x790) * 0x10;
        hi.b = ((((row + 1) * 0xE0) / 15) + (((unsigned)((row + 1) * 0xE0)) >> 31) + 0x790) * 0x10;
        lo.c = 0xFFFFFF;
        hi.c = 0xFFFFFF;

        cur = body + 0x10 + 0x20;
        cell = row_base;
        col = 0;
        do {
            func_00102948(cur, cell);
            func_00102948(cur + 0x10, &lo);
            func_00102948(cur + 0x20, cell + 0x100);
            func_00102948(cur + 0x30, &hi);
            lo.b = float_to_int((float) lo.b + 546.13336f);
            col += 1;
            hi.b = float_to_int((float) hi.b + 546.13336f);
            cell += 0x10;
            cur += 0x40;
        } while (col < 0x10);

        row += 1;
        row_base += 0x100;
    } while (row < 0xF);

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return handle;
}
