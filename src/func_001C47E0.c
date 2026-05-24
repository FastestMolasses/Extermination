// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_00810C64;

int func_001C47E0(int a0, int a1) {
    unsigned char *v1 = &D_00810C64 + a0;
    int v0 = *v1;
    if (v0 < a1) {
        return -1;
    }
    v0 -= a1;
    *v1 = v0;
    return *v1;
}
