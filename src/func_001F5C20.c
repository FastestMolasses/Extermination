// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char *func_001F5640(void);
extern void func_001F5940(int a0, float *a1, int a2);

void func_001F5C20(void) {
    unsigned char *p;
    int i;
    float local[4];

    p = func_001F5640();
    if (p == 0) return;
    i = 0;
    while (*(short *)(p + 0) >= 0) {
        local[0] = *(float *)(p + 0xC);
        local[1] = *(float *)(p + 0x10);
        local[2] = *(float *)(p + 0x14);
        local[3] = 1.0f;
        func_001F5940(*(short *)(p + 4), local, i);
        p += 0x28;
        i += 1;
    }
}
