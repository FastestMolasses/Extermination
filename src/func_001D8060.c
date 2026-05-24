// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;

char *func_001D8060(int a0) {
    char *v0;
    int v1;
    int a1;

    v0 = D_00275670 + 0x220;
    if (a0 == -1) {
        return (char *)0;
    }
    a1 = 0;
    do {
        v1 = *(int *)(v0 + 0xc);
        if (v1 == a0) {
            return v0;
        }
        a1++;
        v0 += 0x80;
    } while (a1 < 32);
    return (char *)0;
}
