// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00821300[0x40];
extern int D_00275C90;

void func_0020E020(void) {
    char *p = (char *)&D_00821300[0];
    int i = 0;
    while (i < 0x10) {
        *(int *)(p + 0) = 0;
        *(int *)(p + 4) = 0;
        i++;
        *(int *)(p + 8) = 0;
        *(int *)(p + 0xC) = 0;
        p += 0x10;
    }
    D_00275C90 = 0;
}
