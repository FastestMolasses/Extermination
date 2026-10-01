// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char *func_001EF9D0(int kind, void *pos, float scale);
extern void func_00102948(void *, void *);

char *func_001EFF10(int kind, char *obj, void *v0, void *v1, void *v2, void *v3, float value) {
    char *n;
    char *b;

    n = func_001EF9D0(kind, obj + 0x30, 1.0f);
    if (n) {
        b = n + 0x1F0;
        *(char **)(n + 0x1F0) = obj;
        *(float *)(n + 0x1FC) = value;
        func_00102948(b + 0x10, v0);
        func_00102948(b + 0x20, v1);
        func_00102948(b + 0x30, v2);
        func_00102948(b + 0x40, v3);
    }
    return n;
}
