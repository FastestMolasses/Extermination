// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Allocator/initializer: allocates an object via func_001EF9D0 (called with
// f12=1.0f and the original a0 passed through), then if non-null initializes a
// sub-structure at +0xB0 via func_00102948(obj+0xB0, arg1), zeroes fields
// 0xC0/0xC4/0xC8/0xCC and sets float field 0xBC = 1.0f. Returns the object.
extern char *func_001EF9D0(float, int);
extern void func_00102948(char *, int);

char *func_001EFD20(int arg0, int arg1) {
    char *p;

    p = func_001EF9D0(1.0f, arg0);
    if (p) {
        func_00102948(p + 0xB0, arg1);
        *(int *)(p + 0xC4) = 0;
        *(int *)(p + 0xC0) = 0;
        *(int *)(p + 0xCC) = 0;
        *(int *)(p + 0xC8) = 0;
        *(float *)(p + 0xBC) = 1.0f;
    }
    return p;
}
