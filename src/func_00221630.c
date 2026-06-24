// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-state driver dispatched on the state byte at arg0+6 (switch 0/1/2).
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00102948(void *dst, void *src);
extern int func_0019AFE0(void *p, void *a, void *b, int n);
extern int **D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00221630(unsigned char *arg0) {
    unsigned char st;
    float t;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x39, 0, 0.0f);
        *(float *)(arg0 + 0x2E4) = 0.0f;
        func_00102948(arg0 + 0x290, arg0 + 0xB0);
        break;
    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001749A0(arg0, 0x3A, 0, 0.0f);
        } else {
            *(float *)(arg0 + 0x2EC) = *(float *)((char *)*D_00275B40 + 4) - *(float *)(arg0 + 0x2E4);
            *(float *)(arg0 + 0x2E4) = *(float *)((char *)*D_00275B40 + 4);
            *(float *)(arg0 + 0xB4) += *(float *)(arg0 + 0x2EC);
        }
        func_00102948(D_700038A0, arg0 + 0x290);
        func_00102948(D_700038B0, arg0 + 0xB0);
        t = 10.0f + *(float *)0x700038B4;
        *(float *)0x700038B4 = t;
        *(float *)0x700038A4 = t;
        if (func_0019AFE0(arg0, D_700038A0, D_700038B0, 6) != 0) {
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + 4.5f * *(float *)(*(int *)0x700031D0 + 0x24);
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + 4.5f * *(float *)(*(int *)0x700031D0 + 0x2C);
        }
        break;
    case 2:
        break;
    }
}
