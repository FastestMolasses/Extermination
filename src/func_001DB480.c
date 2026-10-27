// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 55.37% NEARMISS).
// Fixes read from the original instructions:
//  - the four VU0 matrix-row uploads after func_001CD370(3) are written as GCC-style
//    inline asm row loads (user-approved for this function only); with them present
//    mwcc stops strength-reducing the grid loop and produces the original 0xE0 frame.
//  - D_00818000 is a struct grid: a [32][32] array of 16-byte cells (u, v, -, height),
//    then the fade scalar at +0x4000, origin at +0x4020, shade block at +0x4030; the
//    second vertex of each pair reads cell [i + 1][j].
//  - the render-context pointer is a block-scoped local (as in the byte-matched twin
//    func_001DDE10); the strip cursor is taken as packet + 0x10 before the packet
//    pointer is bumped by 0xC30; u = -16 and j = 0 start the column loop.
//  - func_00102948 takes two arguments (dst, src), as its definition in src/ shows.
//  - local declaration order fixes the saved-register assignment.
//
// SEMANTICS: builds and submits the whole "water/heat haze grid" GS display list.
// D_00275670 is the render-context pointer; its field at +0x1C is the running GS
// packet write pointer. D_00818000 is the grid workspace (see struct grid).
//
//   1. Snapshot the current packet pointer (used as the DMA chain head at the end),
//      reserve the 0x258000 VRAM arena for two 8-bit buffers (func_001D6B10 /
//      func_001D6BA0), select context 3 (func_001D1FF0) and program the 15-argument
//      GS environment (func_001D6C90: 8 EABI register args followed by seven 64-bit
//      stack slots).
//   2. func_001CD370(3) returns the current view matrix, which is uploaded to VU0 as
//      vf24..vf27.
//   3. For each of the 0x1F rows: emit a GIFtag/AD header into the packet (byte +3 =
//      0x10 quad count, word +4 = 0, halfword +0 = 0xC2 tag id), bump the packet
//      pointer by 0xC30, zero the first quadword and write the tag words
//      0x500000C1 / 0x300E400000008040 / 0x421, then walk the 0x20 columns emitting a
//      TRIANGLE-STRIP vertex PAIR per cell through func_001DAE10(dst, pos, uvc, shade):
//        pos = { origin.x + u, (origin.y - 10) + v, origin.z + 0.1 * cell.height, 1.0 }
//        uvc = { cell.u, cell.v, 128 * (1 - fade), 0 }
//      The second vertex of the pair is the same column one row down: v is offset by
//      the row pitch 1.1612903f (36/31) and the cell is the next row's.
//      u advances by 1.0 per column, v by 1.1612903f per row, and the write cursor by
//      0x60 (two 0x30-byte vertex records) per column.
//   4. Close the chain (func_001D1F20, context 3 back to buffer 1), emit the terminating
//      GIFtag (byte +3 = 0x60, word +4 = 0, halfword +0 = 0), advance the packet pointer
//      by 0x10, copy the grid origin with func_00102948, stamp 455.0f (0x43E38000) into
//      its +8 field and kick the DMA with func_001CAAC0.
extern void func_001D6B10(int, int, int, int);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D6C90(int, int, int, int, int, int, int, int,
                          long long, long long, long long, long long,
                          long long, long long, long long);
extern void *func_001CD370(int);
extern void func_001DAE10(unsigned char *dst, void *a, void *b, float *c);
extern void func_001D1F20(int);
extern void func_00102948(void *, void *);
extern int func_001CAAC0(int *, unsigned char *);

typedef int u128 __attribute__((mode(TI)));

struct cell {
    float u;
    float v;
    float pad;
    float h;
};

struct grid {
    struct cell c[32][32];
    float fade;
    int pad0[7];
    float origin[4];
    float shade[4];
};

extern char *D_00275670;
extern struct grid D_00818000;

struct vtx {
    float x;
    float y;
    float z;
    int w;
};

void func_001DB480(void) {
    struct vtx va;
    struct vtx vb;
    int out[4];
    struct grid *g;
    unsigned char *first;
    int arena;
    int j;
    int i;
    unsigned char *dl;
    void *m;
    float u;
    float v;

    first = *(unsigned char **)(D_00275670 + 0x1C);
    g = &D_00818000;
    arena = 0x258000;
    func_001D6B10(3, arena, 8, 8);
    func_001D6BA0(3, arena, 8, 8, 3, 0);
    func_001D1FF0(3, 3);
    func_001D6C90(3, 0, 1, 0, 0, 1, 0, 0, 1, 2, 0, 1, 0, 1, 0);
    m = func_001CD370(3);
    /* upload the returned 4x4 view matrix to VU0 registers 24..27, one row each */
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    v = 0.0f;
    for (i = 0; i < 0x1F; i++) {
        unsigned char *pkt;
        {
            char *ctx = D_00275670;
            *(char *)(*(char **)(ctx + 0x1C) + 3) = 0x10;
            *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
            *(short *)(*(char **)(ctx + 0x1C) + 0) = 0xC2;
            pkt = *(unsigned char **)(ctx + 0x1C);
            dl = pkt + 0x10;
            *(unsigned char **)(ctx + 0x1C) = pkt + 0xC30;
        }
        *(u128 *)(pkt + 0x10) = 0;
        *(int *)(pkt + 0x1C) = 0x500000C1;
        *(long long *)(pkt + 0x20) = (long long)(int)0x300E4000 << 32 | (unsigned)0x8040;
        dl += 0x20;
        *(long long *)(pkt + 0x28) = 0x421;
        for (j = 0, u = -16.0f; j < 0x20; j++) {
            va.x = g->origin[0] + u;
            va.y = (g->origin[1] - 10.0f) + v;
            va.z = g->origin[2] + 0.1f * g->c[i][j].h;
            va.w = 0x3F800000;
            vb.x = g->c[i][j].u;
            vb.y = g->c[i][j].v;
            vb.z = 128.0f * (1.0f - g->fade);
            vb.w = 0;
            func_001DAE10(dl, &va, &vb, g->shade);

            va.x = g->origin[0] + u;
            va.y = 1.1612903f + ((g->origin[1] - 10.0f) + v);
            va.z = g->origin[2] + 0.1f * g->c[i + 1][j].h;
            va.w = 0x3F800000;
            vb.x = g->c[i + 1][j].u;
            vb.y = g->c[i + 1][j].v;
            vb.z = 128.0f * (1.0f - g->fade);
            vb.w = 0;
            func_001DAE10(dl + 0x30, &va, &vb, g->shade);

            dl += 0x60;
            u += 1.0f;
        }
        v += 1.1612903f;
    }
    func_001D1F20(3);
    func_001D1FF0(3, 1);
    {
        char *ctx = D_00275670;
        *(char *)(*(char **)(ctx + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
        *(short *)(*(char **)(ctx + 0x1C) + 0) = 0;
        *(char **)(ctx + 0x1C) = *(char **)(ctx + 0x1C) + 0x10;
    }
    func_00102948(out, g->origin);
    out[2] = 0x43E38000;
    func_001CAAC0(out, first);
}
