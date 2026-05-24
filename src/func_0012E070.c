// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_0081083C;

void func_0012E070(char *a0) {
    int a1;
    int v1;
    int a2;
    a1 = *(short *)(a0 + 0xf6);
    v1 = a1 & 0x80;
    if (v1 == 0) {
        return;
    }
    v1 = D_0081083C;
    a2 = a1 & 0x7;
    a2 = 1 << a2;
    a2 = ~a2;
    v1 = v1 & a2;
    D_0081083C = v1;
    *(short *)(a0 + 0xf6) = 0;
}
