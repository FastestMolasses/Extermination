// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001026A0(int, int, int);
extern int func_0019AD00(int, int, int);
extern int D_700038A0;
extern int D_700038B0;

int func_001818D0(int a0) {
    int r;
    *(volatile int *)(0x700038A0) = 0;
    *(volatile int *)(0x700038A4) = 0;
    *(volatile int *)(0x700038A8) = 0x41A00000;
    *(volatile int *)(0x700038AC) = 0x3F800000;
    func_001026A0((int)&D_700038B0, a0 + 0xD0, (int)&D_700038A0);
    r = func_0019AD00(a0, (int)&D_700038B0, 7);
    {
        int v1 = 1;
        v1 = r ? v1 : 0;
        return v1;
    }
}
