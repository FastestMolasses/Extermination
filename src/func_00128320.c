// COMPILER: eegcc
// CFLAGS: -O2
// Pack the four args contiguously and forward a pointer to func_001277B0.
extern int func_001277B0(int *p);

int func_00128320(int a0, int a1, int a2, int a3) {
    int args[4];
    args[0] = a0;
    args[1] = a1;
    args[2] = a2;
    args[3] = a3;
    return func_001277B0(args);
}
