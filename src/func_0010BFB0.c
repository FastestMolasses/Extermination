// COMPILER: eegcc
// CFLAGS: -O2
extern void _print(char *buf);

void func_0010BFB0(unsigned int a0) {
    char buf[8 + 8];
    char *p = buf;
    int i;
    for (i = 7; i >= 0; i--) {
        int nib = (a0 >> (i * 4)) & 0xF;
        if (nib < 10) {
            *p = nib + 0x30;
        } else {
            *p = nib + 0x57;
        }
        p++;
    }
    *p = 0;
    _print(buf);
}
