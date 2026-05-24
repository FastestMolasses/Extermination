// CFLAGS: -O4,p -sdatathreshold 0
extern void func_0011A070(void);

void func_0016C520(char *a0) {
    char *s0 = a0;
    signed char c = a0[0x31B];
    if (c == -1) return;
    func_0011A070();
    s0[0x31B] = -1;
    s0[0x31A] = 0;
}
