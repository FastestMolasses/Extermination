// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 60.43% NEARMISS).
// Fixes read from the original instructions:
// - The grid is the [16][16] array of 0x18-byte WaterVtx records (as in
//   func_001E13E0) written as plain indexed for-loops; the original's pointer
//   walkers, the unscheduled row setup and the per-row (float)i / 16.0f divide
//   are mwcc's loop-invariant hoisting and strength reduction of that indexing
//   (the old "scheduler divergence" was the hand-written pointer form).
// - The wrapped row indices (i - 1) & 0xF and (i + 1) & 0xF are locals set at
//   the top of the outer loop.
// - The laplacian is accumulated in sum (0, row above, row below, left, right,
//   minus 0.25 + 4 * height) BEFORE the velocity update, which is the two
//   compound statements w *= 0.961f; w += 0.0022f * sum.
// - The splash impulses go to the velocity field (.w, splat's D_0081E0FC) in
//   the order centre, left, right, above, below, then the four diagonals; the
//   orthogonal constant is 0.90000004f (0x3F666667), not 0.9f.
//
// Water surface step over the 16x16 D_0081E0F0 grid (0x18-byte records, same
// layout func_001E1010 initializes: x, y, height z, velocity w, u, v). The
// first pass damps each cell's velocity by 0.961 and adds 0.0022 times the
// wrapped 4-neighbour laplacian of height (minus a 0.25 bias), and derives
// u = j/16 + 0.03 * (z - right z), v = i/16 + 0.03 * (z - lower z). Then two
// PRNG-picked cells (a = column, b = row from func_00122BB8 >> 6 & 0xF) get a
// velocity impulse: 1.2 at the centre, 0.9 orthogonal, 0.54 diagonal (wrapped).
// The final pass integrates height += velocity over the whole grid.
typedef struct {
    float x;
    float y;
    float z;
    float w;
    float u;
    float v;
} WaterVtx;

extern WaterVtx D_0081E0F0[16][16];

extern int func_00122BB8(void);

void func_001E10A0(void) {
    int i;
    int j;
    float sum;
    int im1;
    int ip1;
    int a;
    int b;

    for (i = 0; i < 16; i++) {
        im1 = (i - 1) & 0xF;
        ip1 = (i + 1) & 0xF;
        for (j = 0; j < 16; j++) {
            sum = 0.0f;
            sum += D_0081E0F0[im1][j].z;
            sum += D_0081E0F0[ip1][j].z;
            sum += D_0081E0F0[i][(j - 1) & 0xF].z;
            sum += D_0081E0F0[i][(j + 1) & 0xF].z;
            sum -= 0.25f + 4.0f * D_0081E0F0[i][j].z;
            D_0081E0F0[i][j].w *= 0.961f;
            D_0081E0F0[i][j].w += 0.0022f * sum;
            D_0081E0F0[i][j].u = (float)j / 16.0f + 0.03f * (D_0081E0F0[i][j].z - D_0081E0F0[i][(j + 1) & 0xF].z);
            D_0081E0F0[i][j].v = (float)i / 16.0f + 0.03f * (D_0081E0F0[i][j].z - D_0081E0F0[ip1][j].z);
        }
    }

    a = (func_00122BB8() >> 6) & 0xF;
    b = (func_00122BB8() >> 6) & 0xF;
    D_0081E0F0[b][a].w += 1.2f;
    D_0081E0F0[b][(a - 1) & 0xF].w += 0.90000004f;
    D_0081E0F0[b][(a + 1) & 0xF].w += 0.90000004f;
    D_0081E0F0[(b - 1) & 0xF][a].w += 0.90000004f;
    D_0081E0F0[(b + 1) & 0xF][a].w += 0.90000004f;
    D_0081E0F0[(b - 1) & 0xF][(a - 1) & 0xF].w += 0.54f;
    D_0081E0F0[(b - 1) & 0xF][(a + 1) & 0xF].w += 0.54f;
    D_0081E0F0[(b + 1) & 0xF][(a - 1) & 0xF].w += 0.54f;
    D_0081E0F0[(b + 1) & 0xF][(a + 1) & 0xF].w += 0.54f;

    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            D_0081E0F0[i][j].z += D_0081E0F0[i][j].w;
        }
    }
}
