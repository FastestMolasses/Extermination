// CFLAGS: -O4,p -sdatathreshold 4
extern int D_0028A5A0[2];
extern int *func_001C6120(int a0, int a1);
extern char *D_00275670;

void func_001D52E0(void) {
    int *v0 = func_001C6120(D_0028A5A0[0], 0);
    char *a0 = (char *)v0 + 0x20;
    *(int *)(D_00275670 + 0x144) = v0[0];
    *(int *)(D_00275670 + 0x148) = v0[1];
    *(float *)(D_00275670 + 0x150) = *(float *)((char *)v0 + 0x8);
    *(float *)(D_00275670 + 0x154) = *(float *)((char *)v0 + 0xC);
    *(float *)(D_00275670 + 0x158) = *(float *)((char *)v0 + 0x10);
    *(float *)(D_00275670 + 0x15C) = *(float *)((char *)v0 + 0x14);
    *(float *)(D_00275670 + 0x160) = *(float *)((char *)v0 + 0x18);
    *(float *)(D_00275670 + 0x164) = *(float *)((char *)v0 + 0x1C);
    *(char **)(D_00275670 + 0x140) = a0;
}
