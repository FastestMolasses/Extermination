// CFLAGS: -O4,p -sdatathreshold 0
extern char *D_00275670;

static char *func_001DEDB0(int a0) {
    if (a0 != 9) {
        return D_00275670 + 0x2470;
    }
    return D_00275670 + 0x2490;
}

void func_001DEE80(int a0, int *src) {
    char *dst;
    dst = func_001DEDB0(a0);
    *(int *)(dst + 0x10) = src[0];
    *(int *)(dst + 0x14) = src[1];
    *(int *)(dst + 0x18) = src[2];
}
