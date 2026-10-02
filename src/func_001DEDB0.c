// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Record address in the render context D_00275670: +0x2490 for id 9,
// +0x2470 for any other id.
extern char *D_00275670;
char *func_001DEDB0(int which) {
    switch (which) {
    case 2:
    default: return D_00275670 + 0x2470;
    case 9: return D_00275670 + 0x2490;
    }
}
