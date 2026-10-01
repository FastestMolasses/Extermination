// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern char *func_001AFA90(int);
extern void func_001F5040(char *node);  /* the node's per-frame worker (node + 0x10) */

char *func_001F4F40(int kind) {
    char *n;

    n = func_001AFA90(0xC);
    if (n == 0) return 0;
    n[0xD] = kind;
    *(void (**)(char *))(n + 0x10) = func_001F5040;
    return n;
}
