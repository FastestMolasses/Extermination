// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned int func_001232E0(void *s);
extern int block_copy(void *dst, void *src, unsigned int n);

extern char D_0027B680;

void func_00114BA8(char *dst) {
    char *src;
    unsigned int n;

    if (dst == 0) {
        return;
    }

    src = (char *)((unsigned int)&D_0027B680 | 0x20000000);
    n = func_001232E0(src);
    if (n < 0x400) {
        n = func_001232E0(src);
    } else {
        n = 0x3FF;
    }
    block_copy(dst, (char *)((unsigned int)&D_0027B680 | 0x20000000), n);
    *(dst + n) = '\0';
}
