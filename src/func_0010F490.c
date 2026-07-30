// COMPILER: eegcc
// CFLAGS: -O2

struct SifCmdBuf {
    int len0;                   /* 0x00 */
    int len1;                   /* 0x04 */
    unsigned char *dst0;        /* 0x08 */
    unsigned char *dst1;        /* 0x0C */
    unsigned char buf0[0x40];   /* 0x10 */
    unsigned char buf1[0x40];   /* 0x50 */
};

void func_0010F490(unsigned int arg)
{
    struct SifCmdBuf *p = (struct SifCmdBuf *)(arg | 0x20000000);
    unsigned char *dst;
    int i;

    if (p->len0 > 0) {
        dst = p->dst0;
        for (i = 0; i < p->len0; i++) {
            dst[i] = p->buf0[i];
        }
    }
    if (p->len1 > 0) {
        dst = p->dst1;
        for (i = 0; i < p->len1; i++) {
            dst[i] = p->buf1[i];
        }
    }
}
