// COMPILER: eegcc
// CFLAGS: -O2
//
// Writes the pair (a1, a2) into entry a0 of the 8-byte-entry table that
// D_00277224 (a0 < 0) or D_0027722C (a0 >= 0) points to.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern int D_00277224;
extern int D_0027722C;

void func_0010DCA8(int a0, int a1, int a2) {
    int *p = (int *)(a0 * 8);
    if (a0 < 0) a0 = D_00277224; else a0 = D_0027722C;
    p = (int *)((char *)p + a0);
    p[0] = a1;
    p[1] = a2;
}
