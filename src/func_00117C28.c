// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_0027CCC0[];

void func_00117C28(unsigned char *a0)
{
    int i;
    unsigned char *base;
    unsigned char *p;
    int one;
    int v40;
    unsigned int target;

    if (*(unsigned short *)(a0 + 0x32) == 1) {
        *(int *)(a0 + 0x20) = 0;
        *(short *)(a0 + 0x34) = 0;
        *(short *)(a0 + 0x42) = 0;
    } else {
        *(short *)(a0 + 0x30) = 0;
        *(int *)(a0 + 0x8) = 0x110;
    }

    *(short *)(a0 + 0x3E) = 1;
    target = *(unsigned int *)(a0 + 0x18);

    base = D_0027CCC0;
    p = base + 0x38;
    one = 1;
    v40 = 0x40;
    i = 0x2F;
    do {
        if (*(unsigned short *)(p - 0x32) == target &&
            *(unsigned short *)(p - 0x1E) == one) {
            *(short *)(p - 0x24) = 0;
            *(short *)(p + 0x0) = v40;
        }
        p += 0x6A;
        i--;
    } while (i >= 0);

    *(unsigned char *)(a0 + 1) = *(unsigned char *)(a0 + 0);
}
