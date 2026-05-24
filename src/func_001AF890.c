// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275BD0;
extern short D_00275BCC;

void func_001AF890(char *a0) {
    int a1;
    char *v1;
    int a1b;
    int v1b;
    short v1c;
    a1 = 0xd;
    v1 = a0;
    do {
        *(int *)(v1 + 0) = 0;
        *(int *)(v1 + 4) = 0;
        *(int *)(v1 + 8) = 0;
        *(int *)(v1 + 12) = 0;
        a1--;
        v1 += 16;
    } while (a1 != 0);
    a1b = D_00275BD0;
    v1b = a1b - 4;
    D_00275BD0 = v1b;
    *(int *)v1b = (int)a0;
    v1c = D_00275BCC;
    v1c = v1c + 1;
    D_00275BCC = v1c;
}
