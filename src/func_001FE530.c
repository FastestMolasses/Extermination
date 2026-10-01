// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Message text line copy. Skips to line number line (lines end at '\n' or
// '\f'; strlen through func_001232E0), copies that line into dst (when dst
// is not null, zero-terminated) and returns a pointer just past the line's
// terminator (at the final zero byte for the last line).
extern unsigned int func_001232E0(unsigned char *s);

unsigned char *func_001FE530(unsigned char *dst, unsigned char *s, int line) {
    int nl = 0;
    unsigned int n;
    unsigned int i;
    int k;
    int c;

    if (line != 0) {
        n = func_001232E0(s);
        for (i = 0; i < n; i++) {
            if ((s[i] == '\n' || s[i] == '\f') && ++nl == line) {
                s += i + 1;
                break;
            }
        }
    }
    c = *s;
    k = 0;
    while (c != '\n' && c != '\f' && c != 0) {
        if (dst != 0) {
            dst[k] = s[k];
        }
        k++;
        c = s[k];
    }
    if (dst != 0) {
        dst[k] = 0;
    }
    if (c == 0) {
        return s + k;
    }
    return s + (k + 1);
}
