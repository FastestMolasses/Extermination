// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Reads state byte at +0x1F0; returns 1 if it equals 0x17, else 0.
// CW emits the paddub-zeroing comparison codegen that mwcc 2.3.3 reproduces byte-identically.
int func_00182D40(int a0) {
    if (*(unsigned char *)(a0 + 0x1F0) == 0x17) {
        return 1;
    }
    return 0;
}
