// COMPILER: eegcc
// CFLAGS: -O2
//
// Clears the first word of entry a0 in the same table pair as func_0010DCA8.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern int D_00277224;
extern int D_0027722C;

void func_0010DCD8(int a0) {
    int *p = (int *)(a0 * 8);
    if (a0 < 0) a0 = D_00277224; else a0 = D_0027722C;
    p = (int *)((char *)p + a0);
    *p = 0;
}
