// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_0028B020[];
extern void func_001AF800(unsigned char *);

void func_001AFEB0(void) {
    unsigned char *s0 = D_0028B020;
    int s1 = 0;
    do {
        if (s0[0] != 0) {
            func_001AF800(s0);
        }
        s1++;
        s0 += 0x2F0;
    } while (s1 < 0x18);
}
