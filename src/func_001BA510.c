// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008106B0;

void func_001BA510(void) {
    char *p = &D_008106B0;
    int i;
    i = 0;
    do {
        i++;
        p[0x24] = 0;
        p++;
    } while (i < 0xC);
}
