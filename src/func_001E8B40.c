// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275C20;

void func_001E8B40(int a0) {
    char *v1;
    int a1;

    a1 = 1;
    switch (a0) {
    case 0:
        v1 = *(char **)(D_00275C20 + 0xa0b8);
        v1[5] = a1;
        break;
    case 1:
        v1 = *(char **)(D_00275C20 + 0x58);
        v1[5] = a1;
        break;
    }
}
