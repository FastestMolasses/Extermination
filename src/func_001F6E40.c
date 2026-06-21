// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Acquire a handle; if non-null, run two operations on it.
extern void *func_001F6D60(void);
extern void func_001F66F0(void *);
extern void func_001F6640(void *);

void func_001F6E40(void) {
    void *p = func_001F6D60();
    if (p) {
        func_001F66F0(p);
        func_001F6640(p);
    }
}
