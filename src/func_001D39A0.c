// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Sibling of func_001D3900: same 0x10-byte command-record writer into the
// per-slot list at D_00275670[a0]. Differs only in the func_001D1F80 third
// arg (3) and the bank-base global D_00816C40.
extern char *D_00275670;
extern char D_00816C40[0x10000];
extern void func_001D1F80(int a0, int a1, int a2);
extern void func_001D37D0(int a0, int a1, int a2, char *a3);

void func_001D39A0(int a0, int a1)
{
    int v0;
    char *t0;
    char *ptr;
    char *v1;
    int t1;
    char **a3;

    func_001D1F80(a0, 2, 3);
    v1 = D_00275670;
    t0 = D_00816C40;
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
