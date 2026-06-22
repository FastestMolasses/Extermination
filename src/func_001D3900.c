// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Area/effect command writer: stages a 0x10-byte command record into the
// per-slot DMA/display list at D_00275670[a0]. After a fixed func_001D1F80
// setup call, writes tag 0x30, the bank base D_00816B40 + (state[0x9c] << 7),
// a halfword count of 8, advances the slot write pointer by 0x10, then hands
// off to func_001D37D0. Sibling of func_001D39A0/func_001D3A30 (differ only in
// the func_001D1F80 arg and bank-base global).
extern char *D_00275670;
extern char D_00816B40[0x10000];
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D37D0(int a0, int a1, int a2, char *a3);

void func_001D3900(int a0, int a1)
{
    int v0;
    char *t0;
    char *ptr;
    char *v1;
    int t1;
    char **a3;

    func_001D1F80(a0, 2, 2);
    v1 = D_00275670;
    t0 = D_00816B40;
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
    func_001D37D0(a0, a1, 0x30, (char *)a3);
}
