// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275670;

void func_001D4A90(int a0, int *a1) {
    int t1;
    int t0;
    int t2;
    int v1;
    int a2;
    int a3;
    int *p;
    t1 = a1[0];
    if (t1 <= 0) return;
    t0 = (int)a1 + 64;
    t2 = a0 * 4;
    a2 = 0x30;
    if (t1 >= 504) {
        a0 = 504;
    } else {
        a0 = t1;
    }
    a3 = (int)(*D_00275670) + t2;
    v1 = a0 * 65;
    t0 = (int)a1 + 64;
    a0 = v1 * 2;
    v1 = v1 * 32;
    do {
        p = (int *)(a3 + 16);
        *(int *)a3 = (int)p + 16;  /* dummy to get a3 address */
        p = (int *)*(int *)(a3 + 16);
        t1 -= 504;
        ((char *)p)[3] = a2;
        p = (int *)*(int *)(a3 + 16);
        ((int *)((char *)p + 4))[0] = t0;
        t0 += v1;
        p = (int *)*(int *)(a3 + 16);
        ((short *)p)[0] = a0;
        p = (int *)*(int *)(a3 + 16);
        p = (int *)((char *)p + 16);
        *(int *)(a3 + 16) = (int)p;
    } while (t1 > 0);
}
