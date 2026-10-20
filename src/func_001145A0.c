// COMPILER: eegcc
// CFLAGS: -O2
//
// Through the uncached alias (a0 | 0x20000000), copies n0 bytes from +0x10 to
// dst0 and n1 bytes from +0x50 to dst1 (counts and destinations at
// +0/+4/+8/+0xC).
// Byte-matched from C with ee-gcc 2.9-991111-01 (decomp lane round 3, 2026-10-20).

typedef struct {
    int n0;
    int n1;
    char *dst0;
    char *dst1;
    char buf0[0x40];
    char buf1[0x40];
} CopyPair;

void func_001145A0(int a0) {
    CopyPair *p = (CopyPair *)(a0 | 0x20000000);
    char *d;
    int i;

    if (p->n0 != 0) {
        d = p->dst0;
        for (i = 0; i < p->n0; i++) {
            *d++ = p->buf0[i];
        }
    }
    if (p->n1 != 0) {
        d = p->dst1;
        for (i = 0; i < p->n1; i++) {
            *d++ = p->buf1[i];
        }
    }
}
