// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Area/effect command writer (mode 1). Sibling of func_001D3900/39A0/3A30:
// stages a 0x10-byte command record into the per-slot DMA/display list at
// D_00275670[a0]. First runs the func_001D3A30 setup, then a fixed
// func_001D1F80(a0, 2, 1) call, writes tag 0x30, the bank base D_00816E40 +
// (state[0x9c] << 7), a halfword count of 8, advances the slot write pointer
// by 0x10, then hands off to func_001D3AD0.
extern char *D_00275670;
extern char D_00816E40[0x10000];
extern void func_001D3A30(int a0, int a1);
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D3AD0(int a0, int a1, int a2, char *a3);

void func_001D3C40(int a0, int a1)
{
    int v0;
    char *t0;
    char *ptr;
    char *v1;
    int t1;
    char **a3;

    func_001D3A30(a0, a1);
    func_001D1F80(a0, 2, 1);
    v1 = D_00275670;
    t0 = D_00816E40;
    v0 = a0 << 2;
    t1 = *((int *)(v1 + 0x9c));
    a3 = (char **)(v1 + v0);
    ptr = a3[4];
    t1 = t1 << 7;
    t0 = t0 + t1;
    ptr[3] = 0x30;
    ptr = a3[4];
    *((char **)(ptr + 4)) = t0;
    ptr = a3[4];
    *((short *)ptr) = 8;
    ptr = a3[4];
    ((char **)(v1 + v0))[4] = ptr + 0x10;
    func_001D3AD0(a0, a1, 0x30, (char *)a3);
}
