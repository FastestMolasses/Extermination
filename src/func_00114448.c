// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00241D68;
extern int func_00114360(int, int, int, int);

int func_00114448(int a0, int a1, int a2, int a3) {
    int v0 = func_00114360(a0, a1, a2, 0x40);
    if (v0 == 0) {
        D_00241D68 = 0xb;
    }
    return v0;
}
