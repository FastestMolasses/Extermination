// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char *func_001FE530(int, char *, int);

int func_001FE660(char *arg0) {
    int count;
    char *p;

    p = arg0;
    count = 0;
    while (*p != 0) {
        p = func_001FE530(0, p, 0);
        count += 1;
    }
    return count;
}
