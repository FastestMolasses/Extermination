// CFLAGS: -O4,p -sdatathreshold 0
extern short D_00810E80;
extern char D_0028F700[0x10000];

void func_001CB8A0(int a0, int a1, int *a2, int *a3) {
    short t1;
    int t0;
    int a1_addr;
    int a4;
    int v1;
    int at;

    t1 = D_00810E80;
    a4 = a1 << 6;
    a1_addr = (int)D_0028F700;
    v1 = 0x20000000;
    t0 = t1 << 3;
    t0 = t0 - t1;
    t0 = t0 << 16;
    a1_addr = a1_addr + t0;
    at = 0x1f3ec0;
    a1_addr = a1_addr + at;
    a1_addr = a1_addr + a4;
    *(int *)a1_addr = v1;
    v1 = a1_addr + 32;
    a0 = (int)((unsigned long long)((long long)v1 << 36) >> 36);
    v1 = (int)((unsigned long long)((long long)a1_addr << 36) >> 36);
    *(int *)(a1_addr + 4) = a0;
    *a2 = v1;
    *a3 = a0;
}
