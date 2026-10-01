// COMPILER: eegcc
// CFLAGS: -O2
// newlib strcspn: length of the leading run of s1 that holds no byte of s2.

typedef unsigned int size_t;

size_t func_00123280(const char *s1, const char *s2) {
    const char *s = s1;
    const char *c;

    while (*s1) {
        for (c = s2; *c; c++) {
            if (*s1 == *c)
                break;
        }
        if (*c)
            break;
        s1++;
    }
    return s1 - s;
}
