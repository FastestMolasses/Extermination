// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: p = *(a0+0x40); p->[0x78] = a1 (64-bit); p->[0x70] = 1.
struct S109B98 { unsigned char pad[0x70]; int flag; int _pad; long handle; };

void func_00109B98(struct S109B98 **a0, long a1) {
    struct S109B98 *p = a0[0x10];
    p->flag = 1;
    p->handle = a1;
}
