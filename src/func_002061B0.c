// CFLAGS: -O4,p -sdatathreshold 4
extern unsigned int D_00275C80;
extern unsigned int D_00275C7C;
extern int D_007A55F8[2];
extern int func_001009C8(int);

void func_002061B0(int a0) {
    int s0 = a0;
    int v0 = func_001009C8(0);
    if (s0 != v0) {
        D_00275C80 = 1;
        D_007A55F8[0] = 0;
        D_00275C7C = 0;
    }
}
