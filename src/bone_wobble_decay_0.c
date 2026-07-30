// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275B40;

int bone_wobble_decay_0(unsigned char *arg0) {
    int i;
    float f2;
    int byteoff;

    *(float *)(arg0 + 0x10) -= 0.2f;
    if (*(float *)(arg0 + 0x10) < 0.0f) {
        *(int *)(arg0 + 0x10) = 0;
    }
    i = 1;
    f2 = 2.5f;
    byteoff = 4;
    for (; i < 0x11; i++) {
        *(float *)(*(int *)((char *)D_00275B40 + byteoff) + 0x80) = *(float *)(arg0 + 0x10);
        if (*(float *)(arg0 + 0x10) <= f2) {
            *(short *)(*(int *)((char *)D_00275B40 + byteoff) + 0x88) = 0x1000;
            *(short *)(*(int *)((char *)D_00275B40 + byteoff) + 0x8A) = 0x1000;
            *(short *)(*(int *)((char *)D_00275B40 + byteoff) + 0x8C) = 0x1000;
        }
        f2 += 2.5f;
        byteoff += 4;
    }
    return (*(float *)(arg0 + 0x10) == 0.0f) ? 1 : 0;
}
