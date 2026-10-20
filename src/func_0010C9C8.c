// COMPILER: eegcc
// CFLAGS: -O2
//
// Initialises the list head D_00276D00: +0 = a0, +4 = 0, +8 and +0xC = head +
// 0x10. Returns the head.
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

extern char D_00276D00[];

int *func_0010C9C8(int a0) {
    int *v1 = (int *)D_00276D00;
    *(int *)v1 = a0;
    a0 = (int)((char *)v1 + 0x10);
    *(int *)((char *)v1 + 0x4) = 0;
    *(int **)((char *)v1 + 0xC) = (int *)a0;
    *(int **)((char *)v1 + 0x8) = (int *)a0;
    return v1;
}
