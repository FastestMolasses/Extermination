// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275B40;

int func_001F8880(int a0) {
    int v0;
    int v1;
    v0 = D_00275B40[a0 / 6];
    v1 = (a0 % 6) << 5;
    return v0 + v1;
}
