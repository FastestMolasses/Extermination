// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern int func_001EFF10(int, int, void *, void *, void *, void *, float);
extern unsigned char D_008104A0;
extern int D_700038A0;
extern int D_700038B0;
extern int D_700038C0;
extern int D_700038D0;

void func_00189D30(unsigned char *arg0) {
    unsigned char state;

    state = arg0[7];
    switch (state) {
    case 0:
        if (arg0[0] == 1) {
            arg0[7] = (unsigned char)(state + 1);
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0;
            *(int *)0x700038A8 = 0;
            *(int *)0x700038B0 = 0;
            *(int *)0x700038D0 = 0;
            *(int *)0x700038D4 = 0;
            *(int *)0x700038D8 = 0;
            *(int *)0x700038DC = 0;
            *(float *)0x700038AC = 1.0f;
            *(float *)0x700038B4 = 5.0f;
            *(int *)0x700038B8 = 0;
            *(float *)0x700038BC = 1.0f;
            *(float *)0x700038C0 = 64.0f;
            *(float *)0x700038C4 = 64.0f;
            *(float *)0x700038C8 = 64.0f;
            *(float *)0x700038CC = 128.0f;
            *(int *)(arg0 + 0x20) = func_001EFF10(0x8000000D, *(int *)(*(int *)(arg0 + 0x14) + 0x110) + 0x90, &D_700038A0, &D_700038B0, &D_700038C0, &D_700038D0, 10.0f);
        }
        break;
    case 1:
        if (arg0[0] == 2) {
            *(signed char *)(*(int *)(arg0 + 0x20) + 4) = 2;
            arg0[7] = (unsigned char)(arg0[7] - 1);
        } else if (D_008104A0 != 0x36 && D_008104A0 != 0x37) {
            *(signed char *)(*(int *)(arg0 + 0x20) + 4) = 2;
            arg0[0] = 2;
            arg0[7] = (unsigned char)(arg0[7] - 1);
        }
        break;
    }
}
