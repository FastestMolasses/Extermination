// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 58.46% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Fixes read from the original instructions:
//  - the four VU0 matrix-row loads after func_001CD370(0) are inline asm.
//  - the source grid is D_0081C0F0[row][column] of 16-byte vertices.
//  - func_001CAAC0 takes two arguments (arg0, saved cursor); the old C passed a
//    third.
//  - the render-context pointer is a block-scoped local (as in func_001DB480); the
//    strip cursor is taken as packet + 0x10 inside that block; the VRAM arena
//    0x258000 is a local; locals declared i, j, acc, dl, p, wp0.
//
// Draws a 16x16 vertex grid as 15 triangle strips into the display list of
// the render context D_00275670 (word +0x1C is the running write cursor).
// arg0 = transform context for func_001028B8, arg1 = passed through to the
// per-vertex emitter func_001E0240, t = morph parameter for func_00103230.
// Setup on unit 3 with texture buffer 0x258000 (func_001D6B10, func_001D1F80,
// func_001D6BA0, func_001D1FF0); the matrix from func_001CD370(0) is loaded
// into VU0 rows 24..27 for func_001E0240. Each row opens a 0x630-byte packet
// (header byte +3 = 0x10, word +4 = 0, halfword +0 = 0x62, control word
// 0x50000061, GIFtag 0x302E400000008020 / 0x421) and emits two strip vertices
// per column (rows i and i+1). Teardown: func_001D1F20(3), func_001D1FF0(3, 1),
// close the list (byte +3 = 0x60) and kick with func_001CAAC0(arg0, saved cursor).

//
// SEMANTICS: draws a 16x16 vertex grid as 15 triangle strips into the display
// list held in the render context D_00275670 (a gp-relative pointer; word
// +0x1C is the running DMA/display-list write cursor).
//   arg0 = transform context handed to func_001028B8 (per-vertex transform)
//   arg1 = passed through to func_001E0240 (per-vertex emit)
//   t    = float interpolation/morph parameter handed to func_00103230
//   D_0081C0F0 = the source vertex grid: 16 bytes per vertex, 0x100 per row
// Sequence:
//   - save the current write cursor (wp0) for the final kick;
//   - render-state setup on unit 3 with texture buffer 0x258000:
//       func_001D6B10(3, 0x258000, 8, 8), func_001D1F80(3, 2, 1),
//       func_001D6BA0(3, 0x258000, 8, 8, 0, 0), func_001D1FF0(3, 3);
//   - func_001CD370(0) returns the base of a 4-quadword (4x4) matrix. The
//     ORIGINAL then loads its four rows into the VU0 registers vf24..vf27 with
//     four quadword loads (rows at +0x0/+0x10/+0x20/+0x30 of the result), which
//     func_001E0240 consumes. There is no readable-C construct that emits
//     `lqc2` into a fixed vf register (every matched lqc2 function in this
//     project is asm-void), so those four instructions are the one piece of
//     this function that cannot be expressed here — the call is kept so the
//     matrix fetch and its ordering stay visible.
//   - per row i (0..14) open a 0x630-byte packet at the cursor: header byte
//     [3] = 0x10, word [4] = 0, halfword [0] = 0x62; advance the cursor by
//     0x630; zero the quadword at +0x10; control word 0x50000061 at +0x1C;
//     GIFtag 0x302E400000008020 at +0x20 and 0x421 at +0x28; vertex data
//     begins at +0x30;
//   - per column j (0..15) emit the two strip vertices (row i then row i+1):
//     morph the source vertex by t (func_00103230), transform it
//     (func_001028B8), then emit it (func_001E0240), which threads a running
//     accumulator `acc` and consumes 0x30 bytes of the packet per vertex;
//   - teardown: func_001D1F20(3), func_001D1FF0(3, 1), close the list (byte
//     [3] = 0x60, word [4] = 0, halfword [0] = 0), advance the cursor by 0x10,
//     and kick with func_001CAAC0(arg0, wp0).
//
typedef int u128 __attribute__((mode(TI)));

extern char *D_00275670;
extern char D_0081C0F0[][16][16];

extern void func_001D6B10(int, int, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D1F20(int);
extern char *func_001CD370(int);
extern void func_00103230(float *, char *, float);
extern void func_001028B8(float *, int, float *);
extern int func_001E0240(char *, float *, int, int);
extern int func_001CAAC0(int world_pos, char *payload);

void func_001E04E0(int arg0, int arg1, float t)
{
    int i;
    int j;
    int acc;
    char *dl;
    char *p;
    char *wp0;
    float v[4];
    char *m;
    int arena;

    wp0 = *(char **)(D_00275670 + 0x1C);
    arena = 0x258000;
    func_001D6B10(3, arena, 8, 8);
    func_001D1F80(3, 2, 1);
    func_001D6BA0(3, arena, 8, 8, 0, 0);
    func_001D1FF0(3, 3);
    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));

    for (i = 0; i < 15; i++) {
        {
            char *ctx = D_00275670;
            *(char *)(*(char **)(ctx + 0x1C) + 3) = 0x10;
            *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
            *(short *)(*(char **)(ctx + 0x1C) + 0) = 0x62;
            p = *(char **)(ctx + 0x1C);
            dl = p + 0x10;
            *(char **)(ctx + 0x1C) = p + 0x630;
        }
        *(u128 *)(p + 0x10) = 0;
        *(int *)(p + 0x1C) = 0x50000061;
        *(long long *)(p + 0x20) = 0x8020 | ((long long)0x302E4000 << 32);
        *(long long *)(p + 0x28) = 0x421;
        dl += 0x20;
        acc = 0;
        for (j = 0; j < 16; j++) {
            func_00103230(v, D_0081C0F0[i][j], t);
            func_001028B8(v, arg0, v);
            acc = func_001E0240(dl, v, arg1, acc);
            dl += 0x30;
            func_00103230(v, D_0081C0F0[i + 1][j], t);
            func_001028B8(v, arg0, v);
            acc = func_001E0240(dl, v, arg1, acc);
            dl += 0x30;
        }
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
    func_001CAAC0(arg0, wp0);
}
