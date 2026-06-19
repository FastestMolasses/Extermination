// CFLAGS: -O4,p -sdatathreshold 0
typedef unsigned long long u64;
typedef int s128 __attribute__((mode(TI)));

extern char D_007635C0[];
extern char *func_001CB5F0(void *buf, int a1, int a2);
extern void func_001CB900(void *buf, int a1, int a2, int a3);

void func_001F7CF0(int *a0, int *a1, int *a2) {
    int n = 8;
    char *p = func_001CB5F0(D_007635C0, 0xFFF000, n);
    *(s128 *)(p + 0x0) = 0;
    *(int *)(p + 0xC) = 0x50000007;
    *(u64 *)(p + 0x10) = (u64)0x8001 | ((u64)0x602B4000 << 32);
    *(u64 *)(p + 0x18) = 0x424216;
    *(u64 *)(p + 0x20) = (u64)0x55322090 | ((u64)0x20041285 << 32);
    *(u64 *)(p + 0x28) = (u64)0x55322090 | ((u64)0x20041285 << 32);
    *(int *)(p + 0x30) = a2[0];
    *(int *)(p + 0x34) = a2[1];
    *(int *)(p + 0x38) = a2[2];
    *(int *)(p + 0x3C) = a2[3];
    *(int *)(p + 0x40) = 0;
    *(int *)(p + 0x44) = 0;
    *(int *)(p + 0x48) = 0x3F800000;
    *(int *)(p + 0x4C) = 0;
    *(int *)(p + 0x50) = a0[0] << 4;
    *(int *)(p + 0x54) = a0[1] << 4;
    *(int *)(p + 0x58) = 0xFFFFFF;
    *(int *)(p + 0x5C) = 0;
    *(int *)(p + 0x60) = 0x3F800000;
    *(int *)(p + 0x64) = 0x3F800000;
    *(int *)(p + 0x68) = 0x3F800000;
    *(int *)(p + 0x6C) = 0;
    *(int *)(p + 0x70) = a1[0] << 4;
    *(int *)(p + 0x74) = a1[1] << 4;
    *(int *)(p + 0x78) = 0xFFFFFF;
    *(int *)(p + 0x7C) = 0;
    func_001CB900(D_007635C0, 0xFFF000, 1, 0xFFFFFF);
}
