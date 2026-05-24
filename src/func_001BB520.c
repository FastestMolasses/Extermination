extern int D_002755E8;
extern void func_001B0FD0(int *);

void func_001BB520(int *a0) {
    int *s0;
    unsigned char v1;
    s0 = a0;
    func_001B0FD0(s0);
    *(int **)((char *)s0 + 0x30) = &D_002755E8;
    v1 = *(unsigned char *)((char *)s0 + 0x2E);
    *(short *)((char *)s0 + 0x34) = v1;
    *(short *)((char *)s0 + 0x2E) = 0;
}
