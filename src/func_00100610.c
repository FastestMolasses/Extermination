// COMPILER: eegcc
// CFLAGS: -O2
extern long *func_00100268(void);

short func_00100610(short a0, short a1, short a2) {
    long *p;
    int h, w;
    p = func_00100268();
    h = (a1 + 0x3F) / 64;
    if (a0 & 2)
        w = (a2 + 0x3F) / 64;
    else
        w = (a2 + 0x1F) / 32;
    if ((*p & 0xFFFF0000FFFFL) == 1)
        return (short)(h * w);
    return (short)(h * w * 2);
}
