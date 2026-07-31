// COMPILER: eegcc
// CFLAGS: -O2

extern int D_002412F4;
extern int D_002414AC;

void func_00107AC8(int *ctx)
{
    char *dev;
    unsigned int src;
    unsigned int dst;
    unsigned int cur;
    int e0;
    int srcStride;
    int dstStride;
    int qwc;
    int passes;
    int i;
    int j;

    dev = *(char **)(D_002412F4 + 0x40);
    src = (unsigned int)ctx[0] & 0x0FFFFFFF;
    dst = *(unsigned int *)(dev + 0xD8) & 0x0FFFFFFF;

    if (D_002414AC == 3 || *(int *)(dev + 0xE0) == 0) {
        e0 = *(int *)(dev + 0xE0);
        srcStride = ctx[4] * 0x180;
        qwc = srcStride >> 4;
        if (e0 != 0)
            dstStride = (e0 >> 4) * 0x180;
        else
            dstStride = srcStride;
        passes = 1;
    } else {
        /* interlaced: two field passes */
        e0 = *(int *)(dev + 0xE0);
        dstStride = (e0 >> 4) * 0xC0;
        srcStride = (ctx[4] >> 1) * 0x180;
        qwc = srcStride >> 4;
        passes = 2;
    }

    for (i = 0; i < passes; i++) {
        cur = dst;
        for (j = 0; j < ctx[3]; j++) {
            *(volatile unsigned int *)0x1000D480 = 0;
            *(volatile unsigned int *)0x1000D410 = src;
            *(volatile unsigned int *)0x1000D420 = qwc;
            *(volatile unsigned int *)0x1000D400 = 0x101;
            while (*(volatile unsigned int *)0x1000D400 & 0x100)
                ;
            *(volatile unsigned int *)0x1000D080 = 0;
            *(volatile unsigned int *)0x1000D010 = cur;
            *(volatile unsigned int *)0x1000D020 = qwc;
            *(volatile unsigned int *)0x1000D000 = 0x100;
            while (*(volatile unsigned int *)0x1000D000 & 0x100)
                ;
            while (*(volatile unsigned int *)0x1000D020)
                ;
            cur += dstStride;
            src += srcStride;
        }
        dst += *(int *)(dev + 0xE4) * 0xC0;
    }
}
