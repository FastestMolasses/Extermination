// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char *func_001EF9D0(int kind, void *pos, float scale);
extern void func_00102948(void *, void *);

char *func_001EFE00(int kind, char *src) {
    float pos[4];
    char *n;

    func_00102948(pos, src + 0xB0);
    switch (kind) {
    case 0x80000027:
        pos[1] += 10.0f;
        break;
    }
    n = func_001EF9D0(kind, pos, 1.0f);
    if (n) {
        *(int *)(n + 0x24) = *(int *)(src + 0x14);
        func_00102948(n + 0xB0, src + 0xB0);
        func_00102948(n + 0xC0, src + 0xC0);
    }
    return n;
}
