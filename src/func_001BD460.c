// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern int **D_00275B40;

int func_001BD460(unsigned char *a0) {
    float f14;
    float f10;

    f14 = *(float *)(a0 + 0x14) - 0.2f;
    *(float *)(a0 + 0x14) = f14;
    if (f14 <= 2.0f) {
        if (f14 <= 0.0f) {
            *(int *)(a0 + 0x14) = 0;
        }
        f10 = *(float *)(a0 + 0x10) - 0.2f;
        *(float *)(a0 + 0x10) = f10;
        if (f10 <= 0.0f) {
            *(int *)(a0 + 0x10) = 0;
        }
    }
    *(float *)((char *)D_00275B40[1] + 0x7C) = -*(float *)(a0 + 0x14);
    *(float *)((char *)D_00275B40[2] + 0x7C) = -*(float *)(a0 + 0x10) - *(float *)(a0 + 0x14);
    *(float *)((char *)D_00275B40[3] + 0x7C) = *(float *)(a0 + 0x10) + *(float *)(a0 + 0x14);
    *(float *)((char *)D_00275B40[4] + 0x7C) = *(float *)(a0 + 0x14);
    if (*(float *)(a0 + 0x10) == 0.0f) {
        return 1;
    }
    return 0;
}
