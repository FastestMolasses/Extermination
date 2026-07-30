// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Three-state (0/1/2) actor script step: seeds a random orientation and pushes a camera/effect
// vector on entry, waits for a distance threshold, then resets state and fires the finish effect.

extern void func_00102948(float *dst, char *src);
extern int func_00122BB8(void);
extern void func_001EFD20(int a, float *b);
extern void func_001EFD90(int a, float *b, float *c);
extern void func_001FBD50(char *a, int b, int c, float d);
extern char D_00245C60;
extern float D_700038A0;
extern float D_700038B0;

void func_0014C490(char *arg0, char *arg1) {
    unsigned char st;

    *(int *)(arg1 + 0x18) = 0xF0;
    *(char *)(arg1 + 0x41) = -1;
    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(char *)(arg0 + 6) = st + 1;
        *(float *)(arg1 + 0x24) = 0.02f;
        *(int *)(arg0 + 0xC0) = 0xBFC90FDB;
        *(int *)(arg0 + 0xC4) = 0;
        *(int *)(arg0 + 0xC8) = 0;
        *(int *)(arg0 + 0xCC) = 0;
        func_00102948((float *)(arg0 + 0xB0), &D_00245C60 + ((func_00122BB8() >> 9 & 7) * 0x10));
        *(float *)(arg1 + 0x38) = *(float *)(arg0 + 0xB4);
        func_00102948(&D_700038A0, arg0 + 0xB0);
        *(volatile float *)0x700038A4 += 4.0f;
        func_001EFD20(4, &D_700038A0);
        break;
    case 1:
        if (!(*(float *)(arg0 + 0xB4) < 3.5f + *(float *)(arg1 + 0x38))) {
            *(char *)(arg0 + 6) = st + 1;
            *(int *)(arg1 + 0x28) = 0xB8D1B717;
        }
        break;
    case 2:
        if (*(float *)(arg1 + 0x24) <= 0.002f) {
            *(char *)(arg0 + 5) = 1;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x41) = 0x64;
            func_00102948(&D_700038A0, arg0 + 0xB0);
            *(volatile int *)0x700038B8 = 0;
            *(volatile int *)0x700038B4 = 0;
            *(volatile int *)0x700038B0 = 0;
            *(volatile float *)0x700038BC = 1.0f;
            *(volatile float *)0x700038B0 = -1.5707964f;
            func_001EFD90(3, &D_700038A0, &D_700038B0);
            func_001FBD50(arg0, 0x874, 0, 500.0f);
        }
        break;
    }
}
