// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Constructor: allocate an 8-tag object, set type byte (5) at +3 and a flag
// byte (arg1) at +0xD, copy 3 floats from parent (arg0) at 0xB0/0xB4/0xB8,
// init a matrix region with 1.0f (0x3F800000) identity entries at
// 0x60/0x64/0x68/0x6C and 0xBC/0xCC (zeros at 0xC0/0xC4/0xC8), and store a
// vtable-style function pointer (func_00188340) at +0x10.
void *func_001AFA90(int);
extern void func_00188340(void);

void func_0016BAE0(unsigned char *arg0, unsigned char arg1) {
    unsigned char *p;

    p = (unsigned char *)func_001AFA90(8);
    if (p != 0) {
        *(unsigned char *)(p + 3) = 5;
        *(unsigned char *)(p + 0xD) = arg1;
        *(float *)(p + 0xB0) = *(float *)(arg0 + 0xB0);
        *(float *)(p + 0xB4) = *(float *)(arg0 + 0xB4);
        *(float *)(p + 0xB8) = *(float *)(arg0 + 0xB8);
        *(int *)(p + 0xBC) = 0x3F800000;
        *(int *)(p + 0xC0) = 0;
        *(int *)(p + 0xC4) = 0;
        *(int *)(p + 0xC8) = 0;
        *(int *)(p + 0xCC) = 0x3F800000;
        *(int *)(p + 0x60) = 0x3F800000;
        *(int *)(p + 0x64) = 0x3F800000;
        *(int *)(p + 0x68) = 0x3F800000;
        *(int *)(p + 0x6C) = 0x3F800000;
        *(void (**)(void))(p + 0x10) = &func_00188340;
    }
}
