// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Starts an effect: e = func_001EF9D0(kind, a, 1.0); when it succeeds a is
// copied to e+0xB0 and b to e+0xC0 (func_00102948), e+0x20 = w and e+0x94 = tag.
// Returns e.
extern char *func_001EF9D0(int kind, void *target, float scale);
extern void func_00102948(void *dst, void *src);

char *func_001EFFD0(int kind, void *a, void *b, short tag, float w) {
    char *e = func_001EF9D0(kind, a, 1.0f);
    if (e) {
        func_00102948(e + 0xB0, a);
        func_00102948(e + 0xC0, b);
        *(float *)(e + 0x20) = w;
        *(short *)(e + 0x94) = tag;
    }
    return e;
}
