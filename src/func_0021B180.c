// CFLAGS: -O4,p -sdatathreshold 4
extern void func_001D2830(int, int);
extern char *D_00275888;

void func_0021B180(void) {
    char *v1 = D_00275888;
    v1[3] = 0;
    v1[2] = 0;
    v1[1] = 0;
    v1[0] = 0;
    *(int *)(v1 + 4) = 0;
    *(int *)(v1 + 0x18) = 0x8000;
    func_001D2830(0x20, 0);
}
