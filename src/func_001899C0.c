// CFLAGS: -O4,p -sdatathreshold 4
extern unsigned char D_008102B0[8];
extern int D_008103D0[2];
extern int *D_00275B40;
extern void func_00189630(void *, unsigned char *);
extern void func_00189730(unsigned char *);
extern void copy_qw4(void *, void *);
extern void func_001C9610(int *, int, void *);

void func_001899C0(void *a0) {
    void *s0;
    s0 = a0;
    func_00189630(s0, D_008102B0);
    func_00189730(D_008102B0);
    copy_qw4((char *)s0 + 0xd0, (void *)(D_008103D0[0] + 0x90));
    func_001C9610(D_00275B40, *(unsigned char *)((char *)s0 + 0xc), (char *)s0 + 0xd0);
}
