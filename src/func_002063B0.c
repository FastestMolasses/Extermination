// CFLAGS: -O4,p -sdatathreshold 0

static inline int lt(int a, int b) {
    return a < b;
}

void func_002063B0(char *a0, int *a1, int *a2, int *a3, int *t0) {
    int *in = (int *)a0;
    if (in[0] == 0) {
        char *p = a0 + 4;
        if (in && in) {
        }
        *a1 = (int)(p + in[0xB]);
        *a2 = 0x28 - in[0xB];
        *a3 = in[0xC];
        *t0 = in[0xF];
    } else {
        int t1 = in[0xF] - in[0xE];
        int v1 = in[0xF] - in[0xD];
        int c = lt(v1, t1);
        if (!c) {
            *a1 = in[0xC] + in[0xD];
            *a2 = t1;
            *a3 = 0;
            *t0 = 0;
        } else {
            *a1 = in[0xC] + in[0xD];
            *a2 = in[0xF] - in[0xD];
            *a3 = in[0xC];
            *t0 = t1 - (in[0xF] - in[0xD]);
        }
    }
}
