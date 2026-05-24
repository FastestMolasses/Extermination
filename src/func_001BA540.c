// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001CA770(char *);
extern void func_001D8BF0(char *, int);

void func_001BA540(char *a0) {
    char *s0;
    if (*(short *)(a0 + 0x56) != 0) {
        func_001CA770(a0);
    }
    func_001D8BF0(a0, 0);
}
