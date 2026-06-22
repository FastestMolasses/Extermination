// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern int D_00275C08;
extern char D_008172C0[];

int func_001D73A0(int arg0) {
    if (arg0 >= 0) {
        if (arg0 < 0x10) {
            goto body;
        }
    }
    return 0;
body:
    return *(int *)(D_00275C08 * 0x48
        + (&D_008172C0[(1 - *(int *)(D_00275670 + 0x9C)) * 0x480]) + 8);
}
