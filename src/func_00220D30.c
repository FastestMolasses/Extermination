// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Cutscene/actor camera-shot driver dispatched on the state byte at arg0+6: state 0 kicks
// anim clip 0x3C and snaps the pose, states 1/2 build a look-at basis from the global
// camera vector (state 2 biases it by the stored offsets at +0x2E0/+0x2E8), push it through
// the matrix helpers, raise the target height by 10.0f, and on a successful trace fix the
// actor position from the 4.5f-scaled camera transform; state 1 additionally latches the
// offsets and advances to clip 0x3D when flag 0x1000 of +0x200 is set.
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern int func_001026A0(char *dst, void *src, char *m);
extern void func_00102948(void *dst, void *src);
extern int func_001028B8(void *a, void *b, void *c);
extern int func_0019AFE0(void *p, void *a, void *b, int n);
extern int **D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00220D30(unsigned char *arg0) {
    unsigned char st;
    float t;
    int *q;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001749A0(arg0, 0x3C, 0, 0.0f);
        func_00102948(arg0 + 0x290, arg0 + 0xB0);
        break;
    case 1:
        q = (int *)D_00275B40;
        *(float *)0x700038A0 = *(float *)(*q + 0);
        *(float *)0x700038A4 = *(float *)(*q + 4);
        *(float *)0x700038A8 = *(float *)(*q + 8);
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, arg0 + 0x290, D_700038B0);
        func_00102948(D_700038A0, arg0 + 0x290);
        func_00102948(D_700038B0, arg0 + 0xB0);
        t = 10.0f + *(float *)0x700038B4;
        *(float *)0x700038B4 = t;
        *(float *)0x700038A4 = t;
        if (func_0019AFE0(arg0, D_700038A0, D_700038B0, 6) != 0) {
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + 4.5f * *(float *)(*(int *)0x700031D0 + 0x24);
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + 4.5f * *(float *)(*(int *)0x700031D0 + 0x2C);
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001749A0(arg0, 0x3D, 0, 1.0f);
            *(float *)(arg0 + 0x2E0) = *(float *)(*(int *)D_00275B40 + 0);
            *(float *)(arg0 + 0x2E8) = *(float *)(*(int *)D_00275B40 + 8);
        }
        break;
    case 2:
        q = (int *)D_00275B40;
        *(float *)0x700038A0 = *(float *)(*q + 0) + *(float *)(arg0 + 0x2E0);
        *(float *)0x700038A4 = *(float *)(*q + 4);
        *(float *)0x700038A8 = *(float *)(*q + 8) + *(float *)(arg0 + 0x2E8);
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, arg0 + 0x290, D_700038B0);
        func_00102948(D_700038A0, arg0 + 0x290);
        func_00102948(D_700038B0, arg0 + 0xB0);
        t = 10.0f + *(float *)0x700038B4;
        *(float *)0x700038B4 = t;
        *(float *)0x700038A4 = t;
        if (func_0019AFE0(arg0, D_700038A0, D_700038B0, 6) != 0) {
            *(float *)(arg0 + 0xB0) = *(float *)0x700031B0 + 4.5f * *(float *)(*(int *)0x700031D0 + 0x24);
            *(float *)(arg0 + 0xB8) = *(float *)0x700031B8 + 4.5f * *(float *)(*(int *)0x700031D0 + 0x2C);
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    }
}
