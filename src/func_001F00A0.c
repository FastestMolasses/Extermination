// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char *func_001EF9D0(int kind, void *pos, float scale);
extern void func_00102948(void *, void *);

char *func_001F00A0(int kind, float *pos, float *dir, int owner) {
    char *n;

    n = func_001EF9D0(kind, pos, dir[3]);
    if (n) {
        func_00102948(n + 0xB0, pos);
        func_00102948(n + 0xC0, dir);
        *(float *)(n + 0xBC) = 1.0f;
        *(int *)(n + 0x38) = owner;
    }
    return n;
}
