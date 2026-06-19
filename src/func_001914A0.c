// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008105D0[1];
extern short D_0028A9A0;
extern void func_00191580(void *, void *);
extern void func_0018C4B0(void *, float, float);
extern void func_0018C6A0(void *, void *, float);

void func_001914A0(unsigned char *a0, unsigned char *a1) {
    unsigned char *s1 = a0;
    unsigned char *s0 = a1;
    func_00191580(s1, s0);
    func_0018C4B0(D_008105D0, *(float *)(s1 + 0x14), 0.4f);
    func_0018C6A0(s1 + 0x10, D_008105D0, 0.4f);
    if (D_0028A9A0 == 0 && s0[4] != 5) {
        s1[6] = 0;
    }
}
