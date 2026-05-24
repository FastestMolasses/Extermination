// CFLAGS: -O4,p -sdatathreshold 0
extern char D_20000010[1];
extern char D_20000050[1];

void func_001145A0(int a0) {
    int v0;
    int a1;
    char *a2;
    char *a3;
    a0 = a0 | 0x20000000;
    v0 = *(int *)a0;
    if (v0 != 0) {
        a2 = *(char **)(a0 + 8);
        if (v0 > 0) {
            a3 = (char *)a0;
            a1 = 0;
            do {
                v0 = *(unsigned char *)(a3 + (int)D_20000010 + a1);
                a1++;
                *a2 = v0;
                a2++;
                v0 = *(int *)a0;
            } while (a1 < v0);
        }
    }
    v0 = *(int *)(a0 + 4);
    if (v0 == 0) return;
    a2 = *(char **)(a0 + 12);
    if (v0 <= 0) return;
    a3 = (char *)a0;
    a1 = 0;
    do {
        v0 = *(unsigned char *)(a3 + (int)D_20000050 + a1);
        a1++;
        *a2 = v0;
        a2++;
        v0 = *(int *)(a0 + 4);
    } while (a1 < v0);
}
