// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.88% NEARMISS).
// Fixes read from the original instructions:
// - The data at D_00818000 is one aggregate: a 0x20 x 0x20 grid of 0x10-byte cells
//   followed by a float block at +0x4000. Indexing it as g->cell[i][j] through a
//   single base pointer (instead of a separate byte row pointer) gives the
//   original's single symbol load plus a register copy for the row pointer.
// - Declaring the inner counter j before the outer counter i gives the original
//   register assignment of the two loop counters.

// Zeroes fields +8/+0xC of every cell of the 0x20 x 0x20 grid at D_00818000
// (row stride 0x200), then writes 8 float constants (as raw bit patterns) into
// the trailing block at base+0x4000..+0x402C.
typedef struct {
    int a;
    int b;
    int c;
    int d;
} Cell;

typedef struct {
    Cell cell[0x20][0x20];
    float f[12];
} Grid;

extern Grid D_00818000;

void func_001DAFA0(void) {
    Grid *g = &D_00818000;
    int j;
    int i;
    for (i = 0; i < 0x20; i++) {
        for (j = 0; j < 0x20; j++) {
            g->cell[i][j].d = 0;
            g->cell[i][j].c = 0;
        }
    }
    *(int *)&g->f[0] = 0x3F7DB22D;
    *(int *)&g->f[1] = 0x3CB43958;
    *(int *)&g->f[3] = 0x3ECCCCCD;
    *(int *)&g->f[2] = 0x3D4CCCCD;
    *(int *)&g->f[8] = 0x43990CCD;
    *(int *)&g->f[9] = 0x43380000;
    *(int *)&g->f[10] = 0x43E61CE0;
    *(int *)&g->f[11] = 0x3F800000;
}
