// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00111018(int, int, char *);

void func_001B6250(unsigned char *a0) {
    if (a0[0x12] == 0) return;
    unsigned char *a2 = a0;
    if (a2[0x16] == 0) return;
    a2[0x16] = 0;
    *(short *)(a2 + 0x28) = 0;
    a2[0x18] = 0;
    a2[0x19] = 0;
    func_00111018(*(int *)(a2 + 4), *(int *)(a2 + 8), (char *)(a2 + 0x18));
}
