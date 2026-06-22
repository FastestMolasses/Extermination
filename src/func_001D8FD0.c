// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Camera/transform update. Caches a struct pointer from func_001D7B30, then tests
// bit 0x80 of func_001B0070's return. If set, drives the camera helpers with defaults
// (func_0021B970(0.0f, 110.0f) where 110.0f = 0x42DC0000; func_0021BA80(0,0,0)).
// Otherwise reads from the cached struct: func_0021B970(p+4, p+8) as floats and
// func_0021BA80(p+0xC, p+0x10, p+0x14) as ints. Finally calls func_0021B8E0. No return.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 2.3.3 is
// byte-identical; 991202 leaves a residual.
extern char *func_001D7B30(void);
extern int func_001B0070(void);
extern void func_0021B970(float, float);
extern void func_0021BA80(int, int, int);
extern void func_0021B8E0(void);

void func_001D8FD0(void) {
    char *p;

    p = func_001D7B30();
    if (func_001B0070() & 0x80) {
        func_0021B970(0.0f, 110.0f);
        func_0021BA80(0, 0, 0);
    } else {
        func_0021B970(*(float *)(p + 4), *(float *)(p + 8));
        func_0021BA80(*(int *)(p + 0xC), *(int *)(p + 0x10), *(int *)(p + 0x14));
    }
    func_0021B8E0();
}
