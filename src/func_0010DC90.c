// COMPILER: eegcc
// CFLAGS: -O2
//
// Swaps in a new (a0, a1) pair at D_00277218 + 0xC / + 0x10 and returns the
// old +0xC value.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern char D_00277218[];

int func_0010DC90(int a0, int a1) {
    int *v1 = (int *)D_00277218;
    int v0 = *(int *)((char *)v1 + 0xC);
    *(int *)((char *)v1 + 0xC) = a0;
    *(int *)((char *)v1 + 0x10) = a1;
    return v0;
}
