// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275C20;

void func_001E8B40(int a0) {
    switch (a0) {
    case 0:
        *(char *)(D_00275C20[0xa0b8 / 4] + 5) = 1;
        break;
    case 1:
        *(char *)(D_00275C20[0x58 / 4] + 5) = 1;
        break;
    }
}
