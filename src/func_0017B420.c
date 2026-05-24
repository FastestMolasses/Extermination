// CFLAGS: -O4,p -sdatathreshold 0
extern short D_00810CAC;
extern short D_00810CAE;

int func_0017B420(void) {
    int v0 = D_00810CAC;
    if (v0 != 0) {
        v0--;
        D_00810CAC = v0;
        D_00810CAE = 0x64;
    }
    return 1;
}
