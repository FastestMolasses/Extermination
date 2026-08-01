// NEARMISS func_001DB480  (vram 0x001DB480, 0x378 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 55.37% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// VU0 macro-mode + loop strength-reduction. (1) HARD WALL: after `func_001CD370(3)` the target uploads a 4x4 matrix into the COP2 register file with four `lqc2 $vf24..$vf27, N($v0)` instructions. mwcc 2.3 has no C construct or intrinsic for lqc2 and the corpus has zero precedent for mixed C + inlin...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: builds and submits the whole "water/heat haze grid" GS display list.
// D_00275670 is the render-context pointer; its field at +0x1C is the running GS
// packet write pointer. D_00818000 is the grid workspace: a 0x1F x 0x20 array of
// 16-byte cells (row stride 0x200 -> `base + (i << 9) + j * 0x10`; each cell holds
// u/v at +0, +4 and a height at +0xC), plus a parameter block at the end --
// +0x4000 = fade/alpha scalar, +0x4020..+0x4028 = grid origin xyz, +0x4030 = the
// shared shading/colour block handed to every strip vertex.
//
//   1. Snapshot the current packet pointer (used as the DMA chain head at the end),
//      reserve the 0x258000 VRAM arena for two 8-bit buffers (func_001D6B10 /
//      func_001D6BA0), select context 3 (func_001D1FF0) and program the 15-argument
//      GS environment (func_001D6C90: 8 EABI register args in a0-a3/t0-t3 followed by
//      seven 64-bit stack slots).
//   2. func_001CD370(3) returns the current view matrix, which is uploaded to VU0 as
//      vf24..vf27 (see WALL below -- not expressible in C).
//   3. For each of the 0x1F rows: emit a GIFtag/AD header into the packet (byte +3 =
//      0x10 quad count, word +4 = 0, halfword +0 = 0xC2 tag id), bump the packet
//      pointer by 0xC30, zero the first quadword and write the tag words
//      0x500000C1 / 0x300E400000008040 / 0x421, then walk the 0x20 columns emitting a
//      TRIANGLE-STRIP vertex PAIR per cell through func_001DAE10(dst, pos, uvc, shade):
//        pos = { origin.x + u, (origin.y - 10) + v, origin.z + 0.1 * cell.height, 1.0 }
//        uvc = { cell.u, cell.v, 128 * (1 - fade), 0 }
//      The second vertex of the pair is the same column one row down: v is offset by
//      the row pitch 1.1612903f (36/31) and the cell is read 0x200 bytes further on.
//      u advances by 1.0 per column, v by 1.1612903f per row, and the write cursor by
//      0x60 (two 0x30-byte vertex records) per column.
//   4. Close the chain (func_001D1F20, context 3 back to buffer 1), emit the terminating
//      GIFtag (byte +3 = 0x60, word +4 = 0, halfword +0 = 0), advance the packet pointer
//      by 0x10, build the transfer descriptor with func_00102948, stamp 455.0f
//      (0x43E38000) into its +8 field and kick the DMA with func_001CAAC0.
//
// WALL: after func_001CD370(3) the original issues four `lqc2 $vf24..$vf27, N($v0)`
// COP2 loads. mwcc 2.3 cannot emit those from C and this project keeps every VU0
// function as whole-function `asm void`, so this function can never reach objdiff
// 100.0 as readable C. The secondary residual is loop strength-reduction: the target
// recomputes the cell address from the two counters each iteration (frame 0xE0,
// s0..s4), mwcc builds induction pointers, spills the counters and grows the frame
// to 0x150. Idiom-19 inlining of the cell expression compiles to an identical object.
extern void func_001D6B10(int, int, int, int);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D6C90(int, int, int, int, int, int, int, int,
                          long long, long long, long long, long long,
                          long long, long long, long long);
extern void *func_001CD370(int);
extern void func_001DAE10(unsigned char *dst, void *a, void *b, float *c);
extern void func_001D1F20(int);
extern void func_00102948(int *, float *, unsigned char *);
extern void func_001CAAC0(int *, unsigned char *);

typedef int u128 __attribute__((mode(TI)));

extern unsigned char *D_00275670;
extern unsigned char D_00818000[];

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
    unsigned char *base;
    unsigned char *dl;
    unsigned char *pkt;
    unsigned char *ctx;
    unsigned char *cell;
    unsigned char *first;
    int arena;
    float u;
    float v;
    int i;
    int j;

    first = *(unsigned char **)(D_00275670 + 0x1C);
    base = D_00818000;
    arena = 0x258000;
    func_001D6B10(3, arena, 8, 8);
    func_001D6BA0(3, arena, 8, 8, 3, 0);
    func_001D1FF0(3, 3);
    func_001D6C90(3, 0, 1, 0, 0, 1, 0, 0, 1, 2, 0, 1, 0, 1, 0);
    func_001CD370(3);
    /* the view matrix returned above is uploaded to VU0 here:
       lqc2 vf24..vf27, 0x00/0x10/0x20/0x30(result) -- see WALL */
    v = 0.0f;
    i = 0;
    while (i < 0x1F) {
        ctx = D_00275670;
        *(unsigned char *)(*(unsigned char **)(ctx + 0x1C) + 3) = 0x10;
        u = -16.0f;
        *(int *)(*(unsigned char **)(ctx + 0x1C) + 4) = 0;
        j = 0;
        *(short *)(*(unsigned char **)(ctx + 0x1C) + 0) = 0xC2;
        pkt = *(unsigned char **)(ctx + 0x1C);
        *(unsigned char **)(ctx + 0x1C) = pkt + 0xC30;
        dl = pkt + 0x10;
        *(u128 *)(pkt + 0x10) = 0;
        *(int *)(pkt + 0x1C) = 0x500000C1;
        *(long long *)(pkt + 0x20) = 0x300E400000008040LL;
        dl += 0x20;
        *(long long *)(pkt + 0x28) = 0x421;
        while (j < 0x20) {
            cell = base + (i << 9) + j * 0x10;
            va.x = *(float *)(base + 0x4020) + u;
            va.y = (*(float *)(base + 0x4024) - 10.0f) + v;
            va.z = *(float *)(base + 0x4028) + 0.1f * *(float *)(cell + 0xC);
            va.w = 0x3F800000;
            vb.x = *(float *)(cell + 0);
            vb.y = *(float *)(cell + 4);
            vb.z = 128.0f * (1.0f - *(float *)(base + 0x4000));
            vb.w = 0;
            func_001DAE10(dl, &va, &vb, (float *)(base + 0x4030));

            cell = base + (i << 9) + j * 0x10;
            va.x = *(float *)(base + 0x4020) + u;
            va.y = 1.1612903f + ((*(float *)(base + 0x4024) - 10.0f) + v);
            va.z = *(float *)(base + 0x4028) + 0.1f * *(float *)(cell + 0x20C);
            va.w = 0x3F800000;
            vb.x = *(float *)(cell + 0x200);
            vb.y = *(float *)(cell + 0x204);
            vb.z = 128.0f * (1.0f - *(float *)(base + 0x4000));
            vb.w = 0;
            func_001DAE10(dl + 0x30, &va, &vb, (float *)(base + 0x4030));

            dl += 0x60;
            u += 1.0f;
            j++;
        }
        i++;
        v += 1.1612903f;
    }
    func_001D1F20(3);
    func_001D1FF0(3, 1);
    ctx = D_00275670;
    *(unsigned char *)(*(unsigned char **)(ctx + 0x1C) + 3) = 0x60;
    *(int *)(*(unsigned char **)(ctx + 0x1C) + 4) = 0;
    *(short *)(*(unsigned char **)(ctx + 0x1C) + 0) = 0;
    *(unsigned char **)(ctx + 0x1C) = *(unsigned char **)(ctx + 0x1C) + 0x10;
    func_00102948(out, (float *)(base + 0x4020), ctx);
    out[2] = 0x43E38000;
    func_001CAAC0(out, first);
}
