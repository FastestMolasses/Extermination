// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00199DB0(void *);
extern int D_700038A0;

int func_00178440(char *a0) {
    int *v1;
    float f;
    func_00199DB0(&D_700038A0);
    v1 = *(int **)0x700031D0;
    f = *(float *)0x700038A0 + 2.5f * *(float *)((char *)v1 + 0x24);
    *(float *)(a0 + 0x2E0) = f;
    v1 = *(int **)0x700031D0;
    f = *(float *)0x700038A8 + 2.5f * *(float *)((char *)v1 + 0x24);
    *(float *)(a0 + 0x2E8) = f;
    *(float *)(a0 + 0x2E4) = 3.0f + *(float *)(a0 + 0xB4);
    *(float *)(a0 + 0x218) = *(float *)(a0 + 0xC4);
    return 1;
}
